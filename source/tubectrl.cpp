
#include "stm32f10x.h"
#include "dwt_delay.h"
#include "tubectrl.h"


uint8_t TubeCtrl::tube_digits[6];
uint8_t TubeCtrl::tube_dots[6];
uint8_t TubeCtrl::tube_commas[6];
uint8_t TubeCtrl::bulb_dots[2];



// First group (H1 (first left to right) - H3 (third left to right)
uint8_t TubeCtrl::encodeTable13[12] = {
    13,         // bit position for "0"
    6,          // bit position for "1"
    5,
    4,
    3,          // "4"
    1,
    0,          // "6"
    2,
    15,
    14,         // bit position for "9"
    7,          // bit position for "."
    12,         // bit position for ","
};

// Second group (H4 (fourth left to right) - H6 (sixs left to right)
uint8_t TubeCtrl::encodeTable46[12] = {
    17,         // bit position for "0"
    10,         // bit position for "1"
    9,
    8,
    20,
    22,
    23,
    21,
    19,
    18,          // bit position for "9"
    16,          // bit position for "."
    11,          // bit position for ","
};

void TubeCtrl::Init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    //------------------------------------//
    // Setup GPIO

    // TUBE_A1..A6
    GPIO_InitTypeDef GPIO_Config;
    GPIO_Config.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_8;
    GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Config.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOE, &GPIO_Config);
    GPIO_ResetBits(GPIOE, GPIO_Config.GPIO_Pin);

    GPIO_Config.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIO_Config);

    GPIO_Config.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_Config);

    // SPI pins (shared with NRF24L01+)
    GPIO_Config.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Config.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOE, &GPIO_Config);
    GPIO_ResetBits(GPIOE, GPIO_Config.GPIO_Pin);

    // SPI
    SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;       // 250kHz @ 32MHz APB1 clock
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;               // don't care
    SPI_Init(SPI2, &SPI_InitStruct);

    SPI_Cmd(SPI2, ENABLE);
    //------------------------------------//

    // Fill the shift registers
    for (int i=0; i<3; i++)
    {
        SPI_I2S_SendData(SPI2, 0);
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
    }
    // Set STC (push clocked data to outputs)
    GPIO_SetBits(GPIOE, GPIO_Pin_8);

    for (int i=0; i<6; i++)
    {
        tube_digits[i] = 0;
        tube_commas[i] = 0;
        tube_dots[i] = 0;
    }
    for (int i=0; i<2; i++)
    {
        bulb_dots[i] = 0;
    }
}


void TubeCtrl::SetTubeDigits(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6)
{
    tube_digits[0] = d1;
    tube_digits[1] = d2;
    tube_digits[2] = d3;
    tube_digits[3] = d4;
    tube_digits[4] = d5;
    tube_digits[5] = d6;
}


void TubeCtrl::SetTubeDots(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6)
{
    tube_dots[0] = d1;
    tube_dots[1] = d2;
    tube_dots[2] = d3;
    tube_dots[3] = d4;
    tube_dots[4] = d5;
    tube_dots[5] = d6;
}


void TubeCtrl::SetTubeCommas(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6)
{
    tube_commas[0] = d1;
    tube_commas[1] = d2;
    tube_commas[2] = d3;
    tube_commas[3] = d4;
    tube_commas[4] = d5;
    tube_commas[5] = d6;
}


void TubeCtrl::SetBulbDots(uint8_t b1, uint8_t b2)
{
    bulb_dots[0] = b1;
    bulb_dots[1] = b2;
}


void TubeCtrl::SetBitInVector(uint8_t bitPosition, uint8_t *vector)
{
    uint8_t byteNum = bitPosition >> 3;
    vector[byteNum] |= 1 << (bitPosition & 0x7);
}


