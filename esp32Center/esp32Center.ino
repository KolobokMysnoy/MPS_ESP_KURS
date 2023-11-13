#include "Additional.hpp"

// block of connections
#include "Connect32.hpp"
#include "sendStructs.hpp"
#include "consoleOut.hpp"

// get structs
messageLux myMessage;
messagePerson msgPers;

// send structs
messageDriver motorMsg;
messageLed msgLed;

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

// times to wait before send values
unsigned long timeToSendLed = 5 * 1000;       // 5s
unsigned long timeToSendCurtains = 10 * 1000; // 10s

// last times send
unsigned long lastTimeToSendLed = 0;
unsigned long lastTimeToSendCurtains = 0;

// previous values
messageDriver prevMotorMsg;
messageLed prevLedMsg;

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
    calibreBrigh();

    if (msgPers.isPersonInside)
    {

      int tmpPerc = getLedPercent(myMessage.insideLux, needLux, msgLed.procent, maxLux);
      if (tmpPerc != -1)
      {
        prevLedMsg.procent = msgLed.procent;
        msgLed.procent = tmpPerc;
      }
    }
    else
    {
      // Person not in room. Off led
      prevLedMsg.procent = msgLed.procent;
      msgLed.procent = 0;
    }

#ifdef INFO_DEBUG
    Serial.print("Data send to led ==");
    Serial.println(msgLed.procent);
#endif

    int tmpMtr = getMotorPercent(myMessage.outsideLux, myMessage.insideLux);
    if (tmpMtr != motorMsg.procent)
    {
      // if motor not in place already
      prevMotorMsg.procent = motorMsg.procent;
      motorMsg.procent = tmpMtr;
    }

    return;
  }
}

void data_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nStatus of Last Message Sent:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// First calibration of max led brightness
void calibreBrigh()
{
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
}

// Send 100 brigtness to led to know it brigtness
void sendCalibre()
{
  // Send 100 to config
  if (initialConfLux == 0)
  {
#ifdef INFO_DEBUG
    Serial.println("First intial conflux");
#endif
    prevLedMsg.procent = 0;
    msgLed.procent = 100;
    con.sendData(ledId, (uint8_t *)&msgLed, sizeof(msgLed));
    initialConfLux = 1;
  }
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
  sendCalibre();

  // send data in period time
  if (abs(lastTimeToSendCurtains - millis()) > timeToSendCurtains)
  {
    lastTimeToSendCurtains = millis();
    con.sendData(motorId, (uint8_t *)&motorMsg, sizeof(motorMsg));
  }

  if (abs(lastTimeToSendLed - millis()) > timeToSendLed)
  {
    lastTimeToSendLed = millis();
    con.sendData(ledId, (uint8_t *)&msgLed, sizeof(msgLed));
  }

  // send data if it changes
  if (msgLed.procent != prevLedMsg.procent)
  {
    lastTimeToSendLed = millis();
    con.sendData(ledId, (uint8_t *)&msgLed, sizeof(msgLed));

    prevLedMsg.procent = msgLed.procent;
  }

  if (motorMsg.procent != prevMotorMsg.procent)
  {
    lastTimeToSendCurtains = millis();
    con.sendData(motorId, (uint8_t *)&motorMsg, sizeof(motorMsg));

    prevMotorMsg.procent = msgLed.procent;
  }
}
