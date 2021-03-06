
#include "board.h"

#include "stm32f10x.h"
#include "dwt_delay.h"




void Board::Init()
{
    // Enable HSE generator
    RCC_HSEConfig(RCC_HSE_ON);
    while (RCC_WaitForHSEStartUp() != SUCCESS);

    // Enable Prefetch Buffer - do it here because system_stm32f10x.c is set up to simply leave HSI clock source due to
    // 16MHz quarz (not the standard 8MHz)
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    // Flash 2 wait state
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

    // Enable PLL
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_4);       // 16MHz * 4 = 64MHz
    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    // HSI stays enabled as it is used for FLASH programming

    // Setup AHB and APB bus clock
    RCC_HCLKConfig(RCC_SYSCLK_Div1);        // AHB bus clock (max 72MHz)
    RCC_PCLK1Config(RCC_HCLK_Div2);         // APB1 bus clock (max 36MHz)
    RCC_PCLK2Config(RCC_HCLK_Div1);         // APB2 bus clock (max 72MHz)

    // Setup peripherals clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    // The rest is initialized by modules themself

    // Init global interrupt options
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);     // 0 bits for pre-emption priority
                                                        // 4 bits for subpriority

    // Init debug module used for delays
    DWT_Init();

    // Debug LEDs
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // Switches SW1 and SW2
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Switches SW3..5
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Switches SW6..12
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Switches SW13
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Switches SW14..16
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // Update global variable for core clock
    SystemCoreClockUpdate();
}


void Board::SetDebugLed(eDebugLed led, bool isOn)
{
    uint16_t pin = (led == led1) ? GPIO_Pin_5 : GPIO_Pin_6;
    if (isOn)
        GPIO_SetBits(GPIOE, pin);
    else
        GPIO_ResetBits(GPIOE, pin);
}


uint16_t Board::GetRawSwitches()
{
    uint16_t swState = 0;
    swState |= ((GPIOA->IDR >> 11) & 0x03) << 0;        // Switches SW1 and SW2
    swState |= ((GPIOC->IDR >> 10) & 0x07) << 2;        // Switches SW3..5
    swState |= ((GPIOD->IDR >> 0) & 0x1F) << 5;         // Switches SW6..10
    swState |= ((GPIOD->IDR >> 6) & 0x03) << 10;        // Switches SW11..12
    swState |= ((GPIOB->IDR >> 5) & 0x01) << 12;        // Switches SW13
    swState |= ((GPIOE->IDR >> 0) & 0x07) << 14;        // Switches SW14..16
    // Active low
    swState = ~swState;
    return swState;
}
