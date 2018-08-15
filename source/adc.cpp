#include "adc.h"
#include "stm32f10x.h"

void Adc::Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);       // 64MHz / 8 = 8MHz (up to 14MHz)


}
