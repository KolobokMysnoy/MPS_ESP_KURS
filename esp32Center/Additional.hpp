#ifndef Additional
#define Additional

#include <Arduino.h>

const int needLuxInside = 300;

int getLedPercent(int insideLux, int needLux = needLuxInside)
{
    // TODO Think about this method
    if (needLux > insideLux)
    {
        return 100;
    }

    return 0;
}

int getMotorPercent(int outsideLux, int insideLux)
{
    // TODO Think about this method
    if (outsideLux < insideLux)
    {
        return 100;
    }

    return 0;
}


template <typename T, size_t N>
bool arraysEqual(const T (&arr1)[N], const T (&arr2)[N]) {
    for (size_t i = 0; i < N; ++i) {
        if (arr1[i] != arr2[i]) {
            return false; 
        }
    }
    return true;
}

template <typename T, size_t N>
void printArray(const T (&arr)[N]) {
    for (size_t i = 0; i < N; ++i) {
        Serial.print(arr[i], HEX);
        Serial.print(":");
    }
    Serial.println("");
}

#endif