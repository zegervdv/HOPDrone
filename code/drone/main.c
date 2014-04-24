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
#include "system.h"


int main(void) {
  
  init_system();

	// Infinite loop
	while(1)
	{
		Delay(0x7FFFFF);
	}

}


