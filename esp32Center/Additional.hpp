#ifndef Additional
#define Additional

#include <Arduino.h>
#include <cmath>

const int luxAccuracy = 10;

// Return value to set to be match to need lux
// if already at need lux then return -1
int getLedPercent(int insideLux, int needLux, int currentPerc, int maxLuxOfLed)
{
    static int percWithOnlyLed = 0;
    if (percWithOnlyLed == 0)
    {
        float percentToSet = (static_cast<float>(needLux) /
                              (maxLuxOfLed - needLux)) *
                             100;

        percWithOnlyLed = static_cast<int>(min(percentToSet, static_cast<float>(100.0)));
    }

    // If the current light level is inside needed range return -1
    if (std::abs(insideLux - needLux) < luxAccuracy)
    {
        return -1;
    }

    // If need lux is more than inside lux than turn on the led
    if (needLux - insideLux > 0)
    {
        return percWithOnlyLed;
    }

    // if need lux is less than inside lux turn off the led
    return 0;
}

// Return percent of outside
int getMotorPercent(int outsideLux, int insideLux)
{
    if (outsideLux < insideLux)
    {
        return 100;
    }

    return 0;
}

template <typename T, size_t N>
bool arraysEqual(const T (&arr1)[N], const T (&arr2)[N])
{
    for (size_t i = 0; i < N; ++i)
    {
        if (arr1[i] != arr2[i])
        {
            return false;
        }
    }
    return true;
}

#endif