/* LDR Luximeter */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "Lux.h"

// Parameters
const int Analog_Pin = 0;  // Analog pin A0
const int OutsidePin = 16; // D0
const int InsidePin= 5; // D1

Luxementr lux = Luxementr(InsidePin, OutsidePin, Analog_Pin);

uint8_t peer1[] = {0xB4, 0x8A, 0x0A, 0x89, 0x0F, 0x10};
typedef struct message {
    int insideLux;
    int outsideLux;
};
struct message myMessage;

void onSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.println("Status:");
  Serial.println(sendStatus);
}

void setup(void) {
  Serial.begin(9600);
  
  WiFi.mode(WIFI_STA);
  // Get Mac Add
  Serial.print("Mac Address: ");
  Serial.print(WiFi.macAddress());
  Serial.println("ESP-Now Sender");

  if (esp_now_init() != 0) {
    Serial.println("Problem during ESP-NOW init");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  // Register the peer
  Serial.println("Registering a peer");
  esp_now_add_peer(peer1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  Serial.println("Registering send callback function");
  esp_now_register_send_cb(onSent);

  pinMode(OutsidePin, OUTPUT);
  pinMode(InsidePin, OUTPUT);
}

void sendLux(int insideLux, int outsideLux) {
  myMessage.insideLux = insideLux;
  myMessage.outsideLux = outsideLux;

  esp_now_send(NULL, (uint8_t *) &myMessage, sizeof(myMessage));
}

void loop(void) {
  sendLux(lux.getInsideLux(), lux.getOutsideLux());

  delay(1400);
}
