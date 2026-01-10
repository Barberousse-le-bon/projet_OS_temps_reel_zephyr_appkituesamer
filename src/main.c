#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <stdio.h>

#include "ssd1306/ssd1306.h"
#include "mpu6050/mpu6050.h"
#include "bmp280/bmp280.h"
#include "utilities/utilities.h"

void bpm280_task(struct k_timer *timer_id);
void mpu6050_task(struct k_timer *timer_id);
void display_task(struct k_timer *timer_id);

K_TIMER_DEFINE(bpm280_timer, bpm280_task, NULL);
K_TIMER_DEFINE(mpu6050_timer, mpu6050_task, NULL);
K_TIMER_DEFINE(display_timer, display_task, NULL);

K_MSGQ_DEFINE(bpm280_queue, sizeof(int), 1, 1);
K_MSGQ_DEFINE(mpu6050_queue, sizeof(MPU6050), 1, 1);


void bpm280_task(struct k_timer *timer_id)
{
	int temp = bmp280_read_temp();
	k_msgq_put(&bpm280_queue, &temp, K_NO_WAIT);
}

void mpu6050_task(struct k_timer *timer_id)
{
	MPU6050 data = mpu6050_read_data();
	k_msgq_put(&mpu6050_queue, &data, K_NO_WAIT);
}

void display_task(struct k_timer *timer_id)
{
	int temp;
	MPU6050 mpu_data;

	k_msgq_get(&bpm280_queue, &temp, K_FOREVER);
	k_msgq_get(&mpu6050_queue, &mpu_data, K_FOREVER);

	display_data(temp, mpu_data);
}


int main(void)
{
	scan_device_i2c();
	
	mpu6050_init();
	init_OLED();

	k_timer_start(&bpm280_timer, K_MSEC(0), K_MSEC(100));
	k_timer_start(&mpu6050_timer, K_MSEC(0), K_MSEC(100));
	k_timer_start(&display_timer, K_MSEC(0), K_MSEC(1000));


	while(1)
	{
		k_sleep(K_MSEC(1000));
	}

	return 0;
}
