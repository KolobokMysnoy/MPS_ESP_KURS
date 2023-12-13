#include "ledCtrl.hpp"
#include <Arduino.h>

LedCntrl::LedCntrl(int needLux, int accuracy, unsigned long updateInterval)
{
    this->accuracy = accuracy;
    this->needLux = needLux;

    maxLux = 0;
    currentPerc = 100;

    isSend = false;

    this->updateInterval = updateInterval;
    prevTime = 0;
    prevLux = 0;
    prevGoodPerc = 0;
    times = 0;
}

void LedCntrl::setMax(int maxLux)
{
    this->maxLux = maxLux;

    if (maxLux < needLux)
    {
        needLux = maxLux;
    }

    percentOfMax = needLux / maxLux * 100;
}

int LedCntrl::getLux(int lux, unsigned long timeNow)
{
    if (currentPerc < 0)
    {
        currentPerc = 0;
    }

    if (absMin(prevTime, timeNow) > updateInterval &&
        (std::abs(lux - needLux) < accuracy || lux > needLux) && !isSend)
    {
        // Понижаем яркость для проверки, что лента нужна
        isSend = true;
        prevTime = timeNow;
        prevLux = lux;
        currentPerc -= MINUS_CNST;

        return static_cast<int>(currentPerc);
    }

    if (isSend)
    {
        // Если 3 раза подряд нет изменения, по сравнению с исходным, то нужно выключаться
        if (std::abs(prevLux - lux) > accuracy)
        {
            // Есть изменения от первого замера
            times = 0;
            isSend = false;
        }
        else
        {
            // Нет изменений от первого замера
            times++;
            if (times == 4)
            {
                times = 0;
                isSend = false;
                currentPerc = 0;
                return static_cast<int>(currentPerc);
            }

            currentPerc -= MINUS_CNST * 2;
            return static_cast<int>(currentPerc);
        }
    }

    // В заданной погрешности
    if (std::abs(lux - needLux) < accuracy)
    {
        prevGoodPerc = currentPerc;
        return static_cast<int>(currentPerc);
    }
    if (needLux <= lux && currentPerc == 0)
    {
        return static_cast<int>(currentPerc);
    }

    if (currentPerc <= 0 && prevGoodPerc > 0)
    {
        currentPerc = prevGoodPerc;
        return currentPerc;
    }

    // Пытаемся подобрать % ленты точнее к нужному
    if (lux > needLux)
    {
        currentPerc -= MINUS_CNST;
        return static_cast<int>(currentPerc);
    }

    currentPerc += MINUS_CNST;
    return static_cast<int>(currentPerc);
};

unsigned long LedCntrl::absMin(unsigned long num1, unsigned long num2)
{
    if (num1 > num2)
        return num1 - num2;
    return num2 - num1;
}