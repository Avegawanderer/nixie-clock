#ifndef RGBLED_H
#define RGBLED_H

#include "stdint.h"

class RgbLed
{
public:
    static void Init();
    static void SetRgb(uint8_t r, uint8_t g, uint8_t b);
};

#endif // RGBLED_H
