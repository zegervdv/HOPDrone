/**
 * accelerometer.c
 * Communicate with accelerometer
 */

#include "accelerometer.h"

/** Internal Functions **/

/* Source: https://github.com/Torrentula/STM32F4-examples/blob/master/I2C%20Master/main.c */

/**
 * Issue I2C start conditions
 */
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction) {
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

  I2C_GenerateSTART(I2Cx, ENABLE);

  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(I2Cx, address, direction);

  if(direction == I2C_Direction_Transmitter){
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  }else if(direction == I2C_Direction_Receiver) {
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  }
}

/**
 * Write one byte of data to slave
 */
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data) {
  I2C_SendData(I2Cx, data);
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

/**
 * Read one byte from slave and acknowledge
 * Asks slave to send next byte
 */
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx) {
  uint8_t data;
  I2C_AcknowledgeConfig(I2Cx, ENABLE);
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));
  data = I2C_ReceiveData(I2Cx);
  return data;
}

/**
 * Read one byte form slave and do not acknowledge
 * Stops transmission by slave
 */
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx) {
  I2C_AcknowledgeConfig(I2Cx, DISABLE);
  I2C_GenerateSTOP(I2Cx, ENABLE);

  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));
  uint8_t data = I2C_ReceiveData(I2Cx);
  return data;
}

/**
 * Issue stop condition
 */
void I2C_stop(I2C_TypeDef* I2Cx) {
  I2C_GenerateSTOP(I2Cx, ENABLE);
}


void I2C_init(void) {
  GPIO_InitTypeDef GPIO_init;
  I2C_InitTypeDef I2C_init;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_init.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_init.GPIO_Mode = GPIO_Mode_AF;
  GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_init.GPIO_OType = GPIO_OType_OD;
  GPIO_init.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_init);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

  I2C_init.I2C_ClockSpeed = 100000;
  I2C_init.I2C_Mode = I2C_Mode_I2C;
  I2C_init.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_init.I2C_OwnAddress1 = 0x00;
  I2C_init.I2C_Ack = I2C_Ack_Disable;
  I2C_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C1, &I2C_init);

  I2C_Cmd(I2C1, ENABLE);

}

void accelerometer_init(void) {
  I2C_init();
  // Write Control Data to register
  I2C_start(ACC_I2C_PORT, ACC_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(ACC_I2C_PORT, ACC_CTRLREG1_ADDR);
  I2C_write(ACC_I2C_PORT, ACC_CTRL_100HZ | ACC_CTRL_MODE_ACT | ACC_CTRL_FS_9 | ACC_CTRL_3D);
  I2C_stop(ACC_I2C_PORT);
}

int accelerometer_read(uint8_t* data) {
  // Read data from 3 axes in one request using auto increment
  I2C_start(ACC_I2C_PORT, ACC_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(ACC_I2C_PORT, ACC_AUTO_INC_ADDR);
  I2C_stop(ACC_I2C_PORT);
  
  I2C_start(ACC_I2C_PORT, ACC_I2C_ADDR, I2C_Direction_Receiver);
  
  // Store x - axis value
  data[0] = I2C_read_ack(ACC_I2C_PORT);
  // Empty register
  I2C_read_ack(ACC_I2C_PORT);
  // Store y - axis value
  data[1] = I2C_read_ack(ACC_I2C_PORT);
  // Empty register
  I2C_read_ack(ACC_I2C_PORT);
  // Store z - axis value
  // And send NACK to terminate transfer
  data[2] = I2C_read_nack(ACC_I2C_PORT);

  return 0;
}

