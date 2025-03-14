#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "pwr.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_PWR;                        // thread id
 
RTC_HandleTypeDef RTCHandle;

void ThPWR (void *argument);                   // thread function
static osTimerId_t timSleepMode;
static void Timer_Sleep(void *arg);

int Init_ThPWR (void) {
 
  timSleepMode = osTimerNew((osTimerFunc_t)&Timer_Sleep, osTimerOnce, NULL, NULL);
  tid_PWR = osThreadNew(ThPWR, NULL, NULL);
  if (tid_PWR == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void ThPWR (void *argument) {
 
  Init_BotonAzul();
  
  __HAL_RCC_PWR_CLK_ENABLE();
  
  while (1) {
    if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET){
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    }
    ETH_PhyExitFromPowerDownMode();
    osTimerStart(timSleepMode, 15000);
    osThreadFlagsWait (OUT_SLEEP_MODE, osFlagsWaitAny, osWaitForever);
  }
}

static void Timer_Sleep(void *arg){
  SleepMode_Measure();
  ETH_PhyEnterPowerDownMode();
}

void Init_BotonAzul(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  //BOTON AZUL
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}
  
void EXTI15_10_IRQHandler(void){
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13); //Limpia la FlagStatus correspondiente a la interrupción generada a traves del pin 13
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN){
  if (GPIO_PIN_0 == GPIO_PIN_13){
    osThreadFlagsSet (tid_PWR, OUT_SLEEP_MODE);
  }
}

void HAL_SYSTICK_Callback(void)
{
  HAL_IncTick();
}

void SleepMode_Measure(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Disable USB Clock */
  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

  /* Disable Ethernet Clock */
  __HAL_RCC_ETH_CLK_DISABLE();

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_GPIOI_CLK_DISABLE();
  __HAL_RCC_GPIOJ_CLK_DISABLE();
  __HAL_RCC_GPIOK_CLK_DISABLE();

  /* Configure user Button */
  //BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
  //BOTON AZUL
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
    /* Configure Button pin as input with External interrupt */
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
    /* Enable and set Button EXTI Interrupt to the lowest priority */
  //HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0x0F, 0x00);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* Suspend Tick increment to prevent wakeup by Systick interrupt. 
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  /* Request to enter SLEEP mode */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  /* Resume Tick interrupt if disabled prior to sleep mode entry */
  HAL_ResumeTick();
  
  /* Exit Ethernet Phy from LowPower mode */
  ETH_PhyExitFromPowerDownMode();
}

void ETH_PhyEnterPowerDownMode(void)
{
  ETH_HandleTypeDef heth;
  GPIO_InitTypeDef GPIO_InitStruct;
  uint32_t phyregval = 0; 
   
  /* This part of code is used only when the ETH peripheral is disabled 
	   when the ETH is used in the application this initialization code 
	   is called in HAL_ETH_MspInit() function  ***********************/
	
	/* Enable GPIO clocks*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  /* Configure PA2: ETH_MDIO */
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL; 
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* Configure PC1: ETH_MDC */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* Enable the ETH peripheral clocks */
  __HAL_RCC_ETH_CLK_ENABLE();
  
  /* Set ETH Handle parameters */
  heth.Instance = ETH;
  heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;

  /* Configure MDC clock: the MDC Clock Range configuration
	   depends on the system clock: 180Mhz/102 = 1.76MHz  */
  /* MDC: a periodic clock that provides the timing reference for 
	   the MDIO data transfer which shouldn't exceed the maximum frequency of 2.5 MHz.*/
  heth.Instance->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div102;

  /*****************************************************************/
  
  /* ETH PHY configuration in Power Down mode **********************/ 
  
  /* Read ETH PHY control register */
  HAL_ETH_ReadPHYRegister(&heth, PHY_BCR, &phyregval);
  
  /* Set Power down mode bit */
  phyregval |= PHY_POWERDOWN;
  
  /* Write new value to ETH PHY control register */
  HAL_ETH_WritePHYRegister(&heth, PHY_BCR, phyregval);
  
  /*****************************************************************/
  
  /* Disable periph CLKs */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_ETH_CLK_DISABLE();
}

