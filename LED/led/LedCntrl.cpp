#include "LedCntrl.h"


LedCnrtl::LedCnrtl(int range, int pinOut) {
  this->currentBrightness = 0;
  this->rangeOutput = range;
  this->pinOut = pinOut;
}

void LedCnrtl::setBrightness(int brightness) {
  if (this->currentBrightness == brightness) {
    return;
  }

  this->currentBrightness = brightness;
  int newVal = (this->rangeOutput * brightness) / 100;
  analogWrite(this->pinOut, newVal);
}