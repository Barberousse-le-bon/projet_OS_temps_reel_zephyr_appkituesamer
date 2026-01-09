#include "mpu6050.h"


int mpu6050_init(void)
{
	//réveiller le mpu6050
	bool init_success = send_sensor(MPU6050_ADDR, 0x6b, 0x00);
	send_sensor(MPU6050_ADDR, 0x6c, 0x00);
	send_sensor(MPU6050_ADDR, 0x1a, 0x00);
	send_sensor(MPU6050_ADDR, 0x1b, 0x00);
	send_sensor(MPU6050_ADDR, 0x1c, 0x00);
	printk("mpu setup correctly\n ");
	return init_success;
}



// Renvoie un angle entre 0 et 360
float mpu6050_read_gyro(uint8_t reg)
{
	uint8_t data[2];
	read_sensor(MPU6050_ADDR, reg, data, 2);
	uint16_t raw_angle = data[1] | (data[0] << 8);
	return 360.f * (float) (raw_angle) / 65535.f;
}

// Renvoie des m/s
float mpu6050_read_accel(uint8_t reg)
{
	uint8_t data[2];
	read_sensor(MPU6050_ADDR, reg, data, 2);
	uint16_t raw_angle = data[1] | (data[0] << 8);
	return raw_angle * 18.6 / 65535;
}

MPU6050 mpu6050_read_all(void)
{
	MPU6050 mpu6050;
	mpu6050.gyro.x = mpu6050_read_gyro(MPU6050_GYRO_X);
	mpu6050.gyro.y = mpu6050_read_gyro(MPU6050_GYRO_Y);
	mpu6050.gyro.z = mpu6050_read_gyro(MPU6050_GYRO_Z);

	mpu6050.accel.x = mpu6050_read_accel(MPU6050_ACCEL_X);
	mpu6050.accel.y = mpu6050_read_accel(MPU6050_ACCEL_Y);
	mpu6050.accel.z = mpu6050_read_accel(MPU6050_ACCEL_Z);

	return mpu6050;
}

