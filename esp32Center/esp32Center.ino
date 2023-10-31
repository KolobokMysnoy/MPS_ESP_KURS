#include <esp_now.h>
#include <WiFi.h>

typedef struct message {
    int insideLux;
    int outsideLux;
};
struct message myMessage;

typedef struct MotorStruct {
  int percent;
};
struct MotorStruct motorStr;

uint8_t motorAdress[] = {0x84, 0xf3, 0xeb, 0xbf, 0xc0, 0x27};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myMessage, incomingData, sizeof(myMessage));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("inside: ");
  Serial.println(myMessage.insideLux);
  Serial.print("Outsude: ");
  Serial.println(myMessage.outsideLux);
  Serial.println();
}

void data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nStatus of Last Message Sent:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void motorSetup(){
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, motorAdress, 6);
  
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;     
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    // todo delete later
    while(esp_now_add_peer(&peerInfo) != ESP_OK) {
      delay(1000);
    }
  }
}

void setupEsp() {
  WiFi.mode(WIFI_STA);

  Serial.print("Mac Address: ");
  Serial.print(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(data_sent);
  
  motorSetup();
  Serial.println("ESP-NOW initialized successfully");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setupEsp();
}

void loop() {
  // put your main code here, to run repeatedly:
  motorStr.percent = 0;
  esp_err_t outcome = esp_now_send(motorAdress, (uint8_t *) &motorStr, sizeof(motorStr));
  delay(4000);

  motorStr.percent = 100;
  outcome = esp_now_send(motorAdress, (uint8_t *) &motorStr, sizeof(motorStr));
  delay(4000);
}
