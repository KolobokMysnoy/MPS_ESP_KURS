#include "Connect.hpp"

Connector::Connector(esp_now_role role)
{
    this->role = role;
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
            esp_now_set_self_role(this->role);
            return true;
        }
    }

    return false;
}

int Connector::addPeer(uint8_t *peer, bool isValid, esp_now_role role, int timesToTry)
{
    esp_now_role rl;
    if (isValid)
    {
        rl = role;
    }
    else
    {
        rl = this->role;
    }
    if (this->freePeerId + 1 == maxSizeOfPeers)
    {
        Serial.println("addPeer: limit of peers");
        return -1;
    }

    memcpy(this->peers[this->freePeerId++], peer, sizeof(this->peers[0]));

    int tries = 0;
    while (tries != timesToTry)
    {
        if (esp_now_add_peer(this->peers[freePeerId], role, 1, NULL, 0) != 0)
        {
            tries++;
        }
        else
        {
            return freePeerId - 1;
        }
    }

    return -1;
}

void Connector::addFunctionOnSent(onSentFunc func)
{
    // TODO maybe remove
    this->sentFunc = func;
    esp_now_register_send_cb(func);
}

void Connector::addFunctionReceive(onReceiveFunc func)
{
    // TODO maybe remove
    this->receiveFunc = func;
    esp_now_register_recv_cb(func);
}

void Connector::getMac()
{
    Serial.print("Mac Address: ");
    Serial.print(WiFi.macAddress());
}
