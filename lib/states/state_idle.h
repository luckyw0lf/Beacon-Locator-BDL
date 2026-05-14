#ifndef STATE_IDLE_H
#define STATE_IDLE_H

#include "states.h"

void idle_entry(StateMachine_t *sm);
void idle_main(StateMachine_t *sm);
void idle_exit(StateMachine_t *sm);

#endif