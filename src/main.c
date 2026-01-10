#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <stdio.h>

#include "ssd1306/ssd1306.h"
#include "mpu6050/mpu6050.h"
#include "bmp280/bmp280.h"
#include "utilities/utilities.h"



int main(void)
{
	//initialisation des variables et quelques lectures de test
	uint8_t reg = 0x75; 
	uint8_t data[3];
	scan_device_i2c();
	
	mpu6050_init();

	int temp = bmp280_read_temp();
	printk("BMP280 Temperature raw data: %d\n", temp);

	MPU6050 mpu6050 = mpu6050_read_all();

	
	init_OLED();
	display_data(temp, mpu6050);

	// fonction while qui lit les capteurs et les affiche en boucle
	while(1){
		int temp = bmp280_read_temp();
		MPU6050 mpu6050 = mpu6050_read_all();
		display_data(temp, mpu6050);
	}

	return 0;
}
