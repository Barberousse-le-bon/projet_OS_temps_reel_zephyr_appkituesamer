#include "ssd1306.h"



uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// envoi d'une commande à l'écran
void ssd1306_cmd(uint8_t cmd)
{
    uint8_t buf[2];
    buf[0] = 0x00;   // Control byte : Command
    buf[1] = cmd;

    i2c_write(bus, buf, 2, SSD1306_ADDR);
}

// envoi de plusieurs commandes d'affilé à l'écran
void ssd1306_cmds(uint8_t *cmds, uint16_t size)
{
    uint8_t buf[size + 1];
    buf[0] = 0x00;
    memcpy(&buf[1], cmds, size);
	i2c_write(bus, buf, size+1, SSD1306_ADDR);

}

// initialisation de l'éran 
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

// éteindre tout les pixels de l'écran
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
// dessiner un pixel aux coordonnées x, y de la couleur color (0 noir, 1 blanc)
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

// une fois tout les piexels dessinés, on affiche 
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
//clear le buffer d'envoi à l'écran
void ssd1306_clear_buffer(void)
{
    memset(ssd1306_buffer, 0x00, sizeof(ssd1306_buffer));
}
//écrire un char grâce à la font 
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
// écrire plusieurs caractères 
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