/**
  ******************************************************************************
  * @brief   This file contains the headers of the interrupt handlers.
  * 			this file also holds the declaration of the delay function in ms.
  ******************************************************************************

  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

void Delay(__IO uint32_t nTick);


void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);


//Usart IRQ handler added
void  USART2_IRQHandler(void);
void  USART3_IRQHandler(void);
//user button IRQ handler
void EXTI0_IRQHandler(void);

#ifdef __cplusplus
}
#endif


#endif /* __STM32F4xx_IT_H */
