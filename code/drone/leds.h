/**
 * leds.h
 * Control LEDs on the controller board
 */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/**
 * Initialize GPIO pins for LED operation
 */
void InitGPIO(void);

/**
 * Flash Error LED
 * Use to indicate an error occured
 * Causes infinite loop and will block further execution
 */
void flash_error(void);
