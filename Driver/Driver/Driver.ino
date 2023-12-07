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

#define INFO_DEBUG

const int MaxValue = 512;

// Motor control
const int PinIn1 = 4; // D1
const int PinIn2 = 5; // D2

// Check for close or open
const int PinClosed = 14; // D5
const int PinOpened = 12; // D6

int timeToWait = 1000;
#define RPZ_OUT
// Init motor and curtains to control motor
MOTOR motor = MOTOR(PinIn1, PinIn2);
Curtains curt = Curtains(&motor, PinClosed, PinOpened);

// ESP-NOW
Connector con(ESP_NOW_ROLE_SLAVE);
uint8_t peer1[] = {0xB4, 0x8A, 0x0A, 0x89, 0x0F, 0x10};
messageDriver myMessage;

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
#ifdef RPZ_OUT
  Serial.println("Receive begin");
  #endif
  memcpy(&myMessage, incomingData, sizeof(myMessage));

#ifdef INFO_DEBUG
  Serial.print("Get message with driver percent");
  Serial.println(myMessage.procent);
#endif

  switch (myMessage.procent)
  {
  case 0:

#ifdef INFO_DEBUG
    Serial.println("Need Closing");
#endif

    curt.setNeedOpening();
    break;
  case 100:

#ifdef INFO_DEBUG
    Serial.println("Need opening");
#endif

    curt.setNeedClosing();
    break;
  }
  #ifdef RPZ_OUT
  Serial.println("receive end");
  #endif
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
  #ifdef RPZ_OUT
  Serial.println("Loop begin");
  #endif
  if (curt.getNeedClosing())
  {
    curt.close(timeToWait);
  }

  if (curt.getNeedOpening())
  {
    curt.open(timeToWait);
  }

  if (!(curt.getNeedOpening() || curt.getNeedClosing()))
  {
    curt.stop();
  }
  #ifdef RPZ_OUT
  Serial.println("Loop end");
  #endif
}
