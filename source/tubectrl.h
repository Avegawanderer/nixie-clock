#ifndef TUBECTRL_H
#define TUBECTRL_H

#include "stdint.h"

class TubeCtrl
{

public:
    static void Init();
    static void SetTubeDigits(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6);  // 0 to 9
    static void SetTubeDots(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6);    // 0 or 1
    static void SetTubeCommas(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6);  // 0 or 1
    static void SetBulbDots(uint8_t b1, uint8_t b2);                                                    // 0 or 1

    static void ProcessIndication();
private:
    static uint8_t tube_digits[6];
    static uint8_t tube_commas[6];
    static uint8_t tube_dots[6];
    static uint8_t bulb_dots[2];        // two separate little bulbs between tubes 2 and 3, 4 and 5
    static uint8_t encodeTable13[12];   // 10 digits + dot + comma
    static uint8_t encodeTable46[12];

    static void SetBitInVector(uint8_t bitPosition, uint8_t *vector);
    static void Encode(uint8_t dig1, uint8_t dig2, uint8_t *encBitVector);

};

#endif // TUBECTRL_H
