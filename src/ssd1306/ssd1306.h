#include "../global_variables.h"
#include "font5x8.h"


#ifndef ssd1306
#define ssd1306

void ssd1306_cmd(uint8_t cmd);
void ssd1306_cmds(uint8_t *cmds, uint16_t size);
void ssd1306_init(void);
void ssd1306_clear(void);
void ssd1306_pixel(uint8_t x, uint8_t y, uint8_t color);
void ssd1306_update(void);
void ssd1306_clear_buffer(void);
void ssd1306_draw_char(uint8_t x, uint8_t y, char c);
void ssd1306_draw_string(uint8_t x, uint8_t y, const char *str);

#endif