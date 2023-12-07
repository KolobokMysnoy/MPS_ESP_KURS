#include "Curtains.hpp"

template <typename T>
T absU(T num1, T num2)
{
  if (num1 > num2)
  {
    return num1 - num2;
  }
  return num2 - num1;
};

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
  Serial.println(static_cast<long>(absU(this->lastChecked, millis())) > timeToWait);
  // check if from last call time pass
  if (static_cast<long>(absU(this->lastChecked, millis())) <= timeToWait)
  {

#ifdef INFO_DEBUG
    Serial.println("Curtains.isCanBeOperated: timeToWait is more than passed");
#endif

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

#ifdef INFO_DEBUG
    Serial.println("Curtains.open: curains is closed");
    Serial.println("Curtains.open: Set to backwards");
#endif

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

  Serial.print("Pin close if");
  Serial.println(!this->isClosed());
  if (!this->isClosed())
  {

#ifdef INFO_DEBUG
    Serial.println("Curtains.close: curains is opened");
    Serial.println("Curtains.close: Set to forwards");
#endif

    mtr->forward();
  }
}

bool Curtains::isClosed()
{
  return digitalRead(this->pinClosed) == 0;
}

bool Curtains::isOpened()
{
  return digitalRead(this->pinOpened) == 0;
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
