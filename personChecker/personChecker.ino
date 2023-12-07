#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "sendStructs.hpp"
#include "Connect.hpp"

#include <cmath>

// Parameters
const int ReadPin = 5; // D1

// 15 minute without person to think that room is empty
// unsigned long timeWithoutPerson = 15 * 60 * 1000;
unsigned long timeWithoutPerson = 15 * 1000; // 15 sec
// TODO fix time to wait back to 15 minutes
// With this period sends message to hub
unsigned long timePeriodSend = 5 * 1000;

volatile unsigned long lastTime;
volatile unsigned long lastSend = 0;
#define RPZ_OUT
volatile int valueInteupt;

messagePerson personMsg;
int idOfHub;

uint8_t peer1[] = {0xB4, 0x8A, 0x0A, 0x89, 0x0F, 0x10};
Connector con = Connector(ESP_NOW_ROLE_SLAVE);

ICACHE_RAM_ATTR void movement_detection()
{
  #ifdef RPZ_OUT
    Serial.println("Interrupt begin");
  #endif
  valueInteupt = digitalRead(ReadPin);
  if (valueInteupt)
  {
    Serial.println("Person in room");

    lastTime = millis();
    personMsg.isPersonInside = true;
  }
  #ifdef RPZ_OUT
    Serial.println("Interrupt end");
  #endif
}

void setup(void)
{
  con.setup();
  con.getMac();
  idOfHub = con.addPeer(peer1);

  pinMode(ReadPin, INPUT);

  personMsg.isPersonInside = false;

  attachInterrupt(ReadPin, movement_detection, CHANGE);
}

template <typename T>
T absU(T num1, T num2)
{
  if (num1 > num2)
  {
    return num1 - num2;
  }
  return num2 - num1;
};

void loop(void)
{
  #ifdef RPZ_OUT
    Serial.println("Loop begin");
  #endif
  if (personMsg.isPersonInside && static_cast<long>(absU(lastTime, millis())) > timeWithoutPerson)
  {
    // if person always in room than pin won't trigger interrupt
    if (digitalRead(ReadPin))
    {
      lastTime = millis();
    }
    else
    {
      Serial.println("Person not in room");
      personMsg.isPersonInside = false;

      lastSend = millis();

      Serial.println("Send to hub");
      con.sendData(idOfHub, (uint8_t *)&personMsg, sizeof(personMsg));
    }
  }

  if (static_cast<long>(absU(lastSend, millis())) > timePeriodSend)
  {
    Serial.println("Send to hub");
    lastSend = millis();
    con.sendData(idOfHub, (uint8_t *)&personMsg, sizeof(personMsg));
  }

  delay(100);
  #ifdef RPZ_OUT
    Serial.println("Loop end");
  #endif
}
