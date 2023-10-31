#include "Curtains.h"

Curtains::Curtains(MOTOR* mtr, int pinClosed, int pinOpened) {
  this->mtr = mtr;
  this->pinClosed = pinClosed;
  this->pinOpened = pinOpened;

  operationGoing = false;
  isNeedClosing = false;
  isNeedOpening = false;
}

void Curtains::setNeedClosing() {
  isNeedClosing = true;
  isNeedOpening = false;
}

void Curtains::setNeedOpening() {
  isNeedOpening = true;
  isNeedClosing = false;
}

void Curtains::open(int timeToWait) {
  if (operationGoing) {
    return;
  }
  operationGoing = true;

  Serial.println("Curtains: Opening");

  int isOpened = 0;
  if (digitalRead(this->pinOpened) != 1) {
    Serial.println("Curtains: isOpened != 1");
    if (!mtr->isGoing()) {
      Serial.println("Curtains: Set to backwards");
      mtr->backward();
    }
    delay(timeToWait);
  }

  mtr->stop();
  operationGoing = false;
}

void Curtains::close(int timeToWait) {
  if (operationGoing) {
    return;
  }
  operationGoing = true;

  Serial.println("Curtains: Closing");

  int isClosed = 0;
  if (digitalRead(this->pinClosed) != 1) {
    Serial.println("Curtains: isClosed != 1");
    if (!mtr->isGoing()) {
      Serial.println("Curtains: Set to forwards");
      mtr->forward();
    }
    delay(timeToWait);
  }

  mtr->stop();
  operationGoing = false;
}

bool Curtains::isClosed() {
  return digitalRead(this->pinClosed) == 1;
}

bool Curtains::isOpened() {
  return digitalRead(this->pinOpened) == 1;
}
