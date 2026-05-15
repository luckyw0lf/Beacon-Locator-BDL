#include <MCXA153.h>
#include <board.h>
#include <stdio.h>
#include <stdbool.h>
#include "helperFunctions.h"
#include "keypad.h"

volatile char pressedKey;
volatile char keypadFlag;

void initKeypad(){
    // later make code here to setup IRQ with i2c keypad, for now using normal buttons.
    // I looked at Hugo his code for the button example
    keypadFlag = false;
    setUpPin(PORT3, 29, 0, PORT_PCR_IBE(1) | PORT_PCR_MUX(0));
    setUpPin(PORT1, 7, 0, PORT_PCR_IBE(1) | PORT_PCR_MUX(0));

    PORT1->PCR[7] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0);
    PORT3->PCR[29] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0);

    GPIO1->ICR[7] = GPIO_ICR_ISF(1) | GPIO_ICR_IRQC(0b1010);
    GPIO3->ICR[29] = GPIO_ICR_ISF(1) | GPIO_ICR_IRQC(0b1010);
    
    // Enable GPIO1 interrupts
    NVIC_SetPriority(GPIO1_IRQn, 3);
    NVIC_ClearPendingIRQ(GPIO1_IRQn);
    NVIC_EnableIRQ(GPIO1_IRQn);

    // Enable GPIO3 interrupts
    NVIC_SetPriority(GPIO3_IRQn, 3);
    NVIC_ClearPendingIRQ(GPIO3_IRQn);
    NVIC_EnableIRQ(GPIO3_IRQn);
}

void GPIO1_IRQHandler(void)
{
    // Clear the interrupt
    NVIC_ClearPendingIRQ(GPIO1_IRQn);

    // Interrupt handler triggered by P1_7?
    if((GPIO1->ISFR[0] & GPIO_ISFR_ISF7(1)) != 0)
    {
        // Clear the flag
        GPIO1->ISFR[0] = GPIO_ISFR_ISF7(1);

        // Handle the event
        pressedKey = DOWN;
        keypadFlag = 1;
    }
}

void GPIO3_IRQHandler(void)
{
    // Clear the interrupt
    NVIC_ClearPendingIRQ(GPIO3_IRQn);

    // Interrupt handler triggered by P3_29?
    if((GPIO3->ISFR[0] & GPIO_ISFR_ISF29(1)) != 0)
    {
        // Clear the flag
        GPIO3->ISFR[0] = GPIO_ISFR_ISF29(1);

        // Handle the event
        pressedKey = ENTER;
        keypadFlag = 1;
    }
}
