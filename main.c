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
#include "keypad.h"
#include "hm10.h"
#include "state_searching_beacon.h"

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
void emptyFunc(StateMachine_t *sm);
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

// for menu
State_t STATE_RECONFIG_KEYPAD = { &emptyFunc, &keypadReInit, &emptyFunc, "ADMIN_MODE" };
volatile char handle_touch_interrupt = false;
volatile uint32_t touch_interrupt_now = 0;
// also serves for debouncing, initially only needed for a small delay but this works nicely for debouncing too.
static unsigned char touch_ms_delay = 80;
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Main application
int main(void)
{   
    initStateMachine(&systemSM, &STATE_INIT);
    serial_init(115200);
    hm10_init(9600);

    for(volatile int i =0; i < 5000000; i ++) {}

    initStateMachine(&systemSM, &state_searching_beacon);
    
    while(1)
    {
        if(handle_touch_interrupt){
            if(ms - touch_interrupt_now >= touch_ms_delay){
                readKeypad();
                handle_touch_interrupt = false;
            }
        }
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

void emptyFunc(StateMachine_t *sm){}