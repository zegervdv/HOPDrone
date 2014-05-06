/**
 * accelerometer.h
 * Communicate with accelerometer
 */

#include "stm32f4xx_i2c.h"

#define ACC_I2C_PORT        I2C1

// Adresses
#define ACC_I2C_ADDR_BASE   0x1C
#define ACC_I2C_ADDR        ACC_I2C_ADDR_BASE<<1

// Register Adresses
#define ACC_CTRLREG1_ADDR   0x20
#define ACC_XOUT_ADDR       0x29
#define ACC_YOUT_ADDR       0x2B
#define ACC_ZOUT_ADDR       0x2D

#define ACC_AUTO_INC_ADDR   ACC_XOUT_ADDR | 0x80

// Control Register 1 flags
#define ACC_CTRL_100HZ      0x00  // Set Rate to 100Hz
#define ACC_CTRL_400HZ      0x80  // Set Rate to 400Hz
#define ACC_CTRL_MODE_ACT   0x40  // Disable Power Down mode
#define ACC_CTRL_MODE_PD    0x00  // Enable Power Down mode
#define ACC_CTRL_FS_2       0x00  // Set Full Scale to 2g
#define ACC_CTRL_FS_9       0x20  // Set Full Scale to 9g
#define ACC_CTRL_ST_OFF     0x00  // Disable Self Test
#define ACC_CTRL_ST_ON      0x18  // Enable Self Test
#define ACC_CTRL_XAXIS      0x01  // Enable X-axis
#define ACC_CTRL_YAXIS      0x02  // Enable Y-axis
#define ACC_CTRL_ZAXIS      0x04  // Enable Z-axis
#define ACC_CTRL_3D         ACC_CTRL_XAXIS | ACC_CTRL_YAXIS | ACC_CTRL_ZAXIS // Enable all 3 axes

// Sensitivity
#define ACC_SENSITIVITY_2  18  // 18 mg/digit
#define ACC_SENSITIVITY_9  72  // 72 mg/digit


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
 * data - int array of size 3
 * Updates data with [x, y, z]
 */
int accelerometer_read(int32_t* data);
