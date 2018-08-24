#include "hvreg.h"
#include "adc.h"
#include "stm32f10x.h"
#include <stdio.h>


void HvReg::Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    //------------------------------------//
    // Setup GPIO
    GPIO_InitTypeDef GPIO_Config;
    GPIO_Config.GPIO_Pin = GPIO_Pin_9;
    GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_Config);
    // Using remap for CH1
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);

    //------------------------------------//
    // Setup base timer
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_Prescaler = (2 - 1);      // 32MHz @64MHz core clock
    TIM_InitStructure.TIM_Period = (1000 - 1);      // 32kHz
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM1, &TIM_InitStructure);

    TIM_OCInitTypeDef TIM_OCConfig;
    TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCConfig.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCConfig.TIM_Pulse = 0x0000;
    TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCConfig.TIM_OCNPolarity = TIM_OCPolarity_High;
    TIM_OCConfig.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCConfig.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCConfig);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // Run
    TIM_Cmd(TIM1, ENABLE);
}


void HvReg::SetPwmDuty(uint16_t value)
{
    TIM_SetCompare1(TIM1, value);
}


void HvReg::RegulateHv()
{
    uint16_t adcVal = Adc::getHvfbReading();

    // TODO: PI regulator

    /////////////////////////////////////////////
    static uint8_t cnt = 0;
    if (cnt == 0)
    {
        SetPwmDuty(100);
        cnt++;
    }
    else
    {
        SetPwmDuty(400);
        cnt = 0;
    }
    /////////////////////////////////////////////
}










