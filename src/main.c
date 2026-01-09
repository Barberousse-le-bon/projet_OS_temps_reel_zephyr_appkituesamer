#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <stdio.h>

#include "ssd1306/ssd1306.h"
#include "mpu6050/mpu6050.h"
#include "bmp280/bmp280.h"
#include "utilities/utilities.h"



int main(void)
{
	uint8_t reg = 0x75; 
	uint8_t data[3];
	scan_device_i2c();
	
	mpu6050_init();

	int temp = bmp280_read_temp();
	printk("BMP280 Temperature raw data: %d\n", temp);
	
	//Gyro gyro = mpu6050_read_gyro_all();
	//float x = mpu6050_read_gyro(MPU6050_GYRO_X);
	// printk("%d\n", (int)x);
	//printk("MPU6050 Gyro X,Y,Z: (%.2f, %.2f, %.2f)\n", gyro.x, gyro.y, gyro.z);

	// Print des float ne marchent pas ???

	
	MPU6050 mpu6050 = mpu6050_read_all();
	//printk("MPU6050: ACCEL (%d m/s, %d m/s, %d m/s)", (int) mpu6050.accel.x, (int) mpu6050.accel.y, (int) mpu6050.accel.z);


	//read_sensor(MPU6050_ADDR, reg, data, 1);
	//printk("MPU6050 WHO_AM_I register: 0x%02x\n", data[0]);
	
	init_OLED();
	display_data(temp, mpu6050);


	while(1){
		int temp = bmp280_read_temp();
		MPU6050 mpu6050 = mpu6050_read_all();
		display_data(temp, mpu6050);
	}

	return 0;
}