void ETH_PhyExitFromPowerDownMode(void)
{
   ETH_HandleTypeDef heth;
   GPIO_InitTypeDef GPIO_InitStruct;
   uint32_t phyregval = 0;
   
  /* ETH CLKs and GPIOs initialization ******************************/
  /* To be removed when the function is called from HAL_ETH_MspInit() when 
     exiting from Standby mode */
	
	/* Enable GPIO clocks*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  /* Configure PA2 */
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL; 
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* Configure PC1*/
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* Enable ETH CLK */
  __HAL_RCC_ETH_CLK_ENABLE();
  /*****************************************************************/
  
  /* ETH PHY configuration to exit Power Down mode *****************/
  /* Set ETH Handle parameters */
  heth.Instance = ETH;
  heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;
  
  /* Configure MDC clock: the MDC Clock Range configuration
	   depends on the system clock: 180Mhz/102 = 1.76MHz  */
  /* MDC: a periodic clock that provides the timing reference for 
	   the MDIO data transfer which shouldn't exceed the maximum frequency of 2.5 MHz.*/
  heth.Instance->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div102; 
	
  /* Read ETH PHY control register */
  HAL_ETH_ReadPHYRegister(&heth, PHY_BCR, &phyregval);
  
  /* check if the PHY  is already in power down mode */
  if ((phyregval & PHY_POWERDOWN) != RESET)
  {
    /* Disable Power down mode */
    phyregval &= ~ PHY_POWERDOWN;
    
    /* Write value to ETH PHY control register */
    HAL_ETH_WritePHYRegister(&heth, PHY_BCR, phyregval);
  }
  /*****************************************************************/
}

HAL_StatusTypeDef HAL_ETH_ReadPHYRegister(ETH_HandleTypeDef *heth, uint16_t PHYReg, uint32_t *RegValue)
{
  uint32_t tmpreg1 = 0U;
  uint32_t tickstart = 0U;

  /* Check parameters */
  assert_param(IS_ETH_PHY_ADDRESS(heth->Init.PhyAddress));

  /* Check the ETH peripheral state */
  if(heth->State == HAL_ETH_STATE_BUSY_RD)
  {
    return HAL_BUSY;
  }
  /* Set ETH HAL State to BUSY_RD */
  heth->State = HAL_ETH_STATE_BUSY_RD;

  /* Get the ETHERNET MACMIIAR value */
  tmpreg1 = heth->Instance->MACMIIAR;

  /* Keep only the CSR Clock Range CR[2:0] bits value */
  tmpreg1 &= ~ETH_MACMIIAR_CR_MASK;

  /* Prepare the MII address register value */
  tmpreg1 |=(((uint32_t)heth->Init.PhyAddress << 11U) & ETH_MACMIIAR_PA); /* Set the PHY device address   */
  tmpreg1 |=(((uint32_t)PHYReg<<6U) & ETH_MACMIIAR_MR);                   /* Set the PHY register address */
  tmpreg1 &= ~ETH_MACMIIAR_MW;                                            /* Set the read mode            */
  tmpreg1 |= ETH_MACMIIAR_MB;                                             /* Set the MII Busy bit         */

  /* Write the result value into the MII Address register */
  heth->Instance->MACMIIAR = tmpreg1;

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check for the Busy flag */
  while((tmpreg1 & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart ) > PHY_READ_TO)
    {
      heth->State= HAL_ETH_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(heth);

      return HAL_TIMEOUT;
    }

    tmpreg1 = heth->Instance->MACMIIAR;
  }

  /* Get MACMIIDR value */
  *RegValue = (uint16_t)(heth->Instance->MACMIIDR);

  /* Set ETH HAL State to READY */
  heth->State = HAL_ETH_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

