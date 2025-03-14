#ifndef __RTC_H
#define __RTC_H

#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"
#include <string.h>

int Init_ThRTC (void);

static void RTC_CalendarShow(void);
static void RTC_CalendarConfig(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t date, uint8_t month, uint16_t year);
static void time_callback (uint32_t seconds, uint32_t seconds_fraction);
void Configure_RTC(void);
void SNTP_init (void);

#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */ 

#endif /* __RTC_H */