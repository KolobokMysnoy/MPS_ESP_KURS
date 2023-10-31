/* LDR Driver */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// dependenties
#include "motor.hpp"
#include "Curtains.hpp"

// block of connections
#include "../connection/Connect.hpp"
#include "../sendStructs/sendStructs.hpp"

const int MaxValue = 512;

const int PinIn1 = 5; // D1
const int PinIn2 = 4; // D2

const int PinClosed = 14; // D5
const int PinOpened = 12; // D6

int timeToWait = 600;

MOTOR motor = MOTOR(PinIn1, PinIn2);
Curtains curt = Curtains(&motor, PinClosed, PinOpened);

Connector con(ESP_NOW_ROLE_SLAVE);
uint8_t peer1[] = {0xB4, 0x8A, 0x0A, 0x89, 0x0F, 0x10};

messageDriver myMessage;

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  memcpy(&myMessage, incomingData, sizeof(myMessage));

  switch (myMessage.procent)
  {
  case 0:
    curt.setNeedOpening();
    break;
  case 100:
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
  con.setup();
  con.addFunctionReceive(OnDataRecv);

  pinSetup();
}

void loop(void)
{
  if (curt.isNeedClosing)
  {
    if (!curt.isClosed())
    {
      curt.close(timeToWait);
    }
  }
  if (curt.isNeedOpening)
  {
    if (!curt.isOpened())
    {
      curt.open(timeToWait);
    }
  }
  delay(timeToWait);
}
