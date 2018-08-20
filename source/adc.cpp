#include "adc.h"
#include "stm32f10x.h"
#include <stdio.h>

static uint16_t adcData[5];

void Adc::Init()
{
    //------------------------------------//
    // ADC is set up to make 1 regular conversion (PHRES sensor) and 4 regular conversions for HVFB
    // Data is copied to memory by DMA
    // ADC is started by TIM4 CC4 event

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);       // 64MHz / 8 = 8MHz (up to 14MHz)

    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;      //DISABLE;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 5;                 // Num of regular channels
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;            // SCAN bit = 1 for multichannel
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //ADC_ExternalTrigConv_T4_CC4;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Setup single channel for regular group
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_28Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_28Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_28Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_28Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_28Cycles5);

    // Enable ADC
    ADC_Cmd (ADC1,ENABLE);	//enable ADC1

    //	ADC calibration (optional, but recommended at power on)
    ADC_ResetCalibration(ADC1);	// Reset previous calibration
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);	// Start new calibration (ADC must be off at that time)
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);	// start conversion (will be endless as we are in continuous mode)

    //------------------------------------//
    // DMA setup for ADC result

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adcData;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 5;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    // Разрешение прерываний
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;         // [0 .. 15], higher number = lower priority
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Разрешить передачу DMA1 Channel1
    DMA_Cmd(DMA1_Channel1, ENABLE);


    //------------------------------------//
    // Start conversions

    // Enable requests to DMA
    ADC_DMACmd(ADC1, ENABLE);


//    uint16_t adc_value;
//    while (1)
//    {
//        adc_value = ADC_GetConversionValue(ADC1);
//        printf("%d\r\n", adc_value);
//    }
}


extern "C" void DMA1_Channel1_IRQHandler(void);

void DMA1_Channel1_IRQHandler(void)
{
    //printf("DMA\r\n");
}







