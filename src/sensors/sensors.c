#include "sensors.h"


bool send_sensor(int sensor_addr, uint8_t reg_addr, uint8_t data)
{
	// Envoyer l'adresse du registre à lire
	uint8_t raw_data[2] = {reg_addr, data};
 	int ret = i2c_write(bus, raw_data, 2, sensor_addr);
	return ret == 0;
}

int read_sensor(int sensor_addr, uint8_t reg_addr, uint8_t* data, int length)
{
	// Envoyer l'adresse du registre à lire
	int ret = i2c_write(bus, &reg_addr, 1, sensor_addr);
	if (ret) {
		printk("I2C write failed: %d\n", ret);
		return ret;
	}

	// Lire les données du registre
	ret = i2c_read(bus, data, length, sensor_addr);
	if (ret) {
		printk("I2C read failed: %d\n", ret);
		return ret;
	} 

	return ret;
}