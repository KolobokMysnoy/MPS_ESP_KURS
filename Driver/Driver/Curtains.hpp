#ifndef CURTAINS_CNTRL
#define CURTAINS_CNTRL

#include <Arduino.h>
#include "motor.hpp"

class Curtains
{
  MOTOR *mtr;
  int pinClosed;
  int pinOpened;

  bool operationGoing;

public:
  Curtains(MOTOR *mtr, int pinClosed, int pinOpened);
  bool isNeedClosing;
  bool isNeedOpening;

  void setNeedClosing();
  void setNeedOpening();

  void open(int timeToWait);
  void close(int timeToWait);

  bool isClosed();
  bool isOpened();
};

#endif