HAL_StatusTypeDef HAL_ETH_WritePHYRegister(ETH_HandleTypeDef *heth, uint16_t PHYReg, uint32_t RegValue)
{
  uint32_t tmpreg1 = 0U;
  uint32_t tickstart = 0U;

  /* Check parameters */
  assert_param(IS_ETH_PHY_ADDRESS(heth->Init.PhyAddress));

  /* Check the ETH peripheral state */
  if(heth->State == HAL_ETH_STATE_BUSY_WR)
  {
    return HAL_BUSY;
  }
  /* Set ETH HAL State to BUSY_WR */
  heth->State = HAL_ETH_STATE_BUSY_WR;

  /* Get the ETHERNET MACMIIAR value */
  tmpreg1 = heth->Instance->MACMIIAR;

  /* Keep only the CSR Clock Range CR[2:0] bits value */
  tmpreg1 &= ~ETH_MACMIIAR_CR_MASK;

  /* Prepare the MII register address value */
  tmpreg1 |=(((uint32_t)heth->Init.PhyAddress<<11U) & ETH_MACMIIAR_PA); /* Set the PHY device address */
  tmpreg1 |=(((uint32_t)PHYReg<<6U) & ETH_MACMIIAR_MR);                 /* Set the PHY register address */
  tmpreg1 |= ETH_MACMIIAR_MW;                                           /* Set the write mode */
  tmpreg1 |= ETH_MACMIIAR_MB;                                           /* Set the MII Busy bit */

  /* Give the value to the MII data register */
  heth->Instance->MACMIIDR = (uint16_t)RegValue;

  /* Write the result value into the MII Address register */
  heth->Instance->MACMIIAR = tmpreg1;

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check for the Busy flag */
  while((tmpreg1 & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart ) > PHY_WRITE_TO)
    {
      heth->State= HAL_ETH_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(heth);

      return HAL_TIMEOUT;
    }

    tmpreg1 = heth->Instance->MACMIIAR;
  }

  /* Set ETH HAL State to READY */
  heth->State = HAL_ETH_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

void StopMode_Measure(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;  

  /* Disable USB Clock */
  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

  /* Disable Ethernet Clock */
  __HAL_RCC_ETH_CLK_DISABLE();

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_GPIOI_CLK_DISABLE();
  __HAL_RCC_GPIOJ_CLK_DISABLE();
  __HAL_RCC_GPIOK_CLK_DISABLE();

  /* Configure RTC prescaler and RTC data registers as follows:
  - Hour Format = Format 24
  - Asynch Prediv = Value according to source clock
  - Synch Prediv = Value according to source clock
  - OutPut = Output Disable
  - OutPutPolarity = High Polarity
  - OutPutType = Open Drain */ 
  RTCHandle.Instance = RTC;
  RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

  if(HAL_RTC_Init(&RTCHandle) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler(); 
  }

  /*## Configure the Wake up timer ###########################################*/
  /*  RTC Wakeup Interrupt Generation:
      Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI))
      Wakeup Time = Wakeup Time Base * WakeUpCounter 
                  = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI)) * WakeUpCounter
      ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

      To configure the wake up timer to 20s the WakeUpCounter is set to 0xA017:
        RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
        Wakeup Time Base = 16 /(~32.768KHz) = ~0,488 ms
        Wakeup Time = ~20s = 0,488ms  * WakeUpCounter
        ==> WakeUpCounter = ~20s/0,488ms = 40983 = 0xA017 */

  /* Disable Wake-up timer */
  HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);

  /* Enable Wake-up timer */
  HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0xA017, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

  /* FLASH Deep Power Down Mode enabled */
  HAL_PWREx_EnableFlashPowerDown();

  /*## Enter Stop Mode #######################################################*/
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  /* Configures system clock after wake-up from STOP: enable HSI, PLL and select 
     PLL as system clock source (HSI and PLL are disabled in STOP mode) */
  SYSCLKConfig_STOP();

  /* Exit Ethernet Phy from low power mode */
  ETH_PhyExitFromPowerDownMode();

  /* Disable Wake-up timer */
  if(HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler(); 
  }
}

/**
  * @brief  This function configures the system to enter Stop mode with RTC 
  *         clocked by LSE or LSI for current consumption measurement purpose.
  *         STOP Mode with RTC clocked by LSE/LSI
  *         =====================================   
  *           - RTC Clocked by LSE or LSI
  *           - Regulator in LP mode
  *           - Under drive feature enabled
  *           - HSI, HSE OFF and LSI OFF if not used as RTC Clock source
  *           - No IWDG
  *           - FLASH in deep power down mode
  *           - Automatic Wakeup using RTC clocked by LSE/LSI (~20s)
  * @param  None
  * @retval None
  */
