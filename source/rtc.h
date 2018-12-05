#ifndef RTC_H
#define RTC_H

#include "stdint.h"


typedef struct
{
    uint8_t RTC_Hours;
    uint8_t RTC_Minutes;
    uint8_t RTC_Seconds;
    uint8_t RTC_Date;
    uint8_t RTC_Wday;
    uint8_t RTC_Month;
    uint16_t RTC_Year;
} RTC_DateTimeTypeDef;

class Rtc
{
public:
    static void Init();
    static uint32_t GetRawCounter();
    static void SetRawCounter(uint32_t value);
    static void GetDateTime(uint32_t RTC_Counter, RTC_DateTimeTypeDef* RTC_DateTimeStruct);
    static uint32_t GetRTC_Counter(RTC_DateTimeTypeDef* RTC_DateTimeStruct);
    static uint8_t IncWrap(uint8_t value, int8_t increment, uint8_t base);
private:

};

#endif // RTC_H
