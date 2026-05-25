#include <MCXA153.h>
#include <board.h>
#include <stdio.h>
#include <stdbool.h>
#include "helperFunctions.h"
#include "keypad.h"
#include "mpr121.h"

volatile char pressedKey;
volatile char keypadFlag;

void initKeypad(){
    // later make code here to setup IRQ with i2c keypad, for now using normal buttons.
    // I looked at Hugo his code for the button example
   // initialize the keypad
    mpr121_init();

    keypadFlag = false;
    // set up intterupt pins
    setUpPin(PORT3, 1, 0,  PORT_PCR_LK(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0));

    GPIO3->ICR[1] = GPIO_ICR_ISF(1) | GPIO_ICR_IRQC(0b1010);

    // Enable GPIO3 interrupts
    NVIC_SetPriority(GPIO3_IRQn, 3);
    NVIC_ClearPendingIRQ(GPIO3_IRQn);
    NVIC_EnableIRQ(GPIO3_IRQn);
}

void GPIO3_IRQHandler(void)
{
    // Clear the interrupt
    NVIC_ClearPendingIRQ(GPIO3_IRQn);

    if(GPIO_ISFR_ISF1(1) != 0){
        GPIO3->ISFR[0] = GPIO_ISFR_ISF1(1);

        // Handle the event
       getTouchReg();
        // printf("keypad status %d\r\n", padStatus);
    }
}
