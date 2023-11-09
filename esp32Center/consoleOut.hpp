#ifndef CONSOLE_OUT
#define CONSOLE_OUT

#include <Arduino.h>

template <typename T, size_t N>
void printArray(const T (&arr)[N])
{
    for (size_t i = 0; i < N; ++i)
    {
        Serial.print(arr[i], HEX);
        Serial.print(":");
    }
    Serial.println("");
}

template <typename T, size_t N>
void outputMacLen(int len, const T (&arr)[N])
{
    Serial.print("Receive data from mac: ");
    printArray(arr);
    Serial.print("Bytes received: ");
    Serial.println(len);
}

void outputPerson(bool isInside)
{
    Serial.print("Person ");
    if (isInside == 1)
    {
        Serial.println("inside room");
    }
    else
    {
        Serial.println("outside room");
    }
    Serial.println();
}

void outputLux(int insideLux, int outsideLux)
{
    Serial.print("inside: ");
    Serial.println(insideLux);
    Serial.print("Outsude: ");
    Serial.println(outsideLux);
    Serial.println();
}

#endif