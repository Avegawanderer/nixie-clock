
#include "stm32f10x.h"
#include "rgbled.h"

void RgbLed::Init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    //------------------------------------//
    // Setup GPIO
    GPIO_InitTypeDef GPIO_Config;
    GPIO_Config.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Config.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOD, &GPIO_Config);

    // Using remap for TIM4
    GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);

    //------------------------------------//
    // Setup base timer
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_Prescaler = (64 - 1);     // 1MHz @64MHz core clock
    TIM_InitStructure.TIM_Period = (255 - 1);       // 3.9kHz
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM4, &TIM_InitStructure);

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
    TIM_OC1Init(TIM4, &TIM_OCConfig);
    TIM_OC2Init(TIM4, &TIM_OCConfig);
    TIM_OC3Init(TIM4, &TIM_OCConfig);

    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

    // Run
    TIM_Cmd(TIM4, ENABLE);

    // Enable the timer PWM outputs
    TIM_CtrlPWMOutputs(TIM4, ENABLE);
}


void RgbLed::SetRgb(uint8_t r, uint8_t g, uint8_t b)
{
    // Red and Blue are swapped on IN14 board
    TIM_SetCompare3(TIM4, r);
    TIM_SetCompare2(TIM4, g);
    TIM_SetCompare1(TIM4, b);
}







