#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void RTC_CalendarConfig(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t date, uint8_t month, uint16_t year);
static void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate);

static void calendar_callback (uint32_t seconds, uint32_t seconds_fraction);

void Configure_RTC(void);
void Configure_Button(void);

int Init_RTC (void);
void ThRTC (void *argument);

#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */ 
 
#endif /* __RTC_H */
