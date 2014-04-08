/**
 * accelerometer.h
 * Communicate with accelerometer
 */

#include "stm32f4xx_i2c.h"

#define ACC_I2C_ADDRESS       0x1C
#define ACC_I2C_ADDRESS_WRITE ACC_I2C_ADDRESS<<0
#define ACC_I2C_ADDRESS_READ  ACC_I2C_ADDRESS<<1


/**
 * Initialize I2C communication for accelerometer
 * On PB6: SCL and PB7 SDA
 * Speed at 100 kHz
 */
void I2C_init(void);

/**
 * Initialize the accelerometer
 */
void accelerometer_init(void);

/**
 * Read all axis from the accelerometer
 * Returns an array of 3 values [x, y, z]
 */
int* accelerometer_read(void);
