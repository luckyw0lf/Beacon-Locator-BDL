
#include "leds.h"
#include "serial.h"
#include "states.h"
#include <stdio.h>

#ifdef DEBUG
#define TARGETSTR "Debug"
#else
#define TARGETSTR "Release"
#endif

void init_entry(StateMachine_t *sm){
    serial_init(115200);
    
    printf("BDL REVERSE GEOCAHCE BOOTING.....\r\n");
    printf("%s build %s %s\r\n", TARGETSTR, __DATE__, __TIME__);
    
    printf("ENTER %s\r\n", sm->state->name);
    
    // Globally enable interrupts
    __enable_irq();
    
    leds_init();
}
void init_main(StateMachine_t *sm){
    sm->isBusy = false;
}
void init_exit(StateMachine_t *sm){
}