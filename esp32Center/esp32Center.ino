#include "Additional.hpp"

// block of connections
#include "Connect32.hpp"
#include "sendStructs.hpp"
#include "consoleOut.hpp"

messageLux myMessage;
messageDriver motorMsg;
messageLed msgLed;
messagePerson msgPers;

int motorId;
int ledId;

// Macs of other ESP
const uint8_t sizeOfMac = 6;
uint8_t motorAdress[] = {0x84, 0xf3, 0xeb, 0xbf, 0xc0, 0x27};
uint8_t luxControlAdress[] = {0x84, 0xf3, 0xeb, 0xb6, 0xbd, 0xf4};
uint8_t personAdress[] = {0x84, 0xf3, 0xeb, 0x01, 0x7f, 0xf9};
uint8_t ledAdress[] = {0xec, 0xfa, 0xbc, 0xc9, 0x6a, 0x11};

Connector con;

int needLux = 64;
int maxLux = needLux;
uint8_t initialConfLux = 0;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  uint8_t macArr[sizeOfMac];
  memcpy(macArr, mac, sizeOfMac);

  outputMacLen(len, macArr);

  if (arraysEqual(macArr, personAdress))
  {
    // From person sensor data
    memcpy(&msgPers, incomingData, sizeof(msgPers));

#ifdef INFO_DEBUG
    outputPerson(msgPers.isPersonInside);
#endif

    return;
  }
  if (arraysEqual(macArr, luxControlAdress))
  {
    // From light sensors data
    memcpy(&myMessage, incomingData, sizeof(myMessage));
#ifdef INFO_DEBUG
    outputLux(myMessage.insideLux, myMessage.outsideLux);
#endif
    if (initialConfLux == 1)
    {
      maxLux = myMessage.insideLux;
      if (maxLux < needLux)
      {
        needLux = maxLux;
      }
      initialConfLux = 2;
#ifdef INFO_DEBUG
      Serial.println("--------------------------------------------------");
      Serial.print("max == ");
      Serial.println(maxLux);
      Serial.print("need == ");
      Serial.println(needLux);
#endif
    }

    if (msgPers.isPersonInside)
    {

      int tmpPerc = getLedPercent(myMessage.insideLux, needLux, msgLed.procent, maxLux);
      if (tmpPerc != -1)
      {
        msgLed.procent = tmpPerc;
      }
    }
    else
    {
      // Person not in room. Off led
      msgLed.procent = 0;
    }

    con.sendData(ledId, (uint8_t *)&msgLed, sizeof(msgLed));

#ifdef INFO_DEBUG
    Serial.print("Data send to led ==");
    Serial.println(msgLed.procent);
#endif

    int tmpMtr = getMotorPercent(myMessage.outsideLux, myMessage.insideLux);
    if (tmpMtr != motorMsg.procent)
    {
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

void setup()
{
  con.setup();
  con.getMac();

#ifdef INFO_DEBUG
  con.addFunctionOnSent(data_sent);
#endif

  con.addFunctionReceive(OnDataRecv);

  ledSetup();
  motorSetup();

  Serial.println("ESP-NOW initialized successfully");
}

void loop()
{
  // Send 100 to config
  if (initialConfLux == 0)
  {
#ifdef INFO_DEBUG
    Serial.println("First intial conflux");
#endif
    msgLed.procent = 100;
    con.sendData(ledId, (uint8_t *)&msgLed, sizeof(msgLed));
    initialConfLux = 1;
  }
}
