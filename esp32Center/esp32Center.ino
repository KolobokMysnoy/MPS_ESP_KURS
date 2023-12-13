#include "Additional.hpp"

// block of connections
#include "Connect32.hpp"
#include "sendStructs.hpp"
#include "consoleOut.hpp"

// Котроль ленты
#include "ledCtrl.hpp"

/////////////////////////////////////////
// Setup for messages between ESP      //
/////////////////////////////////////////
#define INFO_DEBUG
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

// интервал для обновления яркости ленты
unsigned long lineUpdateTime = 2 * 1000;

/////////////////////////////////////////
// Setup for led control               //
/////////////////////////////////////////

LedCntrl controlLed(needLux, luxAccuracy, lineUpdateTime);
#define RPZ_OUT
// callback function that will be executed when data is received
// check mac of receive and calculate new data for motor and led
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  uint8_t macArr[sizeOfMac];
  memcpy(macArr, mac, sizeOfMac);

  outputMacLen(len, macArr);

  // person
  if (arraysEqual(macArr, personAdress))
  {
    // From person sensor data
    memcpy(&persMsg, incomingData, sizeof(persMsg));

#ifdef INFO_DEBUG
    outputPerson(persMsg.isPersonInside);
#endif

    return;
  }

  // lux
  if (arraysEqual(macArr, luxControlAdress))
  {
    // From light sensors data
    memcpy(&luxMsg, incomingData, sizeof(luxMsg));
#ifdef INFO_DEBUG
    outputLux(luxMsg.insideLux, luxMsg.outsideLux);
#endif
    calibrateBrightness();
    if (initialConfLux <= 2)
      return;

    stateRecalculationLed(persMsg.isPersonInside, luxMsg.insideLux);

#ifdef INFO_DEBUG
    Serial.print("Data send to led ==");
    Serial.println(ledMsg.procent);
    Serial.print("MAX: ");
    Serial.println(maxLux);
#endif

    stateRecalculationMotor(luxMsg.insideLux, luxMsg.outsideLux);
  }
#ifdef RPZ_OUT
  Serial.println("End receive");
#endif
}

// Output status of message
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
#ifdef OUT_INFO
  Serial.print("\r\nStatus of Last Message Sent:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
#endif
}

// Calculate led to set
// Change ledMsg structure and prevLedMsg structure
void stateRecalculationLed(const bool isPersonInside, const int luxInside)
{
#ifdef RPZ_OUT
  Serial.println("Recalc led beg");
#endif
  if (isPersonInside)
  {
    prevLedMsg.procent = ledMsg.procent;
    ledMsg.procent = controlLed.getLux(luxInside, millis());
  }
  else
  {
    // Person not in room. Off led
#ifdef INFO_DEBUG
    Serial.println("PERSON NOT IN ROOM: LED OFF");
#endif
    prevLedMsg.procent = ledMsg.procent;
    ledMsg.procent = 0;
  }
#ifdef RPZ_OUT
  Serial.println("Recalc led end");
#endif
}

// Calculate motor percent to set
// Change motorMsg structure and prevMotorMsg structure
void stateRecalculationMotor(const int luxInside, const int luxOutside)
{
#ifdef RPZ_OUT
  Serial.println("Recalc motor beg");
#endif
  int tmpMtr = getMotorPercent(luxOutside, luxInside);
  if (tmpMtr != motorMsg.procent)
  {
    // if motor not in place already
    prevMotorMsg.procent = motorMsg.procent;
    motorMsg.procent = tmpMtr;
  }
#ifdef RPZ_OUT
  Serial.println("Recalc motor end");
#endif
}

// First calibration of max led brightness
void calibrateBrightness()
{
  if (initialConfLux == 2)
  {
    // controlLed.setMax(luxMsg.insideLux);
    initialConfLux = 3;
#ifdef INFO_DEBUG
    Serial.println("--------------------------------------------------");
    Serial.print("max == ");
    Serial.println(maxLux);
    Serial.print("need == ");
    Serial.println(needLux);
#endif
  }
  initialConfLux++;
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
#ifdef RPZ_OUT
  Serial.println("Loop beg");
#endif

  sendCalibre();

  // send data in period time
  if (absU(lastTimeMsgSendToCurtains, millis()) > intervalTimeToSendCurtains)
  {
    lastTimeMsgSendToCurtains = millis();
    con.sendData(motorId, (uint8_t *)&motorMsg, sizeof(motorMsg));
    prevMotorMsg.procent = motorMsg.procent;
  }

  if (absU(lastTimeMsgToLedSend, millis()) > intervalTimeToSendLed)
  {
    lastTimeMsgToLedSend = millis();
    con.sendData(ledId, (uint8_t *)&ledMsg, sizeof(ledMsg));
  }

  // send data if it changes
  if (ledMsg.procent != prevLedMsg.procent && initialConfLux > 2)
  {
    lastTimeMsgToLedSend = millis();
    con.sendData(ledId, (uint8_t *)&ledMsg, sizeof(ledMsg));

    prevLedMsg.procent = ledMsg.procent;
  }

  if (motorMsg.procent != prevMotorMsg.procent)
  {
#ifdef INFO_DEBUG
    Serial.println("Prev motor != motor now");
    Serial.print(prevMotorMsg.procent);
    Serial.print("  |  ");
    Serial.print(motorMsg.procent);

#endif

    lastTimeMsgSendToCurtains = millis();
    con.sendData(motorId, (uint8_t *)&motorMsg, sizeof(motorMsg));

    prevMotorMsg.procent = motorMsg.procent;
  }

#ifdef RPZ_OUT
  Serial.println("Loop end");
#endif
}
