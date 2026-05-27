#ifndef OLED_H
#define OLED_H

#include <stdint.h>

void oled_init(void);
void oled_clear(void);
void oled_test_pattern(void);
void oled_set_cursor(uint8_t column, uint8_t page);
void oled_put_char(char c);
void oled_puts(const char *str);

#endif // OLED_H