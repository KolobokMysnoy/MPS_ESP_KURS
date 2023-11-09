#ifndef Connection_ESP
#define Connection_ESP

#include <stdint.h>
#include <Arduino.h>

// ESP32 libs for wifi
#include <esp_now.h>
#include <WiFi.h>

typedef void (*onSentFunc)(const uint8_t *, esp_now_send_status_t);
typedef void (*onReceiveFunc)(const uint8_t *, const uint8_t *, int);
const int defaultTimesTry = 3;
const int maxSizeOfPeers = 6;

class Connector
{
    onSentFunc sentFunc;
    onReceiveFunc receiveFunc;

    int freePeerId = 0;
    uint8_t peers[maxSizeOfPeers][6];

    // Init esp and set role that was set in constructor
    bool initESP(int timesToTry);

public:
    Connector();

    bool setup(int timesToTry = defaultTimesTry);

    // Add peer with [6]MAC and role
    // int addPeer(uint8_t *peer, bool isValid = true, esp_now_role role = ESP_NOW_ROLE_SLAVE, int timesToTry = defaultTimesTry);
    // INFO Was removed because ESP8266 don't support peerInfo
    // // Add peer with peerInfo
    int addPeer(uint8_t *peerMac, int timesToTry = defaultTimesTry);

    // Send data to idOfPeer that was registered
    void sendData(int idOfPeer, uint8_t *data, uint8_t size);
    // Add function to trigger when sent data
    void addFunctionOnSent(onSentFunc func);

    // Add function to trigger when receive data
    void addFunctionReceive(onReceiveFunc func);

    void getMac();
};

#endif