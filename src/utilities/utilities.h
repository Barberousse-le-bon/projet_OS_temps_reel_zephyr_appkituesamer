#include "../global_variables.h"
#include "../mpu6050/mpu6050.h"
#include "../bmp280/bmp280.h"
#include "../ssd1306/ssd1306.h"

#ifndef utilities
#define utilities

int scan_device_i2c(void);
void init_OLED(void);
void display_temp(int temp);
void display_accel(int x, int y, int z);
void display_gyro(int x, int y, int z);
void display_dummy_data(void);
void display_data(int temp, struct MPU6050 mpu_data);

#endif