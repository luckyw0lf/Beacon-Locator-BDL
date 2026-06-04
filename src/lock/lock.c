#include "lock.h"

#include <MCXA153.h>
#include <board.h>
#include <stdio.h>

#include "helperFunctions.h"

#define LOCK_PORT PORT3
#define LOCK_GPIO GPIO3
#define LOCK_PIN  14

static void delay_cycles(volatile int cycles)
{
    while (cycles-- > 0)
    {
    }
}

void lock_init(void)
{
    setUpPin(
        LOCK_PORT,
        LOCK_PIN,
        1,
        PORT_PCR_MUX(0) | PORT_PCR_DSE(1)
    );

    LOCK_GPIO->PCOR = (1 << LOCK_PIN); // lock off
    printf("LOCK INIT DONE\r\n");
}

void lock_open(void)
{
    printf("LOCK OPEN\r\n");

    LOCK_GPIO->PSOR = (1 << LOCK_PIN); // MOSFET on

    delay_cycles(3000000); // test value, adjust duration

    LOCK_GPIO->PCOR = (1 << LOCK_PIN); // MOSFET off

    printf("LOCK CLOSED/OFF\r\n");
}