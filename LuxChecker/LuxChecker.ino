/* LDR Luximeter */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "Lux.h"
#include "../connection/Connect.hpp"
#include "../sendStructs/sendStructs.hpp"

// Parameters
const int Analog_Pin = 0;  // Analog pin A0
const int OutsidePin = 16; // D0
const int InsidePin = 5;   // D1

Luxementr lux = Luxementr(InsidePin, OutsidePin, Analog_Pin);

Connector con(ESP_NOW_ROLE_CONTROLLER);

uint8_t peer1[] = {0xB4, 0x8A, 0x0A, 0x89, 0x0F, 0x10};
messageLux myMessage;

int idOfPeer;

void onSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.println("Status:");
  Serial.println(sendStatus);
}

void setup(void)
{
  con.setup();

  idOfPeer = con.addPeer(peer1, ESP_NOW_ROLE_SLAVE);
  con.addFunctionOnSent(onSent);

  pinMode(OutsidePin, OUTPUT);
  pinMode(InsidePin, OUTPUT);
}

void sendLux(int insideLux, int outsideLux)
{
  myMessage.insideLux = insideLux;
  myMessage.outsideLux = outsideLux;

  con.sendData(idOfPeer, (uint8_t *)&myMessage, sizeof(myMessage));
}

void loop(void)
{
  sendLux(lux.getInsideLux(), lux.getOutsideLux());

  delay(1400);
}
