#ifndef HVREG_H
#define HVREG_H

#include "stdint.h"


class HvReg
{
public:
    static void Init();
    static void RegulateHv();

private:
    static void SetPwmDuty(uint16_t value);
    static const int TIM1ARR = 4000;            // Defines PWM resolution (more = greater) and PWM frequency (32MHz / ARR)
    static float pid_iterm;
    static float kp;
    static float ki;
};

#endif // HVREG_H
