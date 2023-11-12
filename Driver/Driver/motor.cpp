#include "motor.hpp"

MOTOR::MOTOR(int pinIn1, int pinIn2)
{
  this->pinIn1 = pinIn1;
  this->pinIn2 = pinIn2;
  this->isGoingForward = false;
  this->isGoingBackward = false;
}

bool MOTOR::isGoing()
{
  return this->isGoingForward || this->isGoingBackward;
}

void MOTOR::forward()
{
  Serial.println("Motor: Forward");
  if (this->isGoingForward)
  {
    return;
  }

  if (this->isGoingBackward)
  {
    Serial.println("Motor.Forward: Stop motor");
    this->stop();
  }

  this->isGoingForward = true;
  digitalWrite(this->pinIn1, HIGH);
  digitalWrite(this->pinIn2, LOW);
}

void MOTOR::backward()
{
  Serial.println("Motor: Bacward");
  if (this->isGoingBackward)
  {
    return;
  }

  if (this->isGoingForward)
  {
    Serial.println("Motor.Bacward: Stop motor");
    this->stop();
  }

  isGoingBackward = true;
  digitalWrite(this->pinIn1, LOW);
  digitalWrite(this->pinIn2, HIGH);
}

void MOTOR::stop()
{
  Serial.println("Motor: Stop");

  digitalWrite(this->pinIn1, HIGH);
  digitalWrite(this->pinIn2, HIGH);

  isGoingForward = false;
  isGoingBackward = false;
}