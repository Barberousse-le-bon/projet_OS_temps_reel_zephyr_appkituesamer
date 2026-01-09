/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/i2c.h>
#include <stdio.h>

#include "font5x8.h"

#define BMP280_ADDR 0x76
#define MPU6050_ADDR 0x68

#define BMP280_TEMP_REG_MSB 0xFA

#define MPU6050_GYRO_X   0x43 // MSB REG
#define MPU6050_GYRO_Y   0x45 // MSB REG
#define MPU6050_GYRO_Z   0x47 // MSB REG

#define MPU6050_ACCEL_X  0x3B // MSB REG
#define MPU6050_ACCEL_Y  0x3D // MSB REG
#define MPU6050_ACCEL_Z  0x3F // MSB REG


#define SSD1306_ADDR    0x3C
#define SSD1306_WIDTH   128
#define SSD1306_HEIGHT   64
#define SSD1306_COL_OFFSET 2

uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];


static const struct device *bus = DEVICE_DT_GET(DT_NODELABEL(i2c1));




typedef struct MPU6050
{
	struct Gyro // EN degrée
	{
		float x, y, z;
	};

	struct Accelerometer // En m/s
	{
		float x, y, z;
	};

	struct Gyro gyro;
	struct Accelerometer accel;
	int temp;

} MPU6050;

int scan_device_i2c(void)
{
	int i = 0x03;
	for(; i < 0xF0; i++) {
		uint8_t data;
		if(i2c_write(bus, NULL, 0, i) == 0) {
			printk("Device found at address 0x%02x\n", i);
		}
	}
	printk("Scan done !\n");
	return i;
}

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


int bmp280_read_temp(void)
{
	uint8_t data[3];
	read_sensor(BMP280_ADDR, BMP280_TEMP_REG_MSB, data, 3);
	return data[2] | (data[1] << 8) | (data[0] << 16);
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

MPU6050 mpu6050_read_all()
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

void ssd1306_cmd(uint8_t cmd)
{
    uint8_t buf[2];
    buf[0] = 0x00;   // Control byte : Command
    buf[1] = cmd;

    i2c_write(bus, buf, 2, SSD1306_ADDR);
}


void ssd1306_cmds(uint8_t *cmds, uint16_t size)
{
    uint8_t buf[size + 1];
    buf[0] = 0x00;
    memcpy(&buf[1], cmds, size);
	i2c_write(bus, buf, size+1, SSD1306_ADDR);

}


void ssd1306_init(void)
{
    HAL_Delay(100); // Laisser le temps à l'écran de démarrer

    uint8_t init_cmds[] = {
        0xAE,       // Display OFF

        0xD5, 0x80, // Set Display Clock Divide Ratio
        0xA8, 0x3F, // Multiplex Ratio (1/64)

        0xD3, 0x00, // Display Offset
        0x40,       // Start Line = 0

        0x8D, 0x14, // Charge Pump ON

        0x20, 0x00, // Memory Addressing Mode = Horizontal

        0xA1,       // Segment remap
        0xC8,       // COM Output Scan Direction remapped

        0xDA, 0x12, // COM Pins hardware configuration

        0x81, 0x7F, // Contrast

        0xD9, 0xF1, // Pre-charge period
        0xDB, 0x40, // VCOMH deselect level

        0xA4,       // Resume RAM content display
        0xA6,       // Normal display (not inverted)

        0xAF        // Display ON
    };

    ssd1306_cmds(init_cmds, sizeof(init_cmds));
}


void ssd1306_clear(void)
{
    uint8_t zero[128];
    memset(zero, 0x00, sizeof(zero));

    for (uint8_t page = 0; page < 8; page++)
    {
        ssd1306_cmd(0xB0 + page); // Page
		ssd1306_cmd(0x00 + (SSD1306_COL_OFFSET & 0x0F));
		ssd1306_cmd(0x10 + (SSD1306_COL_OFFSET >> 4));

        uint8_t buf[129];
        buf[0] = 0x40;
        memcpy(&buf[1], zero, 128);
		i2c_write(bus, buf, 129, SSD1306_ADDR);

    }
}
void ssd1306_pixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
        return;

    uint16_t index = x + (y / 8) * SSD1306_WIDTH;
    uint8_t bit = 1 << (y % 8);

    if (color)
        ssd1306_buffer[index] |= bit;   // pixel ON
    else
        ssd1306_buffer[index] &= ~bit;  // pixel OFF
}


void ssd1306_update(void)
{
    for (uint8_t page = 0; page < 8; page++)
    {
        ssd1306_cmd(0xB0 + page); // Page address
		ssd1306_cmd(0x00 + (SSD1306_COL_OFFSET & 0x0F));
		ssd1306_cmd(0x10 + (SSD1306_COL_OFFSET >> 4));


        uint8_t buf[129];
        buf[0] = 0x40; // Data
        memcpy(&buf[1], &ssd1306_buffer[page * 128], 128);
		i2c_write(bus, buf, 129, SSD1306_ADDR);

    }
}

