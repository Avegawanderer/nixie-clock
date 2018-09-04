
#include "stm32f10x.h"
#include <stdio.h>
#include "board.h"
#include "adc.h"
#include "hvreg.h"


uint16_t Adc::adcData[Adc::NumOfVfbCh + 1];
uint16_t Adc::hvfbConvResult;
uint16_t Adc::phresConvResult;


void Adc::Init()
{
    //------------------------------------//
    // ADC is set up to make 1 regular conversion (PHRES sensor) and NumOfVfbCh regular conversions for HVFB
    // Data is copied to memory by DMA
    // ADC is running in continous mode started once by software
    // Conversion time Tconv = Sampling time + 12.5 cycles
    // ADC_SampleTime_28Cycles5: Tconv = 12.5 + 28.5 = 41 = 5,125us @8MHz
    // ADC_SampleTime_239Cycles5: Tconv = 12.5 + 239.5 = 252 = 31,5us @8MHz

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);                       // 64MHz / 8 = 8MHz (up to 14MHz)

    // Setup inputs
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // Don't care for analog
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Setup ADC
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = NumOfVfbCh + 1;    // Num of regular channels
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;            // SCAN bit = 1 for multichannel
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Setup single channel for regular group
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_239Cycles5);        // PHRES
    for (int i = 0; i < NumOfVfbCh; i++)
        ADC_RegularChannelConfig(ADC1, ADC_Channel_3, i + 2, ADC_SampleTime_71Cycles5); // HVFB

    // Enable ADC
    ADC_Cmd (ADC1,ENABLE);	//enable ADC1

    //	ADC calibration (optional, but recommended at power on)
    ADC_ResetCalibration(ADC1);	// Reset previous calibration
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);	// Start new calibration (ADC must be off at that time)
    while(ADC_GetCalibrationStatus(ADC1));

    //------------------------------------//
    // DMA setup for ADC result

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adcData;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = NumOfVfbCh + 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    // Enable interrupts
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;         // [0 .. 15], higher number = lower priority
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable DMA1 Channel1
    DMA_Cmd(DMA1_Channel1, ENABLE);

    //------------------------------------//
    // Start conversions

    // Enable requests to DMA
    ADC_DMACmd(ADC1, ENABLE);

    // Start conversion (will be endless as we are in continuous mode)
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


void Adc::DmaIsrHandler()
{
    /////////////////////////////////////////////
//    static uint16_t cnt = 0;
//    if (++cnt == 10000)
//    {
//        for (uint8_t i=0; i<NumOfVfbCh + 1; i++)
//            printf("%04X\n", adcData[i]);
//        cnt = 0;
//    }
    /////////////////////////////////////////////

    Board::SetDebugLed(Board::led1, 1);

    // Filter ADC data
    uint32_t acc = 0;
    for (uint8_t i=0; i<NumOfVfbCh; i++)
        acc += adcData[i + 1];

    // Save
    hvfbConvResult = acc / NumOfVfbCh;
    phresConvResult = adcData[0];

    // Call the regulator
    HvReg::RegulateHv();

    Board::SetDebugLed(Board::led1, 0);
}

uint16_t Adc::getHvfbReading()
{
    return hvfbConvResult;
}


extern "C" void DMA1_Channel1_IRQHandler(void);

void DMA1_Channel1_IRQHandler(void)
{
    Adc::DmaIsrHandler();
    DMA_ClearITPendingBit(DMA1_Channel1_IRQn);
}







