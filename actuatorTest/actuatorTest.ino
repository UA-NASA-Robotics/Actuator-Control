// This code is simply here to run to verify that the IBT-2 is working and you are capable of driving a motor
const int L_EN = 7; // ORANGE
const int LPWM = 5; // YELLOW
const int R_EN = 8; // BLUE
const int RPWM = 6; // GREEN

void setup() {
  // Set all the control pins as outputs
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);

  // Enable the motor driver
  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);
}

void loop() {
  // Spin motor forward at full speed
  analogWrite(RPWM, 255); // Full speed forward
  analogWrite(LPWM, 0);   // Ensure LPWM is low
  delay(2000);             // Run for 2 seconds

  // Spin motor in reverse at half speed
  analogWrite(RPWM, 0);    // Ensure RPWM is low
  analogWrite(LPWM, 127);  // Half speed reverse
  delay(2000);             // Run for 2 seconds

  // Stop the motor
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
  delay(2000);             // Stop for 2 seconds
}