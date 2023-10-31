#ifndef Connection_ESP
#define Connection_ESP

#include <stdint.h>
#include "c_types.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

class Connector {
  uint8_t peer[6];
  esp_now_role role;

  static void onSent(uint8_t *mac_addr, uint8_t sendStatus);

  bool initESP(int timesToTry);
  bool addPeer(int timesToTry);
public:
  Connector(uint8_t peer[6], esp_now_role role);

  bool setup(int timesToTry = 2);

  void sendData(bool inRoom);
};

#endif