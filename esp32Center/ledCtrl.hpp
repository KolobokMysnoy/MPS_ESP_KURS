#ifndef LED_CTRL_HPP
#define LED_CTRL_HPP

#include <cmath> // abs

#define MINUS_CNST 5

class LedCntrl
{
private:
    int maxLux;
    int needLux;
    float currentPerc;
    float percentOfMax;
    float prevGoodPerc;
    int accuracy;
    int times;

    bool isSend;
    unsigned long updateInterval;
    unsigned long prevTime;
    int prevLux;
    bool lampOff;

public:
    LedCntrl(int needLux, int accuracy = 8, unsigned long updateInterval = 5 * 1000);

    void setMax(int maxLux);

    int getLux(int lux, unsigned long timeNow);

private:
    unsigned long absMin(unsigned long num1, unsigned long num2);
};

#endif