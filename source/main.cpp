
#include "stm32f10x.h"
#include "dwt_delay.h"
#include "board.h"
#include "adc.h"

/*

Peripherals map:

    ADC1 - HVFB and PHRES sensor capture (Fs = 5-10kHz)

    TIM4 - Tube LEDs,
           ADC1 conversion trigger
    TIM1 - HV PWM source,
           IR PWM source
    TIM3 - Sound PWM

    SPI1 - SD card
    SPI2 - NRF24L01+

    USART1 - Bluetooth
    USART2 - DS_1WIRE temperature sensor

*/

extern "C" void initialise_monitor_handles(void);

int main()
{
    initialise_monitor_handles();
    Board::Init();
    Adc::Init();

    while(1)
    {
        Board::SetDebugLed(Board::led1, 1);
        Board::SetDebugLed(Board::led2, 1);
        DWT_DelayMs(250);
//        Board::SetDebugLed(Board::led1, 0);
//        Board::SetDebugLed(Board::led2, 1);
//        DWT_DelayMs(250);
//        Board::SetDebugLed(Board::led1, 1);
//        Board::SetDebugLed(Board::led2, 0);
//        DWT_DelayMs(250);
//        Board::SetDebugLed(Board::led1, 0);
//        Board::SetDebugLed(Board::led2, 0);
//        DWT_DelayMs(250);
    }

    return 0;
}


