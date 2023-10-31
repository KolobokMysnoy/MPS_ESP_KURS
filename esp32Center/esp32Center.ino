#include <esp_now.h>
#include <WiFi.h>

#include "Additional.hpp"

// block of connections
#include "../connection/Connect.hpp"
#include "../sendStructs/sendStructs.hpp"

messageLux myMessage;
messageDriver motorMsg;
messageLed msgLed;
messagePerson messagePerson;

int motorId;
int ledId;

uint8_t motorAdress[] = {0x84, 0xf3, 0xeb, 0xbf, 0xc0, 0x27};
// TODO place real macs
uint8_t ledAdress[] = {x84, 0xf3, 0xeb, 0xbf, 0xc0, 0x27};
uint8_t personAdress[] = {0x84, 0xf3, 0xeb, 0xbf, 0xc0, 0x27};
uint8_t luxControlAdress[] = {0x84, 0xf3, 0xeb, 0xbf, 0xc0, 0x27};

Connector con(ESP_NOW_ROLE_CONTROLLER);

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  if (mac == ledAdress)
  {
  }
  memcpy(&myMessage, incomingData, sizeof(myMessage));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("inside: ");
  Serial.println(myMessage.insideLux);
  Serial.print("Outsude: ");
  Serial.println(myMessage.outsideLux);
  Serial.println();
}

void data_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nStatus of Last Message Sent:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void motorSetup()
{
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, motorAdress, 6);

  motorId = con.addPeer(peerInfo);
}

void ledSetup()
{
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, ledAdress, 6);

  ledId = con.addPeer(peerInfo);
}

void setupEsp()
{
  con.setup();
  con.addFunctionOnSent(data_sent);
  con.addFunctionReceive(OnDataRecv);

  motorSetup();
  Serial.println("ESP-NOW initialized successfully");
}

void setup()
{
  setupEsp();
}

void loop()
{
  motorMsg.percent = 0;
  con.sendData(motorId, (uint8_t *)&motorMsg, sizeof(motorMsg));
  delay(4000);

  motorMsg.percent = 100;
  con.sendData(motorId, (uint8_t *)&motorMsg, sizeof(motorMsg));
  delay(4000);
}
