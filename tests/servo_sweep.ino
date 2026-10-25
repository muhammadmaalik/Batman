/*
  Batman — Servo sweep test
  Sweeps base, arm, and grip servos between 30..150 deg.
  Ensure servos are unobstructed before running.
*/

#include <Servo.h>

const int SERVO_BASE = 9;
const int SERVO_ARM  = 10;
const int SERVO_GRIP = A0; // digital 14

Servo sBase, sArm, sGrip;

void setup() {
  sBase.attach(SERVO_BASE);
  sArm.attach(SERVO_ARM);
  sGrip.attach(SERVO_GRIP);
}

void sweepServo(Servo& s, int minA, int maxA, int stepDelay) {
  for (int a = minA; a <= maxA; a += 2) { s.write(a); delay(stepDelay); }
  for (int a = maxA; a >= minA; a -= 2) { s.write(a); delay(stepDelay); }
}

void loop() {
  sweepServo(sBase, 30, 150, 10);
  sweepServo(sArm,  30, 150, 10);
  sweepServo(sGrip, 10, 120, 10);
}

