#ifndef __DWT_DELAY_H_
#define __DWT_DELAY_H_

#define    DWT_CYCCNT    *(volatile uint32_t *)0xE0001004
#define    DWT_CONTROL   *(volatile uint32_t *)0xE0001000
#define    SCB_DEMCR     *(volatile uint32_t *)0xE000EDFC

// F_CPU must be set for proper delays
#ifndef     F_CPU
    #define 	F_CPU		64000000UL
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void DWT_Init(void);
    uint32_t DWT_Get(void);
    uint32_t DWT_GetDelta(uint32_t time1, uint32_t time2);
    uint32_t DWT_GetDeltaForNow(uint32_t time_mark);
    uint32_t DWT_StartDelayUs(uint32_t us);
    uint8_t DWT_DelayInProgress(uint32_t time_mark);
    void DWT_DelayUs(uint32_t time);
    void DWT_DelayMs(uint32_t time);

#ifdef __cplusplus
}
#endif

#endif
