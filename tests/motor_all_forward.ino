/*
  Batman — Motor sanity test
  Spins all four motors forward at a moderate PWM.
  Verify wheels are lifted and direction is correct.
*/

// HW-166 #1
const int M1_PWMA = 3;
const int M1_AIN1 = 2;
const int M1_AIN2 = 4;
const int M1_PWMB = 5;
const int M1_BIN1 = A2;
const int M1_BIN2 = A3;

// HW-166 #2
const int M2_PWMA = 6;
const int M2_AIN1 = 7;
const int M2_AIN2 = 8;
const int M2_PWMB = 11;
const int M2_BIN1 = 12;
const int M2_BIN2 = 13;

void setup() {
  pinMode(M1_AIN1, OUTPUT); pinMode(M1_AIN2, OUTPUT);
  pinMode(M1_BIN1, OUTPUT); pinMode(M1_BIN2, OUTPUT);
  pinMode(M2_AIN1, OUTPUT); pinMode(M2_AIN2, OUTPUT);
  pinMode(M2_BIN1, OUTPUT); pinMode(M2_BIN2, OUTPUT);

  // Forward direction: IN1=HIGH, IN2=LOW
  digitalWrite(M1_AIN1, HIGH); digitalWrite(M1_AIN2, LOW);
  digitalWrite(M1_BIN1, HIGH); digitalWrite(M1_BIN2, LOW);
  digitalWrite(M2_AIN1, HIGH); digitalWrite(M2_AIN2, LOW);
  digitalWrite(M2_BIN1, HIGH); digitalWrite(M2_BIN2, LOW);
}

void loop() {
  analogWrite(M1_PWMA, 140);
  analogWrite(M1_PWMB, 140);
  analogWrite(M2_PWMA, 140);
  analogWrite(M2_PWMB, 140);
  delay(3000);

  // stop
  digitalWrite(M1_AIN1, LOW); digitalWrite(M1_AIN2, LOW);
  digitalWrite(M1_BIN1, LOW); digitalWrite(M1_BIN2, LOW);
  digitalWrite(M2_AIN1, LOW); digitalWrite(M2_AIN2, LOW);
  digitalWrite(M2_BIN1, LOW); digitalWrite(M2_BIN2, LOW);
  analogWrite(M1_PWMA, 0); analogWrite(M1_PWMB, 0);
  analogWrite(M2_PWMA, 0); analogWrite(M2_PWMB, 0);
  while (true) { /* halt */ }
}

