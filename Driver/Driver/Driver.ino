/* LDR Driver */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// dependenties
#include "motor.hpp"
#include "Curtains.hpp"

// block of connections
#include "Connect.hpp"
#include "sendStructs.hpp"

const int MaxValue = 512;

// Motor control
const int PinIn1 = 4; // D1
const int PinIn2 = 5; // D2

// Check for close or open
const int PinClosed = 14; // D5
const int PinOpened = 12; // D6

int timeToWait = 500;

// Init motor and curtains to control motor
MOTOR motor = MOTOR(PinIn1, PinIn2);
Curtains curt = Curtains(&motor, PinClosed, PinOpened);

// ESP-NOW
Connector con(ESP_NOW_ROLE_SLAVE);
uint8_t peer1[] = {0xB4, 0x8A, 0x0A, 0x89, 0x0F, 0x10};
messageDriver myMessage;

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  memcpy(&myMessage, incomingData, sizeof(myMessage));

  Serial.print("Get message with driver percent");
  Serial.println(myMessage.procent);

  switch (myMessage.procent)
  {
  case 0:
    Serial.println("Need Closing");
    curt.setNeedOpening();
    break;
  case 100:
    Serial.println("Need opening");
    curt.setNeedClosing();
    break;
  }
}

void pinSetup()
{
  pinMode(PinClosed, INPUT);
  pinMode(PinOpened, INPUT);

  pinMode(PinIn1, OUTPUT);
  pinMode(PinIn2, OUTPUT);
}

void setup(void)
{
  pinSetup();

  con.setup();
  con.addFunctionReceive(OnDataRecv);
  con.getMac();
}

void loop(void)
{
  if (curt.getNeedClosing())
  {
    curt.close(timeToWait);
  }
  

  if (curt.getNeedOpening())
  {
    curt.open(timeToWait);
  }

  if (!(curt.getNeedOpening() || curt.getNeedClosing())) {
    curt.stop();
  }
}
