#include "rtc.h"
#include "stm32f10x.h"


// (UnixTime = 00:00:00 01.01.1970 = JD0 = 2440588)
#define JULIAN_DATE_BASE    2440588


void Rtc::Init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    // Init if RTC is disabled
    if ((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN)
    {
        // Reset backup data
        BKP_DeInit();

        // Enable LSE
        RCC_LSEConfig(RCC_LSE_ON);
        while ((RCC->BDCR & RCC_BDCR_LSERDY) != RCC_BDCR_LSERDY) {}

        // Select LSE
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        // Enable RTC
        RCC_RTCCLKCmd(ENABLE);

        // Set prescaler for seconds
        RTC_WaitForLastTask();
        RTC_SetPrescaler(32768 - 1);

        // Reset counter
        RTC_WaitForLastTask();
        RTC_SetCounter(0);
    }
    RTC_WaitForSynchro();
}


uint32_t Rtc::GetRawCounter()
{
    RTC_WaitForLastTask();
    return RTC_GetCounter();
}


void Rtc::SetRawCounter(uint32_t value)
{
    RTC_WaitForLastTask();
    RTC_SetCounter(value);
    RTC_WaitForSynchro();
}


// Get current date
void Rtc::GetDateTime(uint32_t RTC_Counter, RTC_DateTimeTypeDef* RTC_DateTimeStruct)
{
    unsigned long time;
    unsigned long t1, a, b, c, d, e, m;
    int year = 0;
    int mon = 0;
    int wday = 0;
    int mday = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;
    uint64_t jd = 0;;
    uint64_t jdn = 0;

    jd = ((RTC_Counter+43200)/(86400>>1)) + (2440587<<1) + 1;
    jdn = jd>>1;

    time = RTC_Counter;
    t1 = time/60;
    sec = time - t1*60;

    time = t1;
    t1 = time/60;
    min = time - t1*60;

    time = t1;
    t1 = time/24;
    hour = time - t1*24;

    wday = jdn%7;

    a = jdn + 32044;
    b = (4*a+3)/146097;
    c = a - (146097*b)/4;
    d = (4*c+3)/1461;
    e = c - (1461*d)/4;
    m = (5*e+2)/153;
    mday = e - (153*m+2)/5 + 1;
    mon = m + 3 - 12*(m/10);
    year = 100*b + d - 4800 + (m/10);

    RTC_DateTimeStruct->RTC_Year = year;
    RTC_DateTimeStruct->RTC_Month = mon;
    RTC_DateTimeStruct->RTC_Date = mday;
    RTC_DateTimeStruct->RTC_Hours = hour;
    RTC_DateTimeStruct->RTC_Minutes = min;
    RTC_DateTimeStruct->RTC_Seconds = sec;
    RTC_DateTimeStruct->RTC_Wday = wday;
}


// Convert Date to Counter
uint32_t Rtc::GetRTC_Counter(RTC_DateTimeTypeDef* RTC_DateTimeStruct)
{
    uint8_t a;
    uint16_t y;
    uint8_t m;
    uint32_t JDN;

    a=(14-RTC_DateTimeStruct->RTC_Month)/12;
    y=RTC_DateTimeStruct->RTC_Year+4800-a;
    m=RTC_DateTimeStruct->RTC_Month+(12*a)-3;

    JDN=RTC_DateTimeStruct->RTC_Date;
    JDN+=(153*m+2)/5;
    JDN+=365*y;
    JDN+=y/4;
    JDN+=-y/100;
    JDN+=y/400;
    JDN = JDN -32045;
    JDN = JDN - JULIAN_DATE_BASE;
    JDN*=86400;
    JDN+=(RTC_DateTimeStruct->RTC_Hours*3600);
    JDN+=(RTC_DateTimeStruct->RTC_Minutes*60);
    JDN+=(RTC_DateTimeStruct->RTC_Seconds);

    return JDN;
}


uint8_t Rtc::IncWrap(uint8_t value, int8_t increment, uint8_t base)
{
    int16_t newValue = value;
    newValue += increment;
    while (newValue < 0)
    {
        newValue += base;
    }
    while (newValue >= base)
    {
        newValue -= base;
    }
    return (uint8_t) newValue;
}
