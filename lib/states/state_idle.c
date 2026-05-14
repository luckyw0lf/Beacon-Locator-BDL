
#include "leds.h"
#include "serial.h"
#include "states.h"
#include <stdio.h>

void idle_entry(StateMachine_t *sm){
}
void idle_main(StateMachine_t *sm){
    sm->isBusy = false;
}
void idle_exit(StateMachine_t *sm){

}