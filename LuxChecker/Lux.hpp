#ifndef LUX
#define LUX
#include <Arduino.h>


class Luxementr {
  int outsidePin;
  int insidePin;
  int readPin;

  float VIN = 3.3;
  int R = 10000;

  int convertToLux(int res);

  int readLuxFromPin(int pin);

public:
  Luxementr(int insidePin, int outsidePin, int readPin);

  int getOutsideLux();
  int getInsideLux();
};

#endif