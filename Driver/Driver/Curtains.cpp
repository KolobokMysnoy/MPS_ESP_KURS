#include "Curtains.hpp"

Curtains::Curtains(MOTOR *mtr, int pinClosed, int pinOpened)
{
  this->mtr = mtr;
  this->pinClosed = pinClosed;
  this->pinOpened = pinOpened;

  // default values
  this->operationGoing = false;
  this->isNeedClosing = false;
  this->isNeedOpening = false;
  this->lastChecked = 0;
}

bool Curtains::getNeedClosing()
{
  return this->isNeedClosing && !this->isClosed();
}

bool Curtains::getNeedOpening()
{
  return this->isNeedOpening && !this->isOpened();
}

void Curtains::setNeedClosing()
{
  this->isNeedClosing = true;
  this->isNeedOpening = false;
}

void Curtains::setNeedOpening()
{
  this->isNeedOpening = true;
  this->isNeedClosing = false;
}

bool Curtains::isCanBeOperated(int timeToWait)
{
  Serial.println("Curtains.isCanBeOperated:");
  Serial.println(static_cast<long>((this->lastChecked - millis())));
  // check if from last call time pass
  if (std::abs(static_cast<long>((this->lastChecked - millis()))) < timeToWait)
  {
    Serial.println("Curtains.isCanBeOperated: timeToWait is more than passed");
    return false;
  }

  // if operation was started before
  if (this->operationGoing)
  {
    this->operationGoing = false;
    mtr->stop();

    return false;
  }

  return true;
}

void Curtains::open(int timeToWait)
{
  if (!this->isCanBeOperated(timeToWait))
  {
    return;
  }
  this->isStopped = false;
  
  Serial.println("Curtains.open: Opening");

  this->lastChecked = millis();
  operationGoing = true;

  if (!this->isOpened())
  {
    Serial.println("Curtains.open: curains is closed");
    Serial.println("Curtains.open: Set to backwards");

    mtr->backward();
  }
}

void Curtains::close(int timeToWait)
{
  if (!this->isCanBeOperated(timeToWait))
  {
    return;
  }
  this->isStopped = false;

  Serial.println("Curtains: Closing");

  this->lastChecked = millis();
  operationGoing = true;

  if (!this->isClosed())
  {
    Serial.println("Curtains.close: curains is opened");
    Serial.println("Curtains.close: Set to forwards");

    mtr->forward();
  }
}

bool Curtains::isClosed()
{
  return digitalRead(this->pinClosed) == 1;
}

bool Curtains::isOpened()
{
  return digitalRead(this->pinOpened) == 1;
}

void Curtains::stop()
{
  // if stopped already leave
  if (this->isStopped) 
  {
    return;
  }

  Serial.println("Curtains.stop");
  // stop all operation that going now
  this->isNeedClosing = false;
  this->isNeedOpening = false;
  this->operationGoing = false;

  // stop motor
  this->mtr->stop();
  this->isStopped = true;
}
