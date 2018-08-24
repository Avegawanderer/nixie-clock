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
};

#endif // HVREG_H