void StopUnderDriveMode_Measure(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;  

  /* Disable USB Clock */
  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

  /* Disable Ethernet Clock */
  __HAL_RCC_ETH_CLK_DISABLE();

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_GPIOI_CLK_DISABLE();
  __HAL_RCC_GPIOJ_CLK_DISABLE();
  __HAL_RCC_GPIOK_CLK_DISABLE();

  /* Configure RTC prescaler and RTC data registers as follows:
  - Hour Format = Format 24
  - Asynch Prediv = Value according to source clock
  - Synch Prediv = Value according to source clock
  - OutPut = Output Disable
  - OutPutPolarity = High Polarity
  - OutPutType = Open Drain */ 
  RTCHandle.Instance = RTC;
  RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

  if(HAL_RTC_Init(&RTCHandle) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler(); 
  }

  /*## Configure the Wake up timer ###########################################*/
  /*  RTC Wakeup Interrupt Generation:
      Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI))
      Wakeup Time = Wakeup Time Base * WakeUpCounter 
                  = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI)) * WakeUpCounter
      ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

      To configure the wake up timer to 20s the WakeUpCounter is set to 0xA017:
        RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
        Wakeup Time Base = 16 /(~32.768KHz) = ~0,488 ms
        Wakeup Time = ~20s = 0,488ms  * WakeUpCounter
        ==> WakeUpCounter = ~20s/0,488ms = 40983 = 0xA017 */

  /* Disable Wake-up timer */
  HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);

  /* Enable Wake-up timer */
  HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0xA017, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

  /* FLASH Deep Power Down Mode enabled */
  HAL_PWREx_EnableFlashPowerDown();

  /*## Enter under-drive Stop Mode ###########################################*/
  HAL_PWREx_EnterUnderDriveSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  /* Configures system clock after wake-up from STOP: enable HSI, PLL and select 
     PLL as system clock source (HSI and PLL are disabled in STOP mode) */
  SYSCLKConfig_STOP();

  /* Exit Ethernet Phy from low power mode */
  ETH_PhyExitFromPowerDownMode();

  /* Disable Wake-up timer */
  if(HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle) != HAL_OK)
  {
    /* Initialization Error */
//    Error_Handler(); 
  }
}

/**
  * @brief  This function configures the system to enter Standby mode for
  *         current consumption measurement purpose.
  *         STANDBY Mode
  *         ============
  *           - Backup SRAM and RTC OFF
  *           - IWDG and LSI OFF
  *           - Wakeup using WakeUp Pin (PA.00)
  * @param  None
  * @retval None
  */
void StandbyMode_Measure(void)
{
  /* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* Allow access to Backup */
  HAL_PWR_EnableBkUpAccess();

  /* Reset RTC Domain */
  __HAL_RCC_BACKUPRESET_FORCE();
  __HAL_RCC_BACKUPRESET_RELEASE();
  
  /* Disable all used wakeup sources: Pin1(PA.0) */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  
  /* Clear all related wakeup flags */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  
  /* Re-enable all used wakeup sources: Pin1(PA.0) */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

  /*## Enter Standby Mode ####################################################*/
  /* Request to enter STANDBY mode  */
  HAL_PWR_EnterSTANDBYMode();  
}

/**
  * @brief  This function configures the system to enter Standby mode with RTC 
  *         clocked by LSE or LSI for current consumption measurement purpose.
  *         STANDBY Mode with RTC clocked by LSE/LSI
  *         ========================================
  *           - RTC Clocked by LSE/LSI
  *           - IWDG OFF
  *           - Backup SRAM OFF
  *           - Automatic Wakeup using RTC clocked by LSE/LSI (after ~20s)
  * @param  None
  * @retval None
  */
