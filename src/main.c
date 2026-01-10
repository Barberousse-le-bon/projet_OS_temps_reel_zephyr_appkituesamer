#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <stdio.h>

#include "ssd1306/ssd1306.h"
#include "mpu6050/mpu6050.h"
#include "bmp280/bmp280.h"
#include "utilities/utilities.h"

K_THREAD_STACK_DEFINE(sensor_wq_stack, 1024);

// Définition des tâches
void bpm280_task(struct k_timer *timer_id);
void mpu6050_task(struct k_timer *timer_id);

void bpm280_work(struct k_work *work);
void mpu6050_work(struct k_work *work);

// Définition des timers
K_TIMER_DEFINE(bpm280_timer, bpm280_task, NULL);
K_TIMER_DEFINE(mpu6050_timer, mpu6050_task, NULL);

// Définition des files de messages
K_MSGQ_DEFINE(bpm280_queue, sizeof(int), 1, 1);
K_MSGQ_DEFINE(mpu6050_queue, sizeof(MPU6050), 1, 1);

K_MUTEX_DEFINE(i2c_mutex);

// Définition des works
struct k_work bmp280_work_item;
struct k_work mpu6050_work_item;

struct k_work_q sensor_wq;

void bpm280_task(struct k_timer *timer_id)
{
	k_work_submit(&sensor_wq, &bmp280_work_item);
}

void mpu6050_task(struct k_timer *timer_id)
{
	k_work_submit(&sensor_wq, &mpu6050_work_item);
}

void bmp280_work(struct k_work *work)
{
	k_mutex_lock(&i2c_mutex, K_FOREVER);
	int temp = bmp280_read_temp();
	k_mutex_unlock(&i2c_mutex);
	k_msgq_put(&bpm280_queue, &temp, K_NO_WAIT);
}

void mpu6050_work(struct k_work *work)
{
	k_mutex_lock(&i2c_mutex, K_FOREVER);
	MPU6050 data = mpu6050_read_data();
	k_mutex_unlock(&i2c_mutex);
	k_msgq_put(&mpu6050_queue, &data, K_NO_WAIT);
}

void display_task()
{
	int temp = 0;
	MPU6050 mpu_data = {0};

	// Attend que des données soient disponibles
	k_msgq_get(&bpm280_queue, &temp, K_MSEC(100));
	k_msgq_get(&mpu6050_queue, &mpu_data, K_MSEC(100));

	k_mutex_lock(&i2c_mutex, K_FOREVER);
	display_data(temp, mpu_data);
	k_mutex_unlock(&i2c_mutex);
}


int main(void)
{
	scan_device_i2c();
	
	mpu6050_init();
	init_OLED();

	// Démarrer la file de work pour les capteurs
	k_work_queue_start(&sensor_wq, sensor_wq_stack,
		       K_THREAD_STACK_SIZEOF(sensor_wq_stack),
		       2, NULL);

	k_mutex_init(&i2c_mutex);
	
	// Initialiser les works
	k_work_init(&bmp280_work_item, bmp280_work);
	k_work_init(&mpu6050_work_item, mpu6050_work);

	// Démarrer les timers pour les tâches des capteurs
	k_timer_start(&bpm280_timer, K_MSEC(0), K_MSEC(100));
	k_timer_start(&mpu6050_timer, K_MSEC(0), K_MSEC(100));
	
	while(1)
	{
		display_task();
		k_sleep(K_MSEC(1000));
	}

	return 0;
}
