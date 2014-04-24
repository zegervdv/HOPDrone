/**
 * system.h
 * Define general constants
 * Initialize peripherals
 */

#include "leds.h"
#include "rcm.h"

/**
 * Initialize all peripherals
 */
void init_system(void);

/**
 * Sleep for nTick cycles
 * nTick - number of cycles to sleep
 */
void Delay(__IO uint32_t nTick);

