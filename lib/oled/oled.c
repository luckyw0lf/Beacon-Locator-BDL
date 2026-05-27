#include "oled.h"
#include "lpi2c.h"
#include <stdint.h>

#define OLED_I2C_ADDRESS 0x3C
#define OLED_WIDTH       128
#define OLED_HEIGHT      64
#define OLED_PAGES       8

static void oled_command(uint8_t command)
{
    // SSD1306 command control byte = 0x00
    lpi2c_write(OLED_I2C_ADDRESS, 0x00, &command, 1);

    while(lpi2c_busy())
    {
    }
}

static void oled_data(uint8_t data)
{
    // SSD1306 data control byte = 0x40
    lpi2c_write(OLED_I2C_ADDRESS, 0x40, &data, 1);

    while(lpi2c_busy())
    {
    }
}
static void oled_write_bytes(const uint8_t *data, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++)
    {
        oled_data(data[i]);
    }
}
static void oled_set_position(uint8_t page, uint8_t column)
{
    oled_command(0xB0 | page);                      // Set page address
    oled_command(0x00 | (column & 0x0F));           // Lower column address
    oled_command(0x10 | ((column >> 4) & 0x0F));    // Higher column address
}
void oled_set_cursor(uint8_t column, uint8_t page)
{
    if(page >= OLED_PAGES)
    {
        page = OLED_PAGES - 1;
    }

    if(column >= OLED_WIDTH)
    {
        column = OLED_WIDTH - 1;
    }

    oled_set_position(page, column);
}

void oled_init(void)
{
    lpi2c_controller_init();

    // Basic SSD1306 128x64 init sequence
    oled_command(0xAE); // Display OFF

    oled_command(0xD5); // Set display clock divide ratio
    oled_command(0x80);

    oled_command(0xA8); // Set multiplex ratio
    oled_command(0x3F);

    oled_command(0xD3); // Set display offset
    oled_command(0x00);

    oled_command(0x40); // Set display start line

    oled_command(0x8D); // Charge pump
    oled_command(0x14);

    oled_command(0x20); // Memory addressing mode
    oled_command(0x00); // Horizontal addressing mode

    oled_command(0xA1); // Segment remap
    oled_command(0xC8); // COM output scan direction

    oled_command(0xDA); // COM pins hardware configuration
    oled_command(0x12);

    oled_command(0x81); // Contrast
    oled_command(0x7F);

    oled_command(0xD9); // Pre-charge period
    oled_command(0xF1);

    oled_command(0xDB); // VCOMH deselect level
    oled_command(0x40);

    oled_command(0xA4); // Resume display from RAM
    oled_command(0xA6); // Normal display
    oled_command(0xAF); // Display ON

    oled_clear();
}

void oled_clear(void)
{
    for(uint8_t page = 0; page < OLED_PAGES; page++)
    {
        oled_set_position(page, 0);

        for(uint8_t col = 0; col < OLED_WIDTH; col++)
        {
            oled_data(0x00);
        }
    }
}

void oled_test_pattern(void)
{
    oled_clear();

    // Draw simple horizontal filled bars on alternating pages
    for(uint8_t page = 0; page < OLED_PAGES; page++)
    {
        oled_set_position(page, 0);

        for(uint8_t col = 0; col < OLED_WIDTH; col++)
        {
            if(page % 2 == 0)
            {
                oled_data(0xFF);
            }
            else
            {
                oled_data(0x00);
            }
        }
    }
}

