#ifndef BOARD_H
#define BOARD_H


class Board
{
public:

    typedef enum {
        led1,
        led2
    } eDebugLed;

    static void Init();
    static void SetDebugLed(eDebugLed led, bool isOn);
};

#endif // BOARD_H
