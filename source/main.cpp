
#include <stdio.h>
#include "stm32f10x.h"
#include "dwt_delay.h"
#include "board.h"
#include "adc.h"
#include "hvreg.h"
#include "rgbled.h"
#include "tubectrl.h"
#include "rtc.h"

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
           Tube driver

    USART1 - Bluetooth
    USART2 - DS_1WIRE temperature sensor

*/



int main()
{
    uint8_t rgb[3];
    uint32_t i,j;
    uint8_t tmr;
    char str[13];
    RTC_DateTimeTypeDef dateTime;
    int8_t inc;

#ifdef __DEBUG
    initialise_monitor_handles();
#endif
    Board::Init();
    HvReg::Init();
    RgbLed::Init();
    Adc::Init();
    TubeCtrl::Init();
    Rtc::Init();

#ifdef __DEBUG
    printf("Core clock: %lu\n", SystemCoreClock);
#endif
    rgb[0] = rgb[1] = rgb[2] = 255;
    RgbLed::SetRgb(rgb[0],rgb[1],rgb[2]);
    DWT_DelayMs(1000);
    rgb[0] = 200;
    rgb[1] = 0;
    rgb[2] = 255;
    RgbLed::SetRgb(rgb[0],rgb[1],rgb[2]);
    DWT_DelayMs(100);

    //TubeCtrl::SetTubeDigits(1,2,3,4,5,6);
    TubeCtrl::SetTubeDots(0, 0, 0, 0, 0, 0);
    TubeCtrl::SetTubeCommas(0, 1, 0, 1, 0, 0);

    i = 0;
    tmr = 0;
    while(1)
    {
        DWT_DelayUs(2000);
        TubeCtrl::ProcessIndication();

        //rtcCounter = Rtc::GetRawCounter();
        //sprintf(str, "%06d", rtcCounter);

        Rtc::GetDateTime(Rtc::GetRawCounter(), &dateTime);
        sprintf(str, "%02d%02d%02d", dateTime.RTC_Hours, dateTime.RTC_Minutes, dateTime.RTC_Seconds);
        TubeCtrl::SetTubeDigits(str);

        if (++tmr >= 100)
        {
            tmr = 0;
            uint16_t rawSwitches = Board::GetRawSwitches();
            if (rawSwitches & (SW1 | SW2 | SW3 | SW4))
            {
                inc = ((rawSwitches & (SW1 | SW2)) == SW1) ? 1 :
                     (((rawSwitches & (SW1 | SW2)) == SW2) ? -1 : 0);
                dateTime.RTC_Hours = Rtc::IncWrap(dateTime.RTC_Hours, inc, 24);

                inc = ((rawSwitches & (SW3 | SW4)) == SW3) ? 1 :
                     (((rawSwitches & (SW3 | SW4)) == SW4) ? -1 : 0);
                dateTime.RTC_Minutes = Rtc::IncWrap(dateTime.RTC_Minutes, inc, 60);

                Rtc::SetRawCounter(Rtc::GetRTC_Counter(&dateTime));
            }
        }

//        if (++i >= 500)
//        {
//            i = 0;
//            if (++tmr >= 10)
//                tmr = 0;
//            TubeCtrl::SetTubeDigits( tmr,
//                (tmr+1)%10,
//                (tmr+2)%10,
//                (tmr+3)%10,
//                (tmr+4)%10,
//                (tmr+5)%10);
//        }

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




