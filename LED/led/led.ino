#include <ESP8266WiFi.h>
#include <espnow.h>
#include "LedCntrl.hpp"
#include "Connect.hpp"
#include "sendStructs.hpp"

const int PinOut = 4;     // D2
const int32 Range = 1024; // For ESP8266, the analogWriteRange is 0-1023
const int32 Freq = 1000;  // Set a lower frequency, e.g., 1 kHz

LedCnrtl led = LedCnrtl(Range, PinOut);

uint8_t peer1[] = {0xB4, 0x8A, 0x0A, 0x89, 0x0F, 0x10};

Connector con(ESP_NOW_ROLE_SLAVE);
#define RPZ_OUT
messageLed myMessage;
int previousValue = -1; // Initialize to a value that's out of the valid range

void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t data_len)
{
  #ifdef RPZ_OUT
  Serial.println("Receive begin");
  #endif
  if (data_len == sizeof(myMessage))
  {
    memcpy(&myMessage, data, sizeof(myMessage));

    Serial.print("Get message with procent:");
    Serial.println(myMessage.procent);
  }
  #ifdef RPZ_OUT
  Serial.println("Receive end");
  #endif
}

void setup()
{
  con.setup();
  con.addFunctionReceive(OnDataRecv);
  con.getMac();
  
  pinMode(PinOut, OUTPUT);
  analogWriteRange(Range);
  analogWriteFreq(Freq);
}

void loop()
{
  #ifdef RPZ_OUT
  Serial.println("Loop begin");
  #endif
  led.setBrightness(myMessage.procent);
  #ifdef RPZ_OUT
  Serial.println("Loop end");
  #endif
}
