#include "mpr121.h"
#include "lpi2c.h"
#include "serial.h"
#include <stdio.h>

#define I2C_ADDRESS (0x5A)

extern volatile uint32_t ms;
static uint8_t out = 0;
static uint8_t read = 0;
volatile uint16_t touch_reg = 0;

void mpr121_setup(){
    lpi2c_read(I2C_ADDRESS, 0x5E, &read, 1);
    while(lpi2c_busy()){}
    if(read == 0){
        printf("Reconfiguring keypad.\r\n");
        mpr121_reconfigure();
        return;
    }
    printf("Keypad already configured.\r\n");
}

void mpr121_reconfigure()
{

    // soft reset
    out = 0x63; lpi2c_write(I2C_ADDRESS, 0x80, &out, 1);

    // Give the MPR121 a tiny window to reboot its internal core
    for (volatile int i = 0; i < 2000; i++){};

    out = 0x00; lpi2c_write(I2C_ADDRESS, ELE_CFG, &out, 1);

    // Section A - Controls filtering when data is > baseline.
    out = 0x01; lpi2c_write(I2C_ADDRESS, MHD_R, &out, 1);
    out = 0x01; lpi2c_write(I2C_ADDRESS, NHD_R, &out, 1);
    out = 0x00; lpi2c_write(I2C_ADDRESS, NCL_R, &out, 1);
    out = 0x00; lpi2c_write(I2C_ADDRESS, FDL_R, &out, 1);

    // Section B - Controls filtering when data is < baseline.
    out = 0x01; lpi2c_write(I2C_ADDRESS, MHD_F, &out, 1);
    out = 0x01; lpi2c_write(I2C_ADDRESS, NHD_F, &out, 1);
    out = 0xFF; lpi2c_write(I2C_ADDRESS, NCL_F, &out, 1);
    out = 0x02; lpi2c_write(I2C_ADDRESS, FDL_F, &out, 1);

    // Section C - Sets touch and release thresholds for each electrode
    out = 0x0C;
    lpi2c_write(I2C_ADDRESS, ELE0_T, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE1_T, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE2_T, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE3_T, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE4_T, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE5_T, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE6_T, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE7_T, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE8_T, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE9_T, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE10_T, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE11_T, &out, 1);
    out = 0x0F;
    lpi2c_write(I2C_ADDRESS, ELE0_R, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE1_R, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE2_R, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE3_R, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE4_R, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE5_R, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE6_R, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE7_R, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE8_R, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE9_R, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE10_R, &out, 1);
    lpi2c_write(I2C_ADDRESS, ELE11_R, &out, 1);

    // Section D
    // Set the Filter Configuration
    // Set ESI2
    out = 0x04; lpi2c_write(I2C_ADDRESS, FIL_CFG, &out, 1);

    // Section E
    // Electrode Configuration
    // Set ELE_CFG to 0x00 to return to standby mode
    out = 0x0C; lpi2c_write(I2C_ADDRESS, ELE_CFG, &out, 1); // Enables all 12 Electrodes

    // Section F
    // Enable Auto Config and auto Reconfig
    /*set_register(0x5A, ATO_CFG0, 0x0B);
    set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
    set_register(0x5A, ATO_CFGT, 0xB5);*/
    // Target = 0.9*USL = 0xB5 @3.3V
    out = 0x0C; lpi2c_write(I2C_ADDRESS, ELE_CFG, &out, 1);

   }


//Combines 0x00 an 0x01, these contain touch status data
// E7, E6, E5, E4, E3, E2, E1, E0, OVCF, 0, 0, E12, E11, E10, E9, E8   
void getTouchReg(){

    // maybe think of a way to make non blocking
    uint8_t buffer[2] = {0, 0};

    lpi2c_read(I2C_ADDRESS, 0x00, buffer, 2);
    while (lpi2c_busy()) {}

    touch_reg = ((uint16_t)buffer[0] << 8) | buffer[1];

    return;  
}
