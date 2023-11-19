#include "Additional.hpp"

// block of connections
#include "Connect32.hpp"
#include "sendStructs.hpp"
#include "consoleOut.hpp"

/////////////////////////////////////////
// Setup for messages between ESP      //
/////////////////////////////////////////

// get structs
messageLux luxMsg;
messagePerson persMsg;

// send structs
messageDriver motorMsg;
messageLed ledMsg;

int motorId;
int ledId;

// previous values
messageDriver prevMotorMsg;
messageLed prevLedMsg;

/////////////////////////////////////////
// Setup for connection to others ESP  //
/////////////////////////////////////////

// Macs of other ESP
const uint8_t sizeOfMac = 6;
uint8_t motorAdress[] = {0x84, 0xf3, 0xeb, 0xbf, 0xc0, 0x27};
uint8_t luxControlAdress[] = {0x84, 0xf3, 0xeb, 0xb6, 0xbd, 0xf4};
uint8_t personAdress[] = {0x84, 0xf3, 0xeb, 0x01, 0x7f, 0xf9};
uint8_t ledAdress[] = {0xec, 0xfa, 0xbc, 0xc9, 0x6a, 0x11};

Connector con;

/////////////////////////////////////////
// Setup for lux                       //
/////////////////////////////////////////

int needLux = 64;
int maxLux = needLux;
uint8_t initialConfLux = 0;

/////////////////////////////////////////
// Setup for intervals                 //
/////////////////////////////////////////

// times to wait before send values
unsigned long intervalTimeToSendLed = 5 * 1000;       // 5s
unsigned long intervalTimeToSendCurtains = 10 * 1000; // 10s

// last times send
unsigned long lastTimeMsgToLedSend = 0;
unsigned long lastTimeMsgSendToCurtains = 0;

// callback function that will be executed when data is received
// check mac of receive and calculate new data for motor and led
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  uint8_t macArr[sizeOfMac];
  memcpy(macArr, mac, sizeOfMac);

  outputMacLen(len, macArr);

  if (arraysEqual(macArr, personAdress))
  {
    // From person sensor data
    memcpy(&persMsg, incomingData, sizeof(persMsg));

#ifdef INFO_DEBUG
    outputPerson(persMsg.isPersonInside);
#endif

    return;
  }
  if (arraysEqual(macArr, luxControlAdress))
  {
    // From light sensors data
    memcpy(&luxMsg, incomingData, sizeof(luxMsg));
#ifdef INFO_DEBUG
    outputLux(luxMsg.insideLux, luxMsg.outsideLux);
#endif
    calibrateBrightness();

    stateRecalculationLed(persMsg.isPersonInside, luxMsg.insideLux);

#ifdef INFO_DEBUG
    Serial.print("Data send to led ==");
    Serial.println(ledMsg.procent);
#endif

    stateRecalculationMotor(luxMsg.insideLux, luxMsg.outsideLux);
  }
}

// Output status of message
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nStatus of Last Message Sent:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Calculate led to set
// Change ledMsg structure and prevLedMsg structure
void stateRecalculationLed(const bool isPersonInside, const int luxInside)
{
  if (isPersonInside)
  {

    int tmpPerc = getLedPercent(luxInside, needLux, ledMsg.procent, maxLux);
    if (tmpPerc != -1)
    {
      prevLedMsg.procent = ledMsg.procent;
      ledMsg.procent = tmpPerc;
    }
  }
  else
  {
    // Person not in room. Off led
    prevLedMsg.procent = ledMsg.procent;
    ledMsg.procent = 0;
  }
}

// Calculate motor percent to set
// Change motorMsg structure and prevMotorMsg structure
void stateRecalculationMotor(const int luxInside, const int luxOutside)
{
  int tmpMtr = getMotorPercent(luxOutside, luxInside);
  if (tmpMtr != motorMsg.procent)
  {
    // if motor not in place already
    prevMotorMsg.procent = motorMsg.procent;
    motorMsg.procent = tmpMtr;
  }
}

// First calibration of max led brightness
void calibrateBrightness()
{
  if (initialConfLux == 1)
  {
    maxLux = luxMsg.insideLux;
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
    ledMsg.procent = 100;
    con.sendData(ledId, (uint8_t *)&ledMsg, sizeof(ledMsg));
    initialConfLux = 1;
  }
}

void setup()
{
  con.setup();
  con.getMac();

#ifdef INFO_DEBUG
  con.addFunctionOnSent(onDataSent);
#endif

  con.addFunctionReceive(OnDataRecv);

  motorId = con.addPeer(motorAdress);
  ledId = con.addPeer(ledAdress);

  Serial.println("ESP-NOW initialized successfully");
}

void loop()
{
  sendCalibre();

  // send data in period time
  if (abs(lastTimeMsgSendToCurtains - millis()) > intervalTimeToSendCurtains)
  {
    lastTimeMsgSendToCurtains = millis();
    con.sendData(motorId, (uint8_t *)&motorMsg, sizeof(motorMsg));
  }

  if (abs(lastTimeMsgToLedSend - millis()) > intervalTimeToSendLed)
  {
    lastTimeMsgToLedSend = millis();
    con.sendData(ledId, (uint8_t *)&ledMsg, sizeof(ledMsg));
  }

  // send data if it changes
  if (ledMsg.procent != prevLedMsg.procent)
  {
    lastTimeMsgToLedSend = millis();
    con.sendData(ledId, (uint8_t *)&ledMsg, sizeof(ledMsg));

    prevLedMsg.procent = ledMsg.procent;
  }

  if (motorMsg.procent != prevMotorMsg.procent)
  {
    lastTimeMsgSendToCurtains = millis();
    con.sendData(motorId, (uint8_t *)&motorMsg, sizeof(motorMsg));

    prevMotorMsg.procent = ledMsg.procent;
  }
}