void ssd1306_clear_buffer(void)
{
    memset(ssd1306_buffer, 0x00, sizeof(ssd1306_buffer));
}

void ssd1306_draw_char(uint8_t x, uint8_t y, char c)
{
    if (c < 32 || c > 126)
        c = '?';

    const uint8_t *glyph = font5x8[c - 32];

    for (uint8_t col = 0; col < 5; col++)
    {
        uint8_t line = glyph[col];

        for (uint8_t row = 0; row < 8; row++)
        {
            if (line & (1 << row))
                ssd1306_pixel(x + col, y + row, 1);
            else
                ssd1306_pixel(x + col, y + row, 0);
        }
    }

    // 1 colonne vide pour l'espacement
    for (uint8_t row = 0; row < 8; row++)
        ssd1306_pixel(x + 5, y + row, 0);
}

void ssd1306_draw_string(uint8_t x, uint8_t y, const char *str)
{
    while (*str)
    {
        ssd1306_draw_char(x, y, *str++);
        x += 6; // 5 pixels + 1 espace

        if (x + 5 >= SSD1306_WIDTH)
            break;
    }
}

void init_LCD(){

	ssd1306_init();
	ssd1306_clear();
	ssd1306_clear_buffer();
	ssd1306_draw_string(20, 30, "Booting");
	ssd1306_update();
	// afficher le texte qui ne changera pas 
	ssd1306_clear();
	ssd1306_clear_buffer();
	ssd1306_draw_string(5, 15, "temp  :");
	ssd1306_draw_string(5, 25, "accel : in m/s");
	ssd1306_draw_string(5, 45, "gyro  : in rad/s");
	ssd1306_draw_string(25, 2, "Real time data");
	ssd1306_update();

}


void display_temp(int temp)
{
    char str_temp[10];
    sprintf(str_temp, "%d~C", temp);

    ssd1306_draw_string(50, 15, str_temp);
    ssd1306_update();
}

void display_accel(int x, int y, int z){
	int int_x = x/100; 
	int int_y = y/100;
	int int_z = z/100;

	int dec_x = x - int_x*100;
	int dec_y = y - int_y*100;
	int dec_z = z - int_z*100;

	char str_x[22];
	char str_y[22];
	char str_z[22];

	sprintf(str_x, "%d.%d", int_x, dec_x);
	sprintf(str_y, "%d,%d", int_y, dec_y);
	sprintf(str_z, "%d,%d", int_z, dec_z);

	ssd1306_draw_string(5, 35, str_x);
	ssd1306_draw_string(45, 35, str_y);
	ssd1306_draw_string(85, 35, str_z);
    ssd1306_update();

}

void display_gyro(int x, int y, int z){
	int int_x = x/100; 
	int int_y = y/100;
	int int_z = z/100;

	int dec_x = x - int_x*100;
	int dec_y = y - int_y*100;
	int dec_z = z - int_z*100;

	char str_x[22];
	char str_y[22];
	char str_z[22];

	sprintf(str_x, "%d.%d", int_x, dec_x);
	sprintf(str_y, "%d,%d", int_y, dec_y);
	sprintf(str_z, "%d,%d", int_z, dec_z);

	ssd1306_draw_string(5, 55, str_x);
	ssd1306_draw_string(45, 55, str_y);
	ssd1306_draw_string(85, 55, str_z);
    ssd1306_update();

}


void display_dummy_data(){

	display_temp(25);
	display_accel(1233, 2344, 3455);
	display_gyro(4566, 5677,6788);

}
void display_data(int temp, struct MPU6050 mpu_data){

	display_temp(temp);
	display_accel((int)mpu_data.accel.x*100, (int)mpu_data.accel.y*100, (int)mpu_data.accel.z*100);
	display_gyro((int)mpu_data.gyro.x*100, (int)mpu_data.gyro.y*100, (int)mpu_data.gyro.z*100);
}




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
	printk("MPU6050: ACCEL (%d m/s, %d m/s, %d m/s)", (int) mpu6050.accel.x, (int) mpu6050.accel.y, (int) mpu6050.accel.z);


	//read_sensor(MPU6050_ADDR, reg, data, 1);
	//printk("MPU6050 WHO_AM_I register: 0x%02x\n", data[0]);
	
	init_LCD();
	display_data(temp, mpu6050);


	while(1){
		int temp = bmp280_read_temp();
		MPU6050 mpu6050 = mpu6050_read_all();
		display_data(temp, mpu6050);
	}

	return 0;
}
