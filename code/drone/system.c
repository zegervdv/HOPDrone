/**
 * system.c
 * Define general constants
 * Initialize peripherals
 */

#include "system.h"

void init_system(void) {
  LED_init(LED1);
  LED_init(LED2);
  LED_init(LED3);
}