static const uint8_t FONT_SPACE[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

static const uint8_t FONT_A[5] = {0x7E, 0x11, 0x11, 0x11, 0x7E};
static const uint8_t FONT_B[5] = {0x7F, 0x49, 0x49, 0x49, 0x36};
static const uint8_t FONT_C[5] = {0x3E, 0x41, 0x41, 0x41, 0x22};
static const uint8_t FONT_D[5] = {0x7F, 0x41, 0x41, 0x22, 0x1C};
static const uint8_t FONT_E[5] = {0x7F, 0x49, 0x49, 0x49, 0x41};
static const uint8_t FONT_F[5] = {0x7F, 0x09, 0x09, 0x09, 0x01};
static const uint8_t FONT_G[5] = {0x3E, 0x41, 0x49, 0x49, 0x7A};
static const uint8_t FONT_H[5] = {0x7F, 0x08, 0x08, 0x08, 0x7F};
static const uint8_t FONT_I[5] = {0x00, 0x41, 0x7F, 0x41, 0x00};
static const uint8_t FONT_J[5] = {0x20, 0x40, 0x41, 0x3F, 0x01};
static const uint8_t FONT_K[5] = {0x7F, 0x08, 0x14, 0x22, 0x41};
static const uint8_t FONT_L[5] = {0x7F, 0x40, 0x40, 0x40, 0x40};
static const uint8_t FONT_M[5] = {0x7F, 0x02, 0x04, 0x02, 0x7F};
static const uint8_t FONT_N[5] = {0x7F, 0x04, 0x08, 0x10, 0x7F};
static const uint8_t FONT_O[5] = {0x3E, 0x41, 0x41, 0x41, 0x3E};
static const uint8_t FONT_P[5] = {0x7F, 0x09, 0x09, 0x09, 0x06};
static const uint8_t FONT_Q[5] = {0x3E, 0x41, 0x51, 0x21, 0x5E};
static const uint8_t FONT_R[5] = {0x7F, 0x09, 0x19, 0x29, 0x46};
static const uint8_t FONT_S[5] = {0x46, 0x49, 0x49, 0x49, 0x31};
static const uint8_t FONT_T[5] = {0x01, 0x01, 0x7F, 0x01, 0x01};
static const uint8_t FONT_U[5] = {0x3F, 0x40, 0x40, 0x40, 0x3F};
static const uint8_t FONT_V[5] = {0x1F, 0x20, 0x40, 0x20, 0x1F};
static const uint8_t FONT_W[5] = {0x7F, 0x20, 0x18, 0x20, 0x7F};
static const uint8_t FONT_X[5] = {0x63, 0x14, 0x08, 0x14, 0x63};
static const uint8_t FONT_Y[5] = {0x07, 0x08, 0x70, 0x08, 0x07};
static const uint8_t FONT_Z[5] = {0x61, 0x51, 0x49, 0x45, 0x43};

static const uint8_t FONT_0[5] = {0x3E, 0x51, 0x49, 0x45, 0x3E};
static const uint8_t FONT_1[5] = {0x00, 0x42, 0x7F, 0x40, 0x00};
static const uint8_t FONT_2[5] = {0x42, 0x61, 0x51, 0x49, 0x46};
static const uint8_t FONT_3[5] = {0x21, 0x41, 0x45, 0x4B, 0x31};
static const uint8_t FONT_4[5] = {0x18, 0x14, 0x12, 0x7F, 0x10};
static const uint8_t FONT_5[5] = {0x27, 0x45, 0x45, 0x45, 0x39};
static const uint8_t FONT_6[5] = {0x3C, 0x4A, 0x49, 0x49, 0x30};
static const uint8_t FONT_7[5] = {0x01, 0x71, 0x09, 0x05, 0x03};
static const uint8_t FONT_8[5] = {0x36, 0x49, 0x49, 0x49, 0x36};
static const uint8_t FONT_9[5] = {0x06, 0x49, 0x49, 0x29, 0x1E};

static const uint8_t FONT_COLON[5]   = {0x00, 0x36, 0x36, 0x00, 0x00};
static const uint8_t FONT_MINUS[5]   = {0x08, 0x08, 0x08, 0x08, 0x08};
static const uint8_t FONT_GT[5]      = {0x41, 0x22, 0x14, 0x08, 0x00};
static const uint8_t FONT_AMP[5]     = {0x36, 0x49, 0x55, 0x22, 0x50};
static const uint8_t FONT_SLASH[5]   = {0x01, 0x02, 0x04, 0x08, 0x10};
static const uint8_t FONT_DOT[5]     = {0x00, 0x60, 0x60, 0x00, 0x00};
static const uint8_t FONT_QUESTION[5] = {0x02, 0x01, 0x51, 0x09, 0x06};

static const uint8_t *oled_get_glyph(char c)
{
    if(c >= 'a' && c <= 'z')
    {
        c = c - 'a' + 'A';
    }

    switch(c)
    {
        case 'A': return FONT_A;
        case 'B': return FONT_B;
        case 'C': return FONT_C;
        case 'D': return FONT_D;
        case 'E': return FONT_E;
        case 'F': return FONT_F;
        case 'G': return FONT_G;
        case 'H': return FONT_H;
        case 'I': return FONT_I;
        case 'J': return FONT_J;
        case 'K': return FONT_K;
        case 'L': return FONT_L;
        case 'M': return FONT_M;
        case 'N': return FONT_N;
        case 'O': return FONT_O;
        case 'P': return FONT_P;
        case 'Q': return FONT_Q;
        case 'R': return FONT_R;
        case 'S': return FONT_S;
        case 'T': return FONT_T;
        case 'U': return FONT_U;
        case 'V': return FONT_V;
        case 'W': return FONT_W;
        case 'X': return FONT_X;
        case 'Y': return FONT_Y;
        case 'Z': return FONT_Z;

        case '0': return FONT_0;
        case '1': return FONT_1;
        case '2': return FONT_2;
        case '3': return FONT_3;
        case '4': return FONT_4;
        case '5': return FONT_5;
        case '6': return FONT_6;
        case '7': return FONT_7;
        case '8': return FONT_8;
        case '9': return FONT_9;
        
        case '?': return FONT_QUESTION;
        case ':': return FONT_COLON;
        case '-': return FONT_MINUS;
        case '>': return FONT_GT;
        case '&': return FONT_AMP;
        case '/': return FONT_SLASH;
        case '.': return FONT_DOT;
        case ' ': return FONT_SPACE;
        default:  return FONT_SPACE;
    }
}

void oled_put_char(char c)
{
    const uint8_t *glyph = oled_get_glyph(c);

    oled_write_bytes(glyph, 5);

    // 1 empty column between characters
    oled_data(0x00);
}

void oled_puts(const char *str)
{
    while(*str)
    {
        oled_put_char(*str++);
    }
}