void StandbyRTCMode_Measure(void)
{
  /*## Configure the RTC peripheral###########################################*/

  /* Configure RTC prescaler and RTC data registers as follows:
  - Hour Format = Format 24
  - Asynch Prediv = Value according to source clock
  - Synch Prediv = Value according to source clock
  - OutPut = Output Disable
  - OutPutPolarity = High Polarity
  - OutPutType = Open Drain */ 
  RTCHandle.Instance = RTC;  
  RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  
  if(HAL_RTC_Init(&RTCHandle) != HAL_OK)
  {
    /* Initialization Error */
//    Error_Handler(); 
  }
  
  /*## Configure the Wake up timer ###########################################*/
  /*  RTC Wakeup Interrupt Generation:
      Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI))
      Wakeup Time = Wakeup Time Base * WakeUpCounter 
                  = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI)) * WakeUpCounter
      ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

      To configure the wake up timer to 20s the WakeUpCounter is set to 0xA017:
        RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
        Wakeup Time Base = 16 /(~32.768KHz) = ~0,488 ms
        Wakeup Time = ~20s = 0,488ms  * WakeUpCounter
        ==> WakeUpCounter = ~20s/0,488ms = 40983 = 0xA017 */
  /* Disable Wake-up timer */
  HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);
  
  /*## Clear all related wakeup flags ########################################*/
  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  
  /* Clear RTC Wake Up timer Flag */
  __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTCHandle, RTC_FLAG_WUTF);
  
  /*## Setting the Wake up time ##############################################*/
  HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0xA017, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
  
  /*## Enter Standby Mode ####################################################*/
  /* Request to enter STANDBY mode  */
  HAL_PWR_EnterSTANDBYMode();
}

/**
  * @brief  This function configures the system to enter Standby mode with RTC 
  *         clocked by LSE or LSI and with Backup SRAM ON for current consumption 
  *         measurement purpose.
  *         STANDBY Mode with RTC clocked by LSE/LSI and BKPSRAM
  *         ====================================================
  *           - RTC Clocked by LSE or LSI
  *           - Backup SRAM ON
  *           - IWDG OFF
  *           - Automatic Wakeup using RTC clocked by LSE/LSI (after ~20s)
  * @param  None
  * @retval None
  */
void StandbyRTCBKPSRAMMode_Measure(void)
{   
  /*## Configure the RTC peripheral###########################################*/
  /* Configure RTC prescaler and RTC data registers as follows:
  - Hour Format = Format 24
  - Asynch Prediv = Value according to source clock
  - Synch Prediv = Value according to source clock
  - OutPut = Output Disable
  - OutPutPolarity = High Polarity
  - OutPutType = Open Drain */ 
  RTCHandle.Instance = RTC;
  RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  
  if(HAL_RTC_Init(&RTCHandle) != HAL_OK)
  {
    /* Initialization Error */
//    Error_Handler(); 
  }
  
  /*## Configure the Wake up timer ###########################################*/
  /*  RTC Wakeup Interrupt Generation:
      Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI))
      Wakeup Time = Wakeup Time Base * WakeUpCounter 
                  = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI)) * WakeUpCounter
      ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

      To configure the wake up timer to 20s the WakeUpCounter is set to 0xA017:
        RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
        Wakeup Time Base = 16 /(~32.768KHz) = ~0,488 ms
        Wakeup Time = ~20s = 0,488ms  * WakeUpCounter
        ==> WakeUpCounter = ~20s/0,488ms = 40983 = 0xA017 */
   
  /* Disable Wake-up timer */
  HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);
  
  /*## Clear all related wakeup flags ########################################*/
  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  
  /* Clear RTC Wake Up timer Flag */
  __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTCHandle, RTC_FLAG_WUTF);
  
  /*## Setting the Wake up time ##############################################*/
  HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0xA017, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

  /* Enable BKPRAM Clock */
  __HAL_RCC_BKPSRAM_CLK_ENABLE();
  
  /* Enable the Backup SRAM low power Regulator */
  HAL_PWREx_EnableBkUpReg();

  /*## Enter Standby Mode ####################################################*/
  /* Request to enter STANDBY mode  */
  HAL_PWR_EnterSTANDBYMode();
}
    
/**
  * @brief  Configures system clock after wake-up from STOP: enable HSI, PLL
  *         and select PLL as system clock source.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  uint32_t pFLatency = 0;
  
  /* Get the Oscillators configuration according to the internal RCC registers */
  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
  
  /* After wake-up from STOP reconfigure the system clock: Enable HSE and PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
//    Error_Handler();
  }

  /* Get the Clocks configuration according to the internal RCC registers */
  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &pFLatency);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFLatency) != HAL_OK)
  {
//    Error_Handler();
  }
}
