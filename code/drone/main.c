/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
*****************************************************************************
*/

/* Includes */
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include <sys/errno.h>
#include "system.h"
#include "kalman.h"



void Delay(__IO uint32_t nTick);

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/

int main(void)
{
  position_t position;
  position_t sigmapoints[NR_SIGMAPOINTS];

  init_system();


  LED_on(LED2);
  LED_on(LED3);
  LED_blink(LED1);

	// Infinite loop
	while(1)
	{
		Delay(0x7FFFFF);
	}

}

void Delay(__IO uint32_t nTick)
{
  for(; nTick != 0; nTick--);
}

