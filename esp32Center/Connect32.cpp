#include "Connect32.hpp"

Connector::Connector()
{
}

bool Connector::setup(int timesToTry)
{
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);

    if (!initESP(timesToTry))
    {
        Serial.println("Can't init esp");
        return false;
    }

    Serial.println("Connector: ESP init correct");

    return true;
}

void Connector::sendData(int idOfPeer, uint8_t *data, uint8_t size)
{
    if (idOfPeer < this->freePeerId)
    {
        esp_now_send(this->peers[idOfPeer], data, size);
    }
    else
    {
        Serial.println("sendData: IdOfPeer > freePeerId");
    }
}

bool Connector::initESP(int timesToTry)
{
    int tries = 0;
    while (tries != timesToTry)
    {
        if (esp_now_init() != 0)
        {
            tries++;
        }
        else
        {
            return true;
        }
    }

    return false;
}

int Connector::addPeer(uint8_t *peerMac, int timesToTry)
{
    if (this->freePeerId + 1 == maxSizeOfPeers)
    {
        Serial.println("Connector addPeer: limit of peers");
        return -1;
    }

    memcpy(this->peers[this->freePeerId++], peerMac, sizeof(this->peers[0]));

    // TODO maybe add channel and false encrypt
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, peerMac, 6);

    int tries = 0;
    while (tries != timesToTry)
    {
        if (esp_now_add_peer(&peerInfo) != 0)
        {
            tries++;
        }
        else
        {
            return freePeerId - 1;
        }
    }
    Serial.println("Can't connect to PEER");
    return -1;
}

void Connector::addFunctionOnSent(onSentFunc func)
{
    // TODO maybe remove
    this->sentFunc = func;
    Serial.println("Connector: Add send func");
    esp_now_register_send_cb(func);
}

void Connector::addFunctionReceive(onReceiveFunc func)
{
    // TODO maybe remove
    this->receiveFunc = func;
    Serial.println("Connector: Add receive func");
    esp_now_register_recv_cb(func);
}

void Connector::getMac()
{
    Serial.print("Mac Address: ");
    Serial.print(WiFi.macAddress());
}
