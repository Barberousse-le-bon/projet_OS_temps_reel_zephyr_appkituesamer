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
	// première lecture d'initialisation
	int temp = bmp280_read_temp();
	MPU6050 mpu6050 = mpu6050_read_all();

	
	init_OLED();
	display_data(temp, mpu6050);

	// opérations séquentielles
	while(1){
		int temp = bmp280_read_temp();
		MPU6050 mpu6050 = mpu6050_read_all();
		display_data(temp, mpu6050);
	}

	return 0;
}
