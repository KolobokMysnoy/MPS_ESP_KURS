#ifndef Additional
#define Additional

#include <Arduino.h>

const int needLuxInside = 300;

int getLedPercent(int insideLux)
{
    // TODO Think about this method
    if (needLuxInside > insideLux)
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

#endif