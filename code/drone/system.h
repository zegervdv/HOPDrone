/**
 * system.h
 * Define general constants
 * Initialize peripherals
 */

#include "leds.h"
#include "arm_math.h"
#include "rcm.h"
#include "lcm.h"
#include "accelerometer.h"

/**
 * Initialize all peripherals
 */
void init_system(void);

/**
 * Sleep for nTick cycles
 * nTick - number of cycles to sleep
 */
void Delay(__IO uint32_t nTick);

