
#include <stdio.h>
#include "stm32f10x.h"
#include "dwt_delay.h"
#include "board.h"
#include "adc.h"
#include "hvreg.h"
#include "rgbled.h"
#include "tubectrl.h"


extern "C" void initialise_monitor_handles(void);
extern "C" void hard_fault_handler_c(unsigned int * hardfault_args, unsigned int r4, unsigned int r5, unsigned int r6);

void hard_fault_handler_c(unsigned int * hardfault_args, unsigned int r4, unsigned int r5, unsigned int r6)
{
  printf ("[Hard Fault]\n"); // After Joseph Yiu

  printf ("r0 = %08X, r1 = %08X, r2 = %08X, r3 = %08X\n",
    hardfault_args[0], hardfault_args[1], hardfault_args[2], hardfault_args[3]);
  printf ("r4 = %08X, r5 = %08X, r6 = %08X, sp = %08X\n",
    r4, r5, r6, (unsigned int)&hardfault_args[8]);
  printf ("r12= %08X, lr = %08X, pc = %08X, psr= %08X\n",
    hardfault_args[4], hardfault_args[5], hardfault_args[6], hardfault_args[7]);

  printf ("bfar=%08X, cfsr=%08X, hfsr=%08X, dfsr=%08X, afsr=%08X\n",
    *((volatile unsigned int *)(0xE000ED38)),
    *((volatile unsigned int *)(0xE000ED28)),
    *((volatile unsigned int *)(0xE000ED2C)),
    *((volatile unsigned int *)(0xE000ED30)),
    *((volatile unsigned int *)(0xE000ED3C)) );

  while(1);
}


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



int main()
{
    uint8_t rgb[3];
    uint32_t i;

#ifdef __DEBUG
    initialise_monitor_handles();
#endif
    Board::Init();
    HvReg::Init();
    RgbLed::Init();
    Adc::Init();
    TubeCtrl::Init();

#ifdef __DEBUG
    printf("Core clock: %lu\n", SystemCoreClock);
#endif
    rgb[0] = rgb[1] = rgb[2] = 255;
    RgbLed::SetRgb(rgb[0],rgb[1],rgb[2]);
    DWT_DelayMs(1000);
    rgb[0] = rgb[1] = rgb[2] = 0;
    RgbLed::SetRgb(rgb[0],rgb[1],rgb[2]);
    DWT_DelayMs(1000);

    TubeCtrl::SetTubeDigits(1,2,3,4,5,6);

    while(1)
    {
        DWT_DelayUs(1000);
        TubeCtrl::ProcessIndication();


//        for (i=0; i<3; i++)
//        {
//            while(rgb[i] < 255)
//            {
//                rgb[i]++;
//                RgbLed::SetRgb(rgb[0],rgb[1],rgb[2]);
//                DWT_DelayUs(5000);
//            }

//            while(rgb[i] > 0)
//            {
//                rgb[i]--;
//                RgbLed::SetRgb(rgb[0],rgb[1],rgb[2]);
//                DWT_DelayUs(5000);
//            }
//        }

//        Board::SetDebugLed(Board::led1, 0);
//        Board::SetDebugLed(Board::led2, 1);
    }

    return 0;
}




