#ifndef LED_CNRL
#define LED_CNRL
#include <Arduino.h>

class LedCnrtl {
  int rangeOutput;
  int currentBrightness;
  int pinOut;

public:
  LedCnrtl(int range, int pinOut);

  void setBrightness(int brightness);
};

#endif