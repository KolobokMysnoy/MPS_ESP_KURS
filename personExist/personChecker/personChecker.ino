#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "Connection.h"

// Parameters
const int ReadPin = 0;  // Analog pin A0
const int ActivatePin = 16; // D0

unsigned long timeWithoutPerson = 15 * 60 * 1000;

volatile unsigned long lastTime;
volatile int valueInteupt;
volatile bool isPersonInRoom;

uint8_t peer1[] = {0xB4, 0x8A, 0x0A, 0x89, 0x0F, 0x10};
Connector con = Connector(peer1, ESP_NOW_ROLE_SLAVE);

IRAM_ATTR void interact() {
  valueInteupt = digitalRead(ReadPin);
  
  if (valueInteupt == HIGH) {
    con.sendData(true);
    lastTime = millis();
    isPersonInRoom = true;
  }
}

int absoluteValue(int num) {
  if (num < 0) {
    return -num;
  } else {
    return num;
  }
}

void setup(void) {
  con.setup();

  pinMode(ReadPin, INPUT);
  pinMode(ActivatePin, OUTPUT);

  attachInterrupt(ReadPin, interact, CHANGE);
}

void loop(void) {
  if (!isPersonInRoom && absoluteValue(lastTime - millis()) > timeWithoutPerson) {
    con.sendData(false);
    isPersonInRoom = false;
  }
  
  delay(100);
}
