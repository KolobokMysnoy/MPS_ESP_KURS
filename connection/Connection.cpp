#include "Connection.h"

Connector::Connector(uint8_t peer[6], esp_now_role role) {
  memcpy(this->peer, peer, sizeof(this->peer));
  this->role = role;
}

bool Connector::setup(int timesToTry) {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  if (!initESP(timesToTry)) {
    Serial.println("Can't init esp");
    return false;
  }

  if (!addPeer(timesToTry)) {
    Serial.println("Can't add peer");
    return false;
  }

  esp_now_register_send_cb(this->onSent);

  return true;
}

void Connector::sendData(bool inRoom) {
  esp_now_send(NULL, (uint8_t *)&inRoom, sizeof(inRoom));
}

void Connector::onSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.println("Send to:");
  Serial.println(*mac_addr);

  Serial.println("Status:");
  Serial.println(sendStatus);
}

bool Connector::initESP(int timesToTry) {
  int tries = 0;
  while (tries != timesToTry) {
    if (esp_now_init() != 0) {
      tries++;
    } else {
      esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
      return true;
    }
  }

  return false;
}

bool Connector::addPeer(int timesToTry) {
  int tries = 0;
  while (tries != timesToTry) {
    if (esp_now_add_peer(this->peer, this->role, 1, NULL, 0) != 0) {
      tries++;
    } else {
      return true;
    }
  }

  return false;
}
