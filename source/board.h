#ifndef BOARD_H
#define BOARD_H

#include "stdint.h"

#define SW1         (1 << 0)
#define SW2         (1 << 1)
#define SW3         (1 << 2)
#define SW4         (1 << 3)
#define SW5         (1 << 4)
#define SW6         (1 << 5)
#define SW7         (1 << 6)
#define SW8         (1 << 7)
#define SW9         (1 << 8)
#define SW10        (1 << 9)
#define SW11        (1 << 10)
#define SW12        (1 << 11)
#define SW13        (1 << 12)
#define SW14        (1 << 13)
#define SW15        (1 << 14)
#define SW16        (1 << 15)


class Board
{
public:

    typedef enum {
        led1,
        led2
    } eDebugLed;

    static void Init();
    static void SetDebugLed(eDebugLed led, bool isOn);
    static uint16_t GetRawSwitches();
};

#endif // BOARD_H
