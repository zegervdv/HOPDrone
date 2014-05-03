/**
  ******************************************************************************
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  */ 
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include <stdint.h>
#include "leds.h"

extern void rcmIfMakePacket(uint16_t rec_word);
static __IO uint32_t TimingDelay;

//delay function:

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

	LED_init(LED3);
	LED_on(LED3);
  while(TimingDelay != 0);
	LED_off(LED3);
}

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
	  LED_init(LED1);
	  LED_on(LED1);
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
	  LED_init(LED1);
	  LED_on(LED1);
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
	  LED_init(LED1);
	  LED_on(LED1);
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
	  LED_init(LED1);
	  LED_on(LED1);
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	if (TimingDelay != 0x00)
	  {
	    TimingDelay--;
	  }
}



/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

void  USART2_IRQHandler(void){
	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET){
		uint8_t word=USART_ReceiveData(USART2);
		//do something with received word.
		rcmIfMakePacket(word);

	}
}

void  USART3_IRQHandler(void){
	while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET){
		uint8_t word=USART_ReceiveData(USART3);
		//do something with received word.
		rcmIfMakePacket(word);

	}
}

/**
  * @brief  This function handles EXTI0_IRQ Handler.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
	/* Clear the EXTI line pending bit */
	EXTI_ClearITPendingBit(EXTI_Line0);

}




/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
