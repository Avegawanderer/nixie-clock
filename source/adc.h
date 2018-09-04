#ifndef ADC_H
#define ADC_H

#include "stdint.h"


class Adc
{
public:
    static void Init();
    static void DmaIsrHandler();
    static uint16_t getHvfbReading();

private:
    static const uint8_t NumOfVfbCh = 14;       // 1 to 15
    static uint16_t adcData[NumOfVfbCh + 1];   // One extra sample to hold PHRES sensor reading
    static uint16_t hvfbConvResult;
    static uint16_t phresConvResult;
};

#endif // ADC_H
