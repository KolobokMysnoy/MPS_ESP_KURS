#include "Additional.hpp"

// block of connections
#include "Connect32.hpp"
#include "sendStructs.hpp"

messageLux myMessage;
messageDriver motorMsg;
messageLed msgLed;
messagePerson msgPers;

int motorId;
int ledId;

const uint8_t sizeOfMac = 6;
uint8_t motorAdress[] = {0x84, 0xf3, 0xeb, 0xbf, 0xc0, 0x27};
uint8_t luxControlAdress[] = {0x84, 0xf3, 0xeb, 0xb6, 0xbd, 0xf4};
uint8_t personAdress[] = {0x84, 0xf3, 0xeb, 0x01, 0x7f, 0xf9};

// TODO place real macs
uint8_t ledAdress[] = {0xec, 0xfa, 0xbc, 0xc9, 0x6a, 0x11};

Connector con;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  uint8_t macArr[sizeOfMac];
  memcpy(macArr, mac, sizeOfMac);

  Serial.print("Receive some data from mac");
  printArray(macArr);

  if (arraysEqual(macArr, personAdress))
  {
    // From person sensor data
    memcpy(&msgPers, incomingData, sizeof(msgPers));
    
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Person ");
    if (msgPers.isPersonInside == 1) {
      Serial.println("inside room");
    } else {
      Serial.println("outside room");
    }
    Serial.println();

    return;
  }
  if (arraysEqual(macArr, luxControlAdress))
  {
    // From light sensors data
    memcpy(&myMessage, incomingData, sizeof(myMessage));
    
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("inside: ");
    Serial.println(myMessage.insideLux);
    Serial.print("Outsude: ");
    Serial.println(myMessage.outsideLux);
    Serial.println();
    
    if (msgPers.isPersonInside) {
      // If person inside then regulate lux else do nothing
      msgLed.procent = getLedPercent(myMessage.insideLux);
    } else {
      // Person not in room
      // off led
      msgLed.procent = 0;
    }
    con.sendData(ledId, (uint8_t *)&msgLed, sizeof(msgLed));

    int tmpMtr = getMotorPercent(myMessage.outsideLux, myMessage.insideLux);
    if (tmpMtr != motorMsg.procent) {
      // if motor not in place already 
      motorMsg.procent = tmpMtr;
      con.sendData(motorId, (uint8_t *)&motorMsg, sizeof(motorMsg));
    }

    return;
  }
}

void data_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nStatus of Last Message Sent:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void motorSetup()
{
  motorId = con.addPeer(motorAdress);
}

void ledSetup()
{
  ledId = con.addPeer(ledAdress);
}

void setupEsp()
{
  con.setup();
  con.getMac();

  con.addFunctionOnSent(data_sent);
  con.addFunctionReceive(OnDataRecv);

  ledSetup();
  motorSetup();
  Serial.println("ESP-NOW initialized successfully");
}

void setup()
{
  setupEsp();
}

void loop()
{
  motorMsg.procent = 0;
  con.sendData(motorId, (uint8_t *)&motorMsg, sizeof(motorMsg));
  msgLed.procent = 0;
  con.sendData(ledId, (uint8_t *)&msgLed, sizeof(msgLed));
  delay(4000);

  motorMsg.procent = 100;
  con.sendData(motorId, (uint8_t *)&motorMsg, sizeof(motorMsg));
  msgLed.procent = 100;
  con.sendData(ledId, (uint8_t *)&msgLed, sizeof(msgLed));
  delay(4000);
}
