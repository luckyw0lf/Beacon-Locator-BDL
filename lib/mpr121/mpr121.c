#include "mpr121.h"
#include "lpi2c.h"
#include "serial.h"
#include <stdio.h>

#define I2C_ADRESS (0x5A)

extern volatile uint32_t ms;
static uint8_t out = 0;

void mpr121_init(void)
{
    lpi2c_controller_init();

    // soft reset
    out = 0x63; lpi2c_write(I2C_ADRESS, 0x80, &out, 1);

    // Give the MPR121 a tiny window to reboot its internal core
    for (volatile int i = 0; i < 2000; i++){};

    out = 0x00; lpi2c_write(I2C_ADRESS, ELE_CFG, &out, 1);

    // Section A - Controls filtering when data is > baseline.
    out = 0x01; lpi2c_write(I2C_ADRESS, MHD_R, &out, 1);
    out = 0x01; lpi2c_write(I2C_ADRESS, NHD_R, &out, 1);
    out = 0x00; lpi2c_write(I2C_ADRESS, NCL_R, &out, 1);
    out = 0x00; lpi2c_write(I2C_ADRESS, FDL_R, &out, 1);

    // Section B - Controls filtering when data is < baseline.
    out = 0x01; lpi2c_write(I2C_ADRESS, MHD_F, &out, 1);
    out = 0x01; lpi2c_write(I2C_ADRESS, NHD_F, &out, 1);
    out = 0xFF; lpi2c_write(I2C_ADRESS, NCL_F, &out, 1);
    out = 0x02; lpi2c_write(I2C_ADRESS, FDL_F, &out, 1);

    // Section C - Sets touch and release thresholds for each electrode
    out = 0x06;
    lpi2c_write(I2C_ADRESS, ELE0_T, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE1_T, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE2_T, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE3_T, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE4_T, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE5_T, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE6_T, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE7_T, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE8_T, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE9_T, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE10_T, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE11_T, &out, 1);
    out = 0x0A;
    lpi2c_write(I2C_ADRESS, ELE0_R, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE1_R, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE2_R, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE3_R, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE4_R, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE5_R, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE6_R, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE7_R, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE8_R, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE9_R, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE10_R, &out, 1);
    lpi2c_write(I2C_ADRESS, ELE11_R, &out, 1);

    // Section D
    // Set the Filter Configuration
    // Set ESI2
    out = 0x04; lpi2c_write(I2C_ADRESS, FIL_CFG, &out, 1);

    // Section E
    // Electrode Configuration
    // Set ELE_CFG to 0x00 to return to standby mode
    out = 0x0C; lpi2c_write(I2C_ADRESS, ELE_CFG, &out, 1); // Enables all 12 Electrodes

    // Section F
    // Enable Auto Config and auto Reconfig
    /*set_register(0x5A, ATO_CFG0, 0x0B);
    set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
    set_register(0x5A, ATO_CFGT, 0xB5);*/
    // Target = 0.9*USL = 0xB5 @3.3V
    out = 0x0C; lpi2c_write(I2C_ADRESS, ELE_CFG, &out, 1);

   }


