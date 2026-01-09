#include "../global_variables.h"

#ifndef sensors
#define sensors

bool send_sensor(int sensor_addr, uint8_t reg_addr, uint8_t data);
int read_sensor(int sensor_addr, uint8_t reg_addr, uint8_t* data, int length);

#endif