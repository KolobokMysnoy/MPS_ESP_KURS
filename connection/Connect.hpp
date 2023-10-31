#ifndef Connection_ESP
#define Connection_ESP

#include <stdint.h>
#include "c_types.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

typedef void (*onSentFunc)(uint8_t *, uint8_t);
typedef void (*onReceiveFunc)(uint8_t *, uint8_t *, uint8_t);
const int defaultTimesTry = 3;
const int maxSizeOfPeers = 6;

class Connector
{
    onSenfFunc sentFunc;
    onReceiveFunc receiveFunc;
    esp_now_role role;

    int freePeerId = 0;
    uint8_t peers[maxSizeOfPeers][6];

    // Init esp and set role that was set in constructor
    bool initESP(int timesToTry);

public:
    Connector(esp_now_role role);

    bool setup(int timesToTry = defaultTimesTry);

    // Add peer with [6]MAC and role
    int addPeer(uint8_t *peer, esp_now_role role = this->role, int timesToTry = defaultTimesTry);

    // Send data to idOfPeer that was registered
    void sendData(int idOfPeer, uint8_t *data, uint8_t size);
    // Add function to trigger when sent data
    void addFunctionOnSent(onSentFunc func);

    // Add function to trigger when receive data
    void addFunctionReceive(onReceiveFunc func);

    uint8_t *getMac();
};

#endif