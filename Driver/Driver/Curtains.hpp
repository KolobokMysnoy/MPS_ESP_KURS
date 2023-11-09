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
  bool isNeedClosing;
  bool isNeedOpening;

  // Check if curtains is opened
  bool isClosed();
  // Check if curtains is closed
  bool isOpened();

  // Set that need closing
  void setNeedClosing();
  // Set that need opening
  void setNeedOpening();

public:
  // Set motor to what control and pins to check
  Curtains(MOTOR *mtr, int pinClosed, int pinOpened);

  // Check if need closing
  bool getNeedClosing();
  // Check if need opening
  bool getNeedOpening();

  // Open curtains
  void open(int timeToWait);
  // Close curtains
  void close(int timeToWait);
};

#endif
