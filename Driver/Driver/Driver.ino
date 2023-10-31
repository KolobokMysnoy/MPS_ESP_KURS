/* LDR Driver */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "motor.h"
#include "Curtains.h"

const int MaxValue = 512;

const int PinIn1 = 5;  // D1
const int PinIn2 = 4;  // D2

const int PinClosed = 14;  // D5
const int PinOpened = 12;  // D6

int timeToWait = 600;

MOTOR motor = MOTOR(PinIn1, PinIn2);
Curtains curt = Curtains(&motor, PinClosed, PinOpened);

uint8_t peer1[] = { 0xB4, 0x8A, 0x0A, 0x89, 0x0F, 0x10 };
typedef struct message {
  int procent;
};
struct message myMessage;

void OnDataRecv(uint8_t* mac, uint8_t* incomingData, uint8_t len) {
  memcpy(&myMessage, incomingData, sizeof(myMessage));

  switch (myMessage.procent) {
    case 0:
      curt.setNeedOpening();
      break;
    case 100:
      curt.setNeedClosing();
      break;
  }
}


void closeCurt() {
}

void pinSetup() {
  pinMode(PinClosed, INPUT);
  pinMode(PinOpened, INPUT);

  pinMode(PinIn1, OUTPUT);
  pinMode(PinIn2, OUTPUT);
}

void macOut() {
  // Get Mac Add
  Serial.print("Mac Address: ");
  Serial.print(WiFi.macAddress());
  Serial.println("ESP-Now Sender");
}

void wifiSetup() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Problem during ESP-NOW init");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void setup(void) {
  Serial.begin(9600);

  wifiSetup();
  macOut();

  pinSetup();
}

void loop(void) {
  if (curt.isNeedClosing) {
    if (!curt.isClosed()) {
      curt.close(timeToWait);
    }
  }
  if (curt.isNeedOpening) {
    if (!curt.isOpened()) {
      curt.open(timeToWait);
    }
  }
  delay(timeToWait);
}
