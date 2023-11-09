#ifndef MOTOR_CNTRL
#define MOTOR_CNTRL
#include <Arduino.h>

class MOTOR
{
  // Pins to control motor through driver
  int pinIn1, pinIn2;
  // State of motor
  bool isGoingForward, isGoingBackward;

public:
  // Set pins to control motor
  MOTOR(int pinIn1, int pinIn2);
  // Check if motor is doing something now
  bool isGoing();

  // Set motor to go forward
  void forward();
  // Set motor to go bacward
  void backward();

  // Stop motor
  void stop();
};

#endif