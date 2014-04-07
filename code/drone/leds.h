/**
 * leds.h
 * Control LEDs on the controller board
 */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

typedef enum {
  LED1 = 0,
  LED2 = 1,
  LED3 = 2
} type_led;

#define NRLEDS 3

#define LED1_PIN    GPIO_Pin_12
#define LED1_BANK   GPIOD
#define LED1_CLK    RCC_AHB1Periph_GPIOD

#define LED2_PIN    GPIO_Pin_14
#define LED2_BANK   GPIOE
#define LED2_CLK    RCC_AHB1Periph_GPIOE

#define LED3_PIN    GPIO_Pin_14
#define LED3_BANK   GPIOD
#define LED3_CLK    RCC_AHB1Periph_GPIOD

/**
 * Initialize GPIO pins for LED operation
 * led - type_led identifying the LED to be initialized
 */
void LED_init(type_led led);

/**
 * Switch LED on
 * led - led identifier
 */
void LED_on(type_led led);

/**
 * Switch LED off
 * led - led identifier
 */
void LED_off(type_led led);

/**
 * Toggle LED
 * led - led identifier
 */
void LED_toggle(type_led led);

/**
 * Flash Error LED1
 * Use to indicate an error occured
 * Causes infinite loop and will block further execution
 */
void flash_error(void);
