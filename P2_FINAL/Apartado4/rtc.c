#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "rtc.h"
#include "lcd.h"
#include "Board_LED.h"
#include "stm32f4xx_hal_rtc_ex.h"
#include "rl_net_lib.h"
#include <time.h>
#include <stdio.h>

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_rtc;                        // thread id

RTC_HandleTypeDef RtcHandle;

RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;
  
/* Buffers used for displaying Time and Date */
char aShowTime[50] = {0};
char aShowDate[50] = {0};

osThreadId_t tid_RTC;

void ThRTC (void *argument);                   // thread function
 
int Init_ThRTC (void) {
  
  tid_rtc = osThreadNew(ThRTC, NULL, NULL);
  if (tid_rtc == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void ThRTC (void *argument) {
  Configure_RTC();
  SNTP_init ();

  while (1) {
    SNTP_init ();
    RTC_CalendarShow();
  }
}

void Configure_RTC(void)
{
  HAL_RTC_MspInit(&RtcHandle);

  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  
  HAL_RTC_Init(&RtcHandle);

}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc){
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

 
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  __HAL_RCC_RTC_ENABLE();
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc){
  __HAL_RCC_RTC_DISABLE();

  HAL_PWR_DisableBkUpAccess();
  __HAL_RCC_PWR_CLK_DISABLE();
 
}

static void RTC_CalendarConfig(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t date, uint8_t month, uint16_t year)
{

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = year;
  sdatestructure.Month = month;
  sdatestructure.Date = date;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD);

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = hours;
  stimestructure.Minutes = minutes;
  stimestructure.Seconds = seconds;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD);

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
static void RTC_CalendarShow(void)
{

  lcd_borrar();
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf(aShowTime, "%2d:%2d:%2d", stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);
  LCD_write_line(aShowTime, 1);
  /* Display date Format : mm-dd-yy */
  sprintf(aShowDate, "%2d-%2d-%2d", sdatestructure.Month, sdatestructure.Date, 2000 + sdatestructure.Year);
  LCD_write_line(aShowDate, 2);
}

static void time_callback (uint32_t seconds, uint32_t seconds_fraction) {
  struct tm calendario;
  if (seconds != 0) {
   calendario = *localtime(&seconds);
   sdatestructure.Year = calendario.tm_year - 100;
   sdatestructure.Month = calendario.tm_mon + 1;
   sdatestructure.Date = calendario.tm_mday;
   sdatestructure.WeekDay = calendario.tm_wday;
  
   HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN);

   stimestructure.Hours = calendario.tm_hour + 1 ;
   stimestructure.Minutes = calendario.tm_min;
   stimestructure.Seconds = calendario.tm_sec;
   stimestructure.TimeFormat = RTC_HOURFORMAT_24;
   stimestructure.DayLightSaving = calendario.tm_isdst ;
   stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
   
	 HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
	
	   /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
		HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
  }
}

void SNTP_init (void){
  netSNTPc_GetTime(NULL,time_callback);
}