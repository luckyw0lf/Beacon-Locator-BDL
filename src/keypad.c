#include <MCXA153.h>
#include <board.h>
#include <stdio.h>
#include <stdbool.h>
#include "helperFunctions.h"
#include "keypad.h"
#include "mpr121.h"
#include "states.h"

volatile uint16_t pressedKey;
volatile char keypadFlag;
extern uint16_t touch_reg;
extern uint32_t touch_interrupt_now;
extern uint32_t ms;
extern char handle_touch_interrupt;
extern State_t STATE_BOOT_MENU;

const char* TOUCHPAD_STRINGS[16] = {
    "TWO",    // (1 << 15)
    "FIVE",   // (1 << 14)
    "EIGHT",  // (1 << 13)
    "ENTER",  // (1 << 12)
    "ONE",    // (1 << 11)
    "FOUR",   // (1 << 10)
    "SEVEN",  // (1 << 9)
    "LEFT",   // (1 << 8)
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "THREE",  // (1 << 3)
    "SIX",    // (1 << 2)
    "NINE",   // (1 << 1)
    "RIGHT"   // (1 << 0)
};

void initKeypad(){
   // initialize the keypad
   // make sure lpi2c is active
    mpr121_setup();

    keypadFlag = false;
    // set up intterupt pins
    setUpPin(PORT3, 1, 0,  PORT_PCR_LK(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0));

    GPIO3->ICR[1] = GPIO_ICR_ISF(1) | GPIO_ICR_IRQC(0b1010);

    // Enable GPIO3 interrupts
    NVIC_SetPriority(GPIO3_IRQn, 3);
    NVIC_ClearPendingIRQ(GPIO3_IRQn);
    NVIC_EnableIRQ(GPIO3_IRQn);
    getTouchReg();

}

void GPIO3_IRQHandler(void)
{
    // Clear the interrupt
    NVIC_ClearPendingIRQ(GPIO3_IRQn);

    if(GPIO_ISFR_ISF1(1) != 0){
        GPIO3->ISFR[0] = GPIO_ISFR_ISF1(1);

        // Handle the event
        touch_interrupt_now = ms;
        handle_touch_interrupt = true;

        // read the register, the keypad doesn't continue generating interrupts if the register hasn't been read after interrupt
        // Maybe in the future set up a queue to read the register multiple timmes
        // but for now it is good enough.
        getTouchReg();
    }
}

void readKeypad(){
    // read the mpr register
    getTouchReg();

    // plot the touch reg to keys pressed
    // crazy logic:
    if(touch_reg == 0) return;

    // we're going to use a built in function of GCC, Count leading zeros, 
    // it will give back the position of the MSB
    pressedKey = __CLZ(touch_reg) - 16;
    keypadFlag = true;

    printf("proccessing keypress, key: %s \r\n", TOUCHPAD_STRINGS[pressedKey]);
}

void keypadReInit(StateMachine_t *sm){
    mpr121_reconfigure();
    addToQueue(sm, &STATE_BOOT_MENU);
    sm->isBusy = false;
}