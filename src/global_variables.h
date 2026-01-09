#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include "zephyr/drivers/i2c.h"


#define BMP280_ADDR 0x76
#define MPU6050_ADDR 0x68

#define BMP280_TEMP_REG_MSB 0xFA

#define MPU6050_GYRO_X   0x43 // MSB REG
#define MPU6050_GYRO_Y   0x45 // MSB REG
#define MPU6050_GYRO_Z   0x47 // MSB REG

#define MPU6050_ACCEL_X  0x3B // MSB REG
#define MPU6050_ACCEL_Y  0x3D // MSB REG
#define MPU6050_ACCEL_Z  0x3F // MSB REG


#define SSD1306_ADDR    0x3C
#define SSD1306_WIDTH   128
#define SSD1306_HEIGHT   64
#define SSD1306_COL_OFFSET 2

static const struct device *bus = DEVICE_DT_GET(DT_NODELABEL(i2c1));

typedef struct Accelerometer // En m/s
{
	float x, y, z;
} Accelerometer;

typedef struct Gyro // E degrée
{
	float x, y, z;
} Gyro;

typedef struct MPU6050
{

	Gyro gyro;
	Accelerometer accel;
	int temp;

} MPU6050;

#endif