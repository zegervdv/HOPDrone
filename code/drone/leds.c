/**
 * leds.c
 * Control LEDs on controller board
 */

#include "leds.h"

GPIO_TypeDef* led_bank[NRLEDS] = {LED1_BANK, LED2_BANK, LED3_BANK};
const int led_pin[NRLEDS] = {LED1_PIN, LED2_PIN, LED3_PIN};
const int led_clk[NRLEDS] = {LED1_CLK, LED2_CLK, LED3_CLK};

void LED_init(type_led led) {
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(led_clk[led], ENABLE);

  GPIO_InitStructure.GPIO_Pin = led_pin[led];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(led_bank[led], &GPIO_InitStructure);
}

void LED_on(type_led led) {
  led_bank[led]->BSRRL = led_pin[led];
}

void LED_off(type_led led) {
  led_bank[led]->BSRRH = led_pin[led];
}

void LED_toggle(type_led led) {
  led_bank[led]->ODR ^= led_pin[led];
}

void LED_blink(type_led led) {
  LED_on(led);
  while(1){
    int nTick =  0x7FFFFF;
    for(; nTick != 0; nTick--);
    LED_toggle(led);
  }
}
