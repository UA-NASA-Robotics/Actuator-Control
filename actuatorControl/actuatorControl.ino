// Define pins for the IBT-2 motor driver
const int L_EN = 7; // ORANGE
const int LPWM = 5; // YELLOW
const int R_EN = 8; // BLUE
const int RPWM = 6; // GREEN

// Define pins for the Upper and Lower limit buttons
const int buttonUpperPin = 10; // Orange
const int buttonLowerPin = 11; // Orange

// Define pins for the actuator encoders
const int ENC_A = 2;
const int ENC_B = 3;

volatile int counter = 0;

int desiredSpeed = 0;  // Default value if no input is received
int desiredPosition = 0; // Default value if no input is received

bool homed = false;
int homedEncoder = 0;
int length = 0;

struct ButtonStates {
  int buttonUpperState;
  int buttonLowerState;
};

ButtonStates getButtonStates() {
  return {
    digitalRead(buttonUpperPin),
    digitalRead(buttonLowerPin)
  };
}

void speedDriveMotor(int percentage) {
  // Ensures that the input into the function is always a valid percentage
  if (percentage > 100) percentage = 100;
  if (percentage < -100) percentage = -100;

  // Converts the percentage into a PWM value (absolute value because PWM cannot be negative)
  int pwmValue = map(abs(percentage), 0, 100, 0, 255);

  // If percentage is positive, drive forward at the converted PWM percentage
  if (percentage > 0) {
    analogWrite(LPWM, 0);
    analogWrite(RPWM, pwmValue);
  }
  // If percentage is negative, drive backward at the converted PWM percentage
  else if (percentage < 0) {
    analogWrite(RPWM, 0);
    analogWrite(LPWM, pwmValue);
  }
  // If percentage is zero, do nothing
  else {
    analogWrite(RPWM, 0);
    analogWrite(LPWM, 0);
  }
}

// Function to parse serial input "<integer, integer>" format
void parseSerialInput(String input) {
  int commaIndex = input.indexOf(',');
  if (commaIndex > 0) {
    String speedStr = input.substring(1, commaIndex);
    String positionStr = input.substring(commaIndex + 1, input.length() - 1); // Remove the '>'
    
    // Convert strings to integers
    if (speedStr != "None") desiredSpeed = speedStr.toInt();
    if (positionStr != "None") desiredPosition = positionStr.toInt();
  }
}

void setup() {
  // IBT-2 pins
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);
  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);

  // Button pins
  digitalWrite(buttonUpperPin, INPUT_PULLUP);
  digitalWrite(buttonLowerPin, INPUT_PULLUP);
  
  // Encoder pins
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), read_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), read_encoder, CHANGE);

  Serial.begin(115200);
}

void loop() {
  static unsigned long lastCommandTime = 0;
  static int lastCounter = 0;
  static unsigned long lastMovementCheckTime = 0;
  static bool isMoving = false;

  ButtonStates buttonStates = getButtonStates();
  static unsigned long forwardStartTime = 0;
  static bool checkingHomed = false;
  static bool commandReceived = false;

  // Check if serial input is available
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    parseSerialInput(input);

    // Command received, update timestamp and flag
    lastCommandTime = millis();
    commandReceived = true;
  }

  // Check for command timeout
  if (millis() - lastCommandTime > 1000) { //1 second command timeout
    commandReceived = false; // No valid command within timeout
    desiredSpeed = 0;        // Stop actuator for safety
  }

  // Handle limit button conditions to modify desired speed
  if (desiredSpeed > 0 && buttonStates.buttonUpperState == 0) {
    desiredSpeed = 0;
  }

  if (desiredSpeed < 0 && buttonStates.buttonLowerState == 0) {
    desiredSpeed = 0;
  }

  // Movement verification logic
  if (millis() - lastMovementCheckTime >= 500) { // 500 milliseconds timeout
    if (counter != lastCounter) {
      isMoving = true; // Encoder value has changed; actuator is moving
    } else {
      isMoving = false; // Encoder value hasn't changed; actuator is stationary
    }
    lastCounter = counter; // Update last counter for next check
    lastMovementCheckTime = millis(); // Update movement check timestamp
  }

  // Check if actuator is being commanded to go forward and system is receiving commands
  if (desiredSpeed > 0 && !checkingHomed && commandReceived) {
    forwardStartTime = millis();  // Start the timer
    checkingHomed = true;         // Begin homed check process
  }

  // Check if 1 second has passed while going forward
  if (checkingHomed && millis() - forwardStartTime >= 1000) {      
    if (!isMoving && commandReceived && desiredSpeed > 0) {
        homed = true;  // Encoder hasn't changed; actuator is homed
        homedEncoder = counter;
      }
      checkingHomed = false;  // Reset the check
  }

  if (homed) {
    const int lengthExtended = 321; //mm
    const int lengthRetracted = 217; //mm
    const int pulsesPerTravel = 17.4; // pulses per mm of travel    

    length = round(lengthExtended - (homedEncoder - counter) * pulsesPerTravel / );
  }

  // Use the speedDriveMotor function to drive the motor
  speedDriveMotor(desiredSpeed);

  // Send feedback via serial
  Serial.print("<");
  Serial.print(counter);
  Serial.print(",");
  Serial.print(buttonStates.buttonUpperState);
  Serial.print(",");
  Serial.print(buttonStates.buttonLowerState);
  Serial.print(",");
  Serial.print(homed);
  Serial.print(",");
  Serial.print(homedEncoder);
  Serial.print(",");
  Serial.print(length);
  Serial.println(">");    
}

void read_encoder() {
  // Encoder interrupt routine for both pins. Updates counter
  // if they are valid and have rotated a full indent
 
  static uint8_t old_AB = 3;  // Lookup table index
  static int8_t encval = 0;   // Encoder value  
  static const int8_t enc_states[]  = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; // Lookup table

  old_AB <<=2;  // Remember previous state

  if (digitalRead(ENC_A)) old_AB |= 0x02; // Add current state of pin A
  if (digitalRead(ENC_B)) old_AB |= 0x01; // Add current state of pin B
  
  encval += enc_states[( old_AB & 0x0f )];

  // Update counter if encoder has rotated a full indent, that is at least 4 steps
  if( encval > 3 ) {        // Four steps forward
    int changevalue = 1;
    counter = counter + changevalue;              // Update counter
    encval = 0;
  }
  else if( encval < -3 ) {        // Four steps backward
    int changevalue = -1;
    counter = counter + changevalue;              // Update counter
    encval = 0;
  }
} 