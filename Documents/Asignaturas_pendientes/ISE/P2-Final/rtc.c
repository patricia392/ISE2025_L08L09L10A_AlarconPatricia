/**
  ******************************************************************************
  * @file    RTC/RTC_Calendar/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32F4xx RTC HAL API to configure
  *          Time and Date.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include "rtc.h"
#include "Board_LED.h"
#include "stm32f4xx_hal_rtc_ex.h"
#include "rl_net_lib.h"
#include <time.h>

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup RTC_Calendar
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;

RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;
  
/* Buffers used for displaying Time and Date */
uint8_t aShowTime[50] = {0};
uint8_t aShowDate[50] = {0};

osThreadId_t tid_RTC;

static osTimerId_t Alarma_1_min;
static void Timer1_Min_Alarma(void *arg);

static osTimerId_t Seg5_LED;
static void Timer5_Seg_LED(void *arg);

static osTimerId_t Syncronice_5Seg_Before;
static void Timer5_Seg(void *arg);
static osTimerId_t Syncronice_3_min;
static void Timer3_Min(void *arg);
static osTimerId_t Syncronice_2_Seg;
static void Timer2_Seg(void *arg);


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int Init_RTC (void) {

  tid_RTC = osThreadNew(ThRTC, NULL, NULL);
  
  Alarma_1_min = osTimerNew(Timer1_Min_Alarma, osTimerPeriodic, NULL, NULL);
  Seg5_LED = osTimerNew(Timer5_Seg_LED, osTimerOnce, NULL, NULL);

  Syncronice_3_min = osTimerNew(Timer3_Min, osTimerPeriodic, NULL, NULL);
  Syncronice_5Seg_Before = osTimerNew(Timer5_Seg, osTimerOnce, NULL, NULL);
  Syncronice_2_Seg = osTimerNew(Timer2_Seg, osTimerOnce, NULL, NULL);

  if (tid_RTC == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void ThRTC (void *argument) {
  Configure_RTC();
  Configure_Button();
  osTimerStart(Syncronice_5Seg_Before, 5000);
  osTimerStart(Alarma_1_min, 60000);
  while (1) {
    RTC_CalendarShow(aShowTime, aShowDate);
    if(){
      osTimerStart(Alarma_1_min, 500);
    }
  }
}

static void Timer1_Min_Alarma(void *arg){
  osTimerStart(Syncronice_3_min, 180000);
}

static void Timer5_Seg_LED(void *arg){
  LED_On(2);
}

static void Timer5_Seg(void *arg){
  osTimerStart(Syncronice_3_min, 180000);
}

static void Timer3_Min(void *arg){
	netSNTPc_GetTime (NULL, time_callback);
}

void Configure_RTC(void)
{
  HAL_RTC_MspInit(&RtcHandle);
  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */ 
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Turn on LED1 */
  //BSP_LED_On(LED1);

  /*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 1 Data */
  if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    //RTC_CalendarConfig();
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
    {
      /* Turn on LED2: Power on reset occurred */
      //BSP_LED_On(LED2);
    }
    /* Check if Pin Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {
      /* Turn on LED1: External reset occurred */
      //BSP_LED_On(LED1);
    }
    /* Clear source Reset Flag */
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }

}

/*      BOTON     */
void Configure_Button(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void EXTI15_10_IRQHandler(void){
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin==GPIO_PIN_13){
    RTC_CalendarConfig(0, 0, 0, 1, 1, 0);

  }
}

/*       ALARMA      */

void RTC_AlarmConfig(void)
{
  RTC_AlarmTypeDef RTC_AlarmStructure;

  RTC_AlarmStructure.AlarmTime.Hours =  stimestructure.Hours;
  RTC_AlarmStructure.AlarmTime.Minutes = stimestructure.Minutes;
  RTC_AlarmStructure.AlarmTime.Seconds = 0;
  RTC_AlarmStructure.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  RTC_AlarmStructure.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  RTC_AlarmStructure.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;
  RTC_AlarmStructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  RTC_AlarmStructure.AlarmDateWeekDay = 0x1;
  RTC_AlarmStructure.Alarm = RTC_ALARM_A;
  HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

  // Enable the alarm interrupt
  HAL_RTC_SetAlarm_IT(&RtcHandle, &RTC_AlarmStructure, RTC_FORMAT_BIN);
  
  // Unmask the RTC Alarm A interrupt
  CLEAR_BIT(RtcHandle.Instance->CR, RTC_CR_ALRAIE);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) { 
  RTC_AlarmTypeDef sAlarm;
  HAL_RTC_GetAlarm(hrtc,&sAlarm,RTC_ALARM_A,FORMAT_BIN);
	sAlarm.AlarmTime.Hours = stimestructure.Hours; 
	sAlarm.AlarmTime.Minutes = stimestructure.Minutes+1; 
	sAlarm.AlarmTime.Seconds = stimestructure.Seconds; 
  
  while(HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, FORMAT_BIN)!=HAL_OK){}
	osThreadFlagsSet (tid_ThAlarm, 0x01);
}

void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RtcHandle);
}

/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
static void RTC_CalendarConfig(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t date, uint8_t month, uint16_t year)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = year;
  sdatestructure.Month = month;
  sdatestructure.Date = date;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = hours;
  stimestructure.Minutes = minutes;
  stimestructure.Seconds = seconds;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
static void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char *)showtime, "%2d:%2d:%2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  LCD_write_line((char *)showtime, 1);
  /* Display date Format : mm-dd-yy */
  sprintf((char *)showdate, "%2d-%2d-%2d", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
  LCD_write_line((char *)showdate, 2);
}

static void calendar_callback (uint32_t seconds, uint32_t seconds_fraction) {
	struct tm t;
  if (seconds != 0) {
   t = *localtime(&seconds);
   sdatestructure.Year = t.tm_year - 100;
   sdatestructure.Month = t.tm_mon + 1;
   sdatestructure.Date = t.tm_mday;
   sdatestructure.WeekDay = t.tm_wday;
  
   HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN);

   stimestructure.Hours = t.tm_hour + 1 ;
   stimestructure.Minutes = t.tm_min;
   stimestructure.Seconds = t.tm_sec;
   stimestructure.TimeFormat = RTC_HOURFORMAT_24;
   stimestructure.DayLightSaving = t.tm_isdst ;
   stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
   
	 HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
	
	   /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
		HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
		osTimerStart(timsoft4,2000);
		osTimerStart(timsoft5,200);
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */
