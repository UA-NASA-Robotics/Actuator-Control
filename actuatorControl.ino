// Define pins for the IBT-2 motor driver
const int L_EN = 7; // ORANGE
const int LPWM = 5; // YELLOW
const int R_EN = 8; // BLUE
const int RPWM = 6; // GREEN

// Define pins for the Upper and Lower limit buttons
const int buttonUpperPin = 10; // Orange
const int buttonLowerPin = 11; // Orange

int desiredSpeed = 0;  // Default value if no input is received
int desiredPosition = 0; // Default value if no input is received

struct ButtonStates {
  int buttonUpperState;
  int buttonLowerState;
};

ButtonStates getButtonStates() {
  ButtonStates states;
  states.buttonUpperState = digitalRead(buttonUpperPin);
  states.buttonLowerState = digitalRead(buttonLowerPin);
  return states;
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
  // Set all the control pins as outputs
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);

  // For now these two are wired to be enabled, we could just jump them with the 5V power to hold them constant
  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);

  // Sets all the input pins for the limit switch buttons
  digitalWrite(buttonUpperPin, INPUT_PULLUP);
  digitalWrite(buttonLowerPin, INPUT_PULLUP);

  Serial.begin(9600);
}


void loop() {
ButtonStates buttonStates = getButtonStates();
  // Handle limit button conditions to modify desired speed
  if (desiredSpeed > 0 && buttonStates.buttonUpperState == 0) {
    desiredSpeed = 0;
  }

  if (desiredSpeed < 0 && buttonStates.buttonLowerState == 0) {
    desiredSpeed = 0;
  }

    // Use the speedDriveMotor function to drive the motor
    speedDriveMotor(desiredSpeed);

  if (Serial.available() > 0) {

    String input = Serial.readStringUntil('\n');
    parseSerialInput(input);
  }
// Send feedback via serial
  Serial.print("<");
  Serial.print("5");
  Serial.print(",");
  Serial.print(buttonStates.buttonUpperState);
  Serial.print(",");
  Serial.print(buttonStates.buttonLowerState);
  Serial.println(">");    
}