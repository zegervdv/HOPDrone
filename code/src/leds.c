/**
 * leds.c
 * Control LEDs on controller board
 */

#include "leds.h"

void InitGPIO(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void flash_error(void) {
  while(1){
    int i = 10000000;
    GPIOD->BSRRL = 0x1000;
    while(i--);
    GPIOD->BSRRH = 0x1000;
    i = 10000000;
    while(i--);
  }
}
