#ifndef STATE_MENU_H
#define STATE_MENU_H

#include "states.h"

typedef struct {
    State_t *selState;
    char name[14];
} menuItem_t;

void menu_entry(StateMachine_t *sm);
void menu_main(StateMachine_t *sm);
void menu_exit(StateMachine_t *sm);

#endif