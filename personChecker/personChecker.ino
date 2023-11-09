#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "sendStructs.hpp"
#include "Connect.hpp"

// Parameters
const int ReadPin = 5; // D1

// 15 minute without person to think that room is empty
unsigned long timeWithoutPerson = 15 * 60 * 1000;
// With this period sends message to hub
unsigned long timePeriodSend = 30 * 1000;

volatile unsigned long lastTime;
volatile unsigned long lastSend = 0;

volatile int valueInteupt;

messagePerson personMsg;
int idOfHub;

uint8_t peer1[] = {0xB4, 0x8A, 0x0A, 0x89, 0x0F, 0x10};
Connector con = Connector(ESP_NOW_ROLE_SLAVE);

ICACHE_RAM_ATTR void movement_detection()
{
  valueInteupt = digitalRead(ReadPin);
  if (valueInteupt)
  {
    Serial.println("Person in room");

    lastTime = millis();
    personMsg.isPersonInside = true;
  }
}

int absoluteValue(int num)
{
  if (num < 0)
  {
    return -num;
  }
  else
  {
    return num;
  }
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

void loop(void)
{
  if (personMsg.isPersonInside && absoluteValue(lastTime - millis()) > timeWithoutPerson)
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

  if (absoluteValue(lastSend - millis()) > timePeriodSend)
  {
    Serial.println("Send to hub");
    lastSend = millis();
    con.sendData(idOfHub, (uint8_t *)&personMsg, sizeof(personMsg));
  }

  delay(100);
}
