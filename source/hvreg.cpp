
#include "stm32f10x.h"
#include <stdio.h>
#include "board.h"
#include "hvreg.h"
#include "adc.h"


void HvReg::Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    //------------------------------------//
    // Setup GPIO
    GPIO_InitTypeDef GPIO_Config;
    GPIO_Config.GPIO_Pin = GPIO_Pin_9;
    GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Config.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOE, &GPIO_Config);
    // Using remap for CH1
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);

    //------------------------------------//
    // Setup base timer
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_Prescaler = (2 - 1);      // 32MHz @64MHz core clock
    TIM_InitStructure.TIM_Period = (TIM1ARR - 1);      // 32kHz
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM1, &TIM_InitStructure);

    //------------------------------------//
    // Setup PWM generation
    TIM_OCInitTypeDef TIM_OCConfig;
    TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCConfig.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCConfig.TIM_Pulse = 0;
    TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCConfig.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCConfig.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCConfig.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCConfig);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // Run
    TIM_Cmd(TIM1, ENABLE);

    // Enable the timer PWM outputs
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}


void HvReg::SetPwmDuty(uint16_t value)
{
    TIM_SetCompare1(TIM1, value);
}


void HvReg::RegulateHv()
{
    Board::SetDebugLed(Board::led2, 1);
    uint16_t adcVal = Adc::getHvfbReading();        // adc readings, [0..4095]

    // TODO: PI regulator
    uint16_t setpoint = (uint16_t)(((160.0f * 0.01f) / 3.3f) * 4095);
    int16_t error = setpoint - adcVal;
    float kp = 0.5;
    float pidSum = error * kp;
    if (pidSum < 0)
        pidSum = 0;
    else if (pidSum > TIM1ARR / 2)
        pidSum = TIM1ARR / 2;

    SetPwmDuty(pidSum);
    Board::SetDebugLed(Board::led2, 0);
}










