#include "motor.h"

MOTOR::MOTOR(int pinIn1, int pinIn2) {
  this->pinIn1 = pinIn1;
  this->pinIn2 = pinIn2;
  isForward = false;
  isBackward = false;
}
bool MOTOR::isGoing() {
  return isForward || isBackward;
}

void MOTOR::forward() {
  Serial.println("Motor: Forward");

  isForward = true;
  digitalWrite(this->pinIn1, HIGH);
  digitalWrite(this->pinIn2, LOW);
}

void MOTOR::backward() {
  Serial.println("Motor: Bacward");

  isBackward = true;
  digitalWrite(this->pinIn1, LOW);
  digitalWrite(this->pinIn2, HIGH);
}

void MOTOR::stop() {
  Serial.println("Motor: Stop");
  // digitalWrite(this->pinIn1, HIGH);
  // digitalWrite(this->pinIn2, HIGH);

  // delay(5);

  digitalWrite(this->pinIn1, LOW);
  digitalWrite(this->pinIn2, LOW);

  isForward = false;
  isBackward = false;
}