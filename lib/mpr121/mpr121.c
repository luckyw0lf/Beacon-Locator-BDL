#include "mpr121.h"
#include "lpi2c.h"

#define I2C_ADRESS (0x5A)

extern volatile uint32_t ms;
static uint8_t out = 0;

void mpr121_init(void){
    lpi2c_controller_init();

    out = 0b00000000;
    lpi2c_write(I2C_ADDRESS, out, NULL, 0);

    out = 202;
    // lpi2c_write(I2C_ADRESS, out, )
}