//Combines 0x00 an 0x01, these contain touch status data
// E7, E6, E5, E4, E3, E2, E1, E0, OVCF, 0, 0, E12, E11, E10, E9, E8   
uint16_t getTouchReg(){
    uint16_t combined = 0x0;
    uint8_t reg0 = 0x0;
    uint8_t reg1 = 0x0;

    lpi2c_read(I2C_ADRESS, 0x00, &reg0, 1);
    while(lpi2c_busy()){};
    lpi2c_read(I2C_ADRESS, 0x01, &reg1, 1);
    while(lpi2c_busy()){};

    printf("%x\r\n", reg0);
    printf("%x\r\n", reg1);

    combined = ((reg0 << 8) | reg1); 

    return combined;    
}



 // lpi2c_read(I2C_ADRESS, 0x5E, &read, 1);
    // while(lpi2c_busy()){}
    // printf("ECR Mode Register: 0x%02X\r\n", read);

    // // Calibration lock
    // out = 0b01000000;
    // lpi2c_write(I2C_ADRESS, 0x5E, &out, 1);

    // lpi2c_read(I2C_ADRESS, 0x5E, &read, 1);
    // while(lpi2c_busy()){}
    // printf("ECR Mode Register: 0x%02X\r\n", read);

    // // 2. Configure Baseline Filtering (MHD / NHD / NCL / FDL)
    // // Section A: Rising (When capacitance rises)
    // out = 0x01; lpi2c_write(I2C_ADRESS, 0x2B, &out, 1); // MHD Rising
    // out = 0x01; lpi2c_write(I2C_ADRESS, 0x2C, &out, 1); // NHD Rising
    // out = 0x00; lpi2c_write(I2C_ADRESS, 0x2D, &out, 1); // NCL Rising
    // out = 0x00; lpi2c_write(I2C_ADRESS, 0x2E, &out, 1); // FDL Rising

    // // Section B: Falling (When capacitance drops)
    // out = 0x01; lpi2c_write(I2C_ADRESS, 0x2F, &out, 1); // MHD Falling
    // out = 0x05; lpi2c_write(I2C_ADRESS, 0x30, &out, 1); // NHD Falling
    // out = 0x00; lpi2c_write(I2C_ADRESS, 0x31, &out, 1); // NCL Falling
    // out = 0x00; lpi2c_write(I2C_ADRESS, 0x32, &out, 1); // FDL Falling

    // // confirm we are actually setting stuff: we are.
    // lpi2c_read(I2C_ADRESS, 0x2F, &read, 1);
    // while(lpi2c_busy()){}
    // printf("MHD Falling Reg: 0x%02X\r\n", read);

    // // Set Touch and Release Thresholds for all 12 electrodes
    // for (uint8_t i = 0; i < 12; i++) {
    //     out = 0x5A; // Touch threshold
    //     lpi2c_write(I2C_ADRESS, 0x41 + (i * 2), &out, 1);

    //     out = 0x5A; // Release threshold
    //     lpi2c_write(I2C_ADRESS, 0x42 + (i * 2), &out, 1);
    // }

    // // confirm we are actually setting stuff: we are.
    // lpi2c_read(I2C_ADRESS, 0x45, &read, 1);
    // while(lpi2c_busy()){}
    // printf("ELE2 Touch Thresh: 0x%02X\r\n", read);

    // // configure debounce 0x5B
    // out = 0b00100010; // 10 samples, default current
    // lpi2c_write(I2C_ADRESS, 0x5B, &out, 1);

    // // 4. Configure AFE (First Filter Iterations and Charge Current)
    // out = 0b01010000; // 10 samples, default current
    // lpi2c_write(I2C_ADRESS, 0x5C, &out, 1);

    // // 0x5D should have default values.
    // // 0x5E should put it in running mode, which doesn't allow this to change config, thus it's at the end

    // // all CDC, CDT stuff (0x5F-0x72) should be autoset by the auto config. we should check some of these
    // // 5. Configure Auto-Configuration Limits
    // out = 200; lpi2c_write(I2C_ADRESS, 0x7D, &out, 1); // USL
    // out = 131; lpi2c_write(I2C_ADRESS, 0x7E, &out, 1); // LSL
    // out = 150; lpi2c_write(I2C_ADRESS, 0x7F, &out, 1); // Target Level (TL)

    // // auto config reg 2 0x7c
    // // generate intterupts if auto config fails, reconfig fails or out of range error
    // out = 0b00000111;
    // lpi2c_write(I2C_ADRESS, 0x7C, &out, 1);

    // // Enable Auto-Config Control
    // out = 0b01101111;
    // lpi2c_write(I2C_ADRESS, 0x7B, &out, 1);

    // read_oor();

    // // 6. START RUN MODE (Enable all 12 electrodes and baseline tracking)
    // out = 0b01001111; // Enables ELE0 through ELE11
    // lpi2c_write(I2C_ADRESS, 0x5E, &out, 1);

    // // Give the MPR121 a tiny window to calibrate (don't know if needed)
    // for(volatile int i=0; i<2000; i++){};

    // // read some of the electrode currents, to see if the auto calibrate has actually set values.
    // lpi2c_read(I2C_ADRESS, 0x5F, &read, 1);
    // while(lpi2c_busy()){}
    // printf("ELE Current 0 Reg: 0x%02X\r\n", read);
    // lpi2c_read(I2C_ADRESS, 0x60, &read, 1);
    // while(lpi2c_busy()){}
    // printf("ELE Current 1 Reg: 0x%02X\r\n", read);
    // lpi2c_read(I2C_ADRESS, 0x61, &read, 1);
    // while(lpi2c_busy()){}
    // printf("ELE Current 2 Reg: 0x%02X\r\n", read);
    // lpi2c_read(I2C_ADRESS, 0x62, &read, 1);
    // while(lpi2c_busy()){}
    // printf("ELE Current 3 Reg: 0x%02X\r\n", read);

    // // ECR Mode Register: 0x4F
    // lpi2c_read(I2C_ADRESS, 0x5E, &read, 1);
    // while(lpi2c_busy()){}
    // printf("ECR Mode Register: 0x%02X\r\n", read);

    // read_oor();
