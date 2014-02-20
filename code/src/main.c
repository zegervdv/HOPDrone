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
#include "stm32f4_discovery.h"

/* #define USE_STDPERIPH_DRIVER */

/* Private variables */
/* Private macro */
/* Private function prototypes */
/* Private functions */
void RNG_Config(void);
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
	RCC_ClocksTypeDef RCC_Clocks;
	uint32_t random32bit;

	// Get clock frequencies, check in debug if these are set properly
	RCC_GetClocksFreq(&RCC_Clocks);


	// Initialize the LEDs and the user button on the Discovery board
	STM32F4_Discovery_LEDInit(LED3);
	STM32F4_Discovery_LEDInit(LED4);
	STM32F4_Discovery_LEDInit(LED5);
	STM32F4_Discovery_LEDInit(LED6);
	STM32F4_Discovery_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);


	// Orange LED on
	STM32F4_Discovery_LEDOn(LED3);

	// Configure the Random Number Generator, this is described in stm32f4xx_rng.c
	RNG_Config();

	// Infinite loop
	while(1)
	{

		// we use the RNG by polling: we check if a random number is ready before we fetch it.
		// An alternative is to use an interrupt.
		while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET)
		{
		}

		// Get a 32bit Random number
		random32bit = RNG_GetRandomNumber();

		// Select the 4 lowest bits in the random number and shift these by 12 bits to the left
		// The LEDs are connected to GPIO pins 12-15 so the following statement will light some LEDS
		GPIO_Write(GPIOD, (random32bit&0xF)<<12) ;

		// Insert delay: the delay is a number of cycles, it does not represent a time
		Delay(0x7FFFFF);

	}

}

void Delay(__IO uint32_t nTick)
{
  for(; nTick != 0; nTick--);
}

void RNG_Config(void)
{
	// Enable RNG clock source, note that the RNG uses the AHB2: High speed Advanced Peripheral Bus (APB2)
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

	// RNG Peripheral enable
	RNG_Cmd(ENABLE);
}

