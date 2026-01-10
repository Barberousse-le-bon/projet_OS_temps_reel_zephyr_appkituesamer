#include "utilities.h"



// fonction pour chercher les capteurs i2c 
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


// apple des focntions pour initialiser l'écran avec le contenu fixe
void init_OLED(void)
{

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
	ssd1306_draw_string(5, 45, "gyro  : in rad");
	ssd1306_draw_string(25, 2, "Real time data");
	ssd1306_update();

}

// afficher la température
void display_temp(int temp)
{
    char str_temp[10];
    sprintf(str_temp, "%d~C", temp);

    ssd1306_draw_string(50, 15, str_temp);
    ssd1306_update();
}
// afficher l'accéleration
void display_accel(int x, int y, int z)
{
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
// afficher les données gyro
void display_gyro(int x, int y, int z)
{
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

// fonction de test pour tester l'affichage 
// NOTE : toutes les données sont en int parce que nous n'avons pas réussi à utiliser les float sans avoir des données incohérentes 
void display_dummy_data(void)
{

	display_temp(25);
	display_accel(1233, 2344, 3455);
	display_gyro(4566, 5677,6788);

}
// affichage des "vraies" données 
void display_data(int temp, struct MPU6050 mpu_data)
{

	display_temp(temp);
	display_accel((int)mpu_data.accel.x*100, (int)mpu_data.accel.y*100, (int)mpu_data.accel.z*100);
	display_gyro((int)mpu_data.gyro.x*100, (int)mpu_data.gyro.y*100, (int)mpu_data.gyro.z*100);
}


