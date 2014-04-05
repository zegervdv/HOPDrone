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
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void flash_error(void) {
  GPIOD->BSRRL = 0x1000;
  while(1){
    int nTick =  0x7FFFFF;
    for(; nTick != 0; nTick--);
    GPIOD->ODR ^= 0x1000;
  }
}
