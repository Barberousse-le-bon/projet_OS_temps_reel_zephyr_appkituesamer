#ifndef Mpu6050
#define Mpu6050

#include "../global_variables.h"
#include "../sensors/sensors.h"

int mpu6050_init(void);
float mpu6050_read_gyro(uint8_t reg);
float mpu6050_read_accel(uint8_t reg);
MPU6050 mpu6050_read_all(void);

#endif