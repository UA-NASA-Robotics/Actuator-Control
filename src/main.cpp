#include <Arduino.h>

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

int desiredSpeed    = 0;  // Default value if no input is received
int desiredPosition = 0;  // Default value if no input is received

bool isHomed        = false;
int  homedEncoder = 0;
int  length       = 0;

struct ButtonStates {
  int isButtonUpperReleased;
  int isButtonLowerReleased;
};


static unsigned long forwardStartTime = 0;
static bool isCheckingHomed = false;
static bool isCommandReceived = false;

static unsigned long lastCommandTime = 0;
static int lastCounter = 0;
static unsigned long lastMovementCheckTime = 0;
static bool isMoving = false;

const int lengthExtended = 321;    //mm
                                   // const int lengthRetracted = 217; //mm
const int pulsesPerTravel = 17.4;  // pulses per mm of travel    

ButtonStates getButtonStates() {
  return {
    digitalRead(buttonUpperPin),
    digitalRead(buttonLowerPin)
  };
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

void speedDriveMotor(int speed) {
  // Ensures that the input into the function is always a valid speed
  if (speed > 1) speed = 1;
  if (speed < -1) speed = -1;
  if (speed < 1 && speed > -1) speed = 0;

  // If percentage is positive, drive forward at max power.
  if (speed == 1) {
    analogWrite(LPWM, 0);
    analogWrite(RPWM, 255);
  }
  // If percentage is negative, drive backward at max power
  else if (speed == -1) {
    analogWrite(RPWM, 0);
    analogWrite(LPWM, 255);
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

int buttonLimitLogic(struct ButtonStates buttonStates, int desiredSpeed) {
    if (desiredSpeed > 0 && buttonStates.isButtonUpperReleased == 0) {
        desiredSpeed = 0; // Prevent upward motion if the upper limit is not pressed
    }
    if (desiredSpeed < 0 && buttonStates.isButtonLowerReleased == 0) {
        desiredSpeed = 0; // Prevent downward motion if the lower limit is not pressed
    }
    return desiredSpeed;
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
  // Gets booleans of each button
  ButtonStates buttonStates = getButtonStates();

  // Check if serial input is available
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    parseSerialInput(input);

    // Command received, update timestamp and flag command received
    lastCommandTime = millis();
    isCommandReceived = true;
  }

  // Handle limit button conditions to modify desired speed, sets speedto zero if either button is pressed
  desiredSpeed = buttonLimitLogic(buttonStates, desiredSpeed);

  // Movement verification logic
  if (millis() - lastMovementCheckTime >= 500) { // 500 milliseconds timeout
    isMoving              = (counter != lastCounter);  // If current encoder doesn't equal last encoder, it has to be moving
    lastCounter           = counter;                   // Update last counter for next check
    lastMovementCheckTime = millis();                  // Update movement check timestamp
  }

  // Check if actuator is being commanded to go forward and system is receiving commands
  if (desiredSpeed > 0 && !isCheckingHomed && isCommandReceived) {
    forwardStartTime = millis();  // Start the timer
    isCheckingHomed  = true;      // Begin homed check process
  }

  // Check if 1 second has passed while going forward
  if (isCheckingHomed && millis() - forwardStartTime >= 1000) {      
    if (!isMoving && isCommandReceived && desiredSpeed > 0) {
        isHomed      = true;     // Encoder hasn't changed; actuator is homed
        homedEncoder = counter;  //Records the value of the counter when it is homed
      }
      isCheckingHomed = false;  // Reset the check
  }

  if (isHomed) length = round(lengthExtended - (homedEncoder - counter) / pulsesPerTravel);

  // Use the speedDriveMotor function to drive the motor
  speedDriveMotor(desiredSpeed);

  // Send feedback via serial
  Serial.print("<");
  Serial.print(counter);
  Serial.print(",");
  Serial.print(buttonStates.isButtonUpperReleased);
  Serial.print(",");
  Serial.print(buttonStates.isButtonLowerReleased);
  Serial.print(",");
  Serial.print(isHomed);
  Serial.print(",");
  Serial.print(homedEncoder);
  Serial.print(",");
  Serial.print(length);
  Serial.println(">");    
}