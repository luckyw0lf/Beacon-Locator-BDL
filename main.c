#include <board.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <MCXA153.h>

#include "leds.h"
#include "serial.h"
#include "states.h"
#include "state_init.h"
#include "state_menu.h"

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------
#ifdef DEBUG
#define TARGETSTR "Debug"
#else
#define TARGETSTR "Release"
#endif

// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------
volatile uint32_t ms = 0;
static volatile uint32_t previous_ms = 0;
static StateMachine_t systemSM;
// define states here
State_t STATE_INIT = { &init_entry, &init_main, &init_exit, "INIT" };
State_t STATE_BOOT_MENU = { &menu_entry, &menu_main, &menu_exit, "BOOT_MENU" };
State_t STATE_ADMIN_MODE = { &menu_entry, &menu_main, &menu_exit, "ADMIN_MODE" };
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Main application
int main(void)
{   
    initStateMachine(&systemSM, &STATE_INIT);

    while(1)
    {
        updateStateMachine(&systemSM);
    }
}
// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
void SysTick_Handler(void)
{
    ms++;
}