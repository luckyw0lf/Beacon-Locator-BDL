#include <board.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "leds.h"
#include "serial.h"
#include "states.h"
#include "state_init.h"
#include "state_idle.h"

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
static volatile uint32_t ms = 0;
static volatile uint32_t previous_ms = 0;
static StateMachine_t systemSM;
extern char * stateEnumToText[];

// define states here
static State_t STATE_INIT = { &init_entry, &init_main, &init_exit, "Initializing.." };
static State_t STATE_IDLE = { &idle_entry, &idle_main, &idle_exit, "Idle." };
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Main application
int main(void)
{   
    initStateMachine(&systemSM, &STATE_INIT);
    addToQueue(&systemSM, &STATE_IDLE);
    addToQueue(&systemSM, &STATE_IDLE);

    while(1)
    {
        updateStateMachine(&systemSM);
    }
}
// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
