// Define pins for the IBT-2 motor driver
const int L_EN = 7; // ORANGE
const int LPWM = 5; // YELLOW
const int R_EN = 8; // BLUE
const int RPWM = 6; // GREEN

// Define pins for the Upper and Lower limit buttons
const int buttonUpperPin = 10; // Orange
const int buttonLowerPin = 11; // Orange

struct ButtonStates {
  int buttonUpperState;
  int buttonLowerState;
};

// Takes the ButtonStates initialized booleans and updates them based on pin input
ButtonStates getButtonStates() {
  ButtonStates states;
  states.buttonUpperState = digitalRead(buttonUpperPin);
  states.buttonLowerState = digitalRead(buttonLowerPin);
  return states;
}

void speedDriveMotor(int percentage) {
  // This ensures that the input into the function will alwasy be a valid percentage
  if (percentage > 100) percentage = 100;
  if (percentage < -100) percentage = -100;

  // Converts the percentage into a pwm value, has to be absolute value because PWM cannot be negative
  int pwmValue = map(abs(percentage),0,100,0,255);

  // If percentage is positive, drive forward at the converted pwm percentage
  if (percentage > 0) {
    analogWrite(LPWM,0);
    analogWrite(RPWM,pwmValue);
  }
  // If perentage is negative, drive backward at the converted pwm percentage
  else if (percentage < 0) {
    analogWrite(RPWM,0);
    analogWrite(LPWM,pwmValue);
  }
  // If percentage is zero, do nothing
  else {
    analogWrite(RPWM, 0);
    analogWrite(LPWM, 0);
  }
}

void setup() {
  // Set all the control pins as outputs
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);

  // For now these two are wired to be enable, we could just jump them with the 5v power to hold them constant
  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);

  // Sets all the input pins for the limit switch buttons
  digitalWrite(buttonUpperPin, INPUT_PULLUP);
  digitalWrite(buttonLowerPin, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  ButtonStates buttonStates = getButtonStates();

  int desiredSpeed = -100; // Change to be desired speed as you test

  if (desiredSpeed > 0 && buttonStates.buttonUpperState == 0) {
    desiredSpeed = 0;
  }

  if (desiredSpeed < 0 && buttonStates.buttonLowerState == 0) {
    desiredSpeed = 0;
  }

  speedDriveMotor(desiredSpeed);

  Serial.print("<");
  Serial.print("5");
  Serial.print(",");
  Serial.print(buttonStates.buttonUpperState);
  Serial.print(",");
  Serial.print(buttonStates.buttonLowerState);
  Serial.println(">");
}