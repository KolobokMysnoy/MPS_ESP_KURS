#ifndef MOTOR_CNTRL
#define MOTOR_CNTRL
#include <Arduino.h>

class MOTOR {
  int pinIn1, pinIn2;
  bool isForward, isBackward;

public:
  MOTOR(int pinIn1, int pinIn2);
  bool isGoing();

  void forward();
  void backward();

  void stop();
};

#endif