void TubeCtrl::ProcessIndication()
{
    static uint8_t nextTube = 0;
    uint8_t encVector[3] = {0,0,0};

    // Disable anode switches
    GPIO_ResetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    DWT_DelayUs(10);
    // Set STC low
    GPIO_ResetBits(GPIOE, GPIO_Pin_8);

    switch (nextTube)
    {
        case 0:
            // Encode two digits for simultaneous displaying
            SetBitInVector(encodeTable13[tube_digits[0]], encVector);
            if (tube_dots[0])
                SetBitInVector(encodeTable13[10], encVector);
            if (tube_commas[0])
                SetBitInVector(encodeTable13[11], encVector);

            SetBitInVector(encodeTable46[tube_digits[3]], encVector);
            if (tube_dots[3])
                SetBitInVector(encodeTable46[10], encVector);
            if (tube_commas[3])
                SetBitInVector(encodeTable46[11], encVector);

            // Send control vector for cathodes
            SPI_I2S_SendData(SPI2, encVector[2]);       // Start with most significant byte
            while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
            SPI_I2S_SendData(SPI2, encVector[1]);
            while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
            SPI_I2S_SendData(SPI2, encVector[0]);
            while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
            // Set STC (push clocked data to outputs)
            GPIO_SetBits(GPIOE, GPIO_Pin_8);
            DWT_DelayUs(10);
            // Enable anode drivers
            GPIO_SetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_13);
            // Select digit for next entry
            nextTube = 1;

            // Update little bulbs
            if (bulb_dots[0])
                GPIO_SetBits(GPIOB, GPIO_Pin_0);
            else
                GPIO_ResetBits(GPIOB, GPIO_Pin_0);

            if (bulb_dots[1])
                GPIO_SetBits(GPIOC, GPIO_Pin_5);
            else
                GPIO_ResetBits(GPIOC, GPIO_Pin_5);
            break;
        case 1:
            // Encode two digits for simultaneous displaying
            SetBitInVector(encodeTable13[tube_digits[1]], encVector);
            if (tube_dots[1])
                SetBitInVector(encodeTable13[10], encVector);
            if (tube_commas[1])
                SetBitInVector(encodeTable13[11], encVector);

            SetBitInVector(encodeTable46[tube_digits[4]], encVector);
            if (tube_dots[4])
                SetBitInVector(encodeTable46[10], encVector);
            if (tube_commas[4])
                SetBitInVector(encodeTable46[11], encVector);

            // Send control vector for cathodes
            SPI_I2S_SendData(SPI2, encVector[2]);       // Start with most significant byte
            while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
            SPI_I2S_SendData(SPI2, encVector[1]);
            while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
            SPI_I2S_SendData(SPI2, encVector[0]);
            while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
            // Set STC (push clocked data to outputs)
            GPIO_SetBits(GPIOE, GPIO_Pin_8);
            DWT_DelayUs(10);
            // Enable anode drivers
            GPIO_SetBits(GPIOE, GPIO_Pin_10 | GPIO_Pin_14);
            // Select digit for next entry
            nextTube = 2;
            break;
        case 2:
            // Encode two digits for simultaneous displaying
            SetBitInVector(encodeTable13[tube_digits[2]], encVector);
            if (tube_dots[2])
                SetBitInVector(encodeTable13[10], encVector);
            if (tube_commas[2])
                SetBitInVector(encodeTable13[11], encVector);

            SetBitInVector(encodeTable46[tube_digits[5]], encVector);
            if (tube_dots[5])
                SetBitInVector(encodeTable46[10], encVector);
            if (tube_commas[5])
                SetBitInVector(encodeTable46[11], encVector);

            // Send control vector for cathodes
            SPI_I2S_SendData(SPI2, encVector[2]);       // Start with most significant byte
            while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
            SPI_I2S_SendData(SPI2, encVector[1]);
            while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
            SPI_I2S_SendData(SPI2, encVector[0]);
            while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
            // Set STC (push clocked data to outputs)
            GPIO_SetBits(GPIOE, GPIO_Pin_8);
            DWT_DelayUs(10);
            // Enable anode drivers
            GPIO_SetBits(GPIOE, GPIO_Pin_12 | GPIO_Pin_15);
            // Select digit for next entry
            nextTube = 0;
            break;
    }
}



