#include "Lux.h"

int Luxementr::convertToLux(int res) {
  float Vout = float(res) * (float(this->VIN) / float(1023));  // Conversion analog to voltage
  float RLDR = (this->R * (this->VIN - Vout)) / Vout;                    // Conversion voltage to resistance
  int lux = 500 / (RLDR / 1000);                             // Conversion resitance to lumen
  return lux;
}

Luxementr::Luxementr(int insidePin, int outsidePin, int readPin) {
  this->insidePin = insidePin;
  this->outsidePin = outsidePin;
  this->readPin = readPin;
}

int Luxementr::readLuxFromPin(int pin) {
  digitalWrite(pin, HIGH);
  int lux = convertToLux(analogRead(this->readPin));
  digitalWrite(pin, LOW);

  return lux;
}

int Luxementr::getOutsideLux() {
  int lux = readLuxFromPin(this->outsidePin);
  Serial.print("Luxemetr: Outside lux = ");
  Serial.println(lux);

  return lux;
}

int Luxementr::getInsideLux() {
  int lux = readLuxFromPin(this->insidePin);
  Serial.print("Luxemetr: Inside lux = ");
  Serial.println(lux);

  return lux;
}