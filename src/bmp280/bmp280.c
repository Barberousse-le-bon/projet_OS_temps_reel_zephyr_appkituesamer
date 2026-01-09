#include "bmp280.h"


int bmp280_read_temp(void)
{
	uint8_t data[3];
	read_sensor(BMP280_ADDR, BMP280_TEMP_REG_MSB, data, 3);
	return data[2] | (data[1] << 8) | (data[0] << 16);
}
