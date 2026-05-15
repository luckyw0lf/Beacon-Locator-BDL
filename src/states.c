#include "states.h"
#include "fifo.h"
#include <stdio.h>

void initStateMachine(StateMachine_t *sm, State_t *firstState){
    sm->state = firstState;
    sm->isBusy = true;

    f_init(&sm->queue, 
              (void*)sm->buffer,            // Point to the internal array
              16,                           // Max items?
              sizeof(const State_t *));     // Size of one pointer

    // Can't print here, serial isn't initialized yet.
    sm->state->entry(sm);

    printf("MAIN %s\r\n", sm->state->name);
    sm->state->main(sm);
}

void addToQueue(StateMachine_t *sm, State_t *newState){
    f_push(&sm->queue, &newState);
}

void updateStateMachine(StateMachine_t *sm){
    const State_t *nextState = NULL;
    // if not busy pop the first fifo, and read the next one if there is a new one, 
    // put this into the newState variable;
    if(!sm->isBusy && f_pop(&sm->queue, &nextState)){
        printf("EXIT %s\r\n", sm->state->name);
        sm->state->exit(sm);
        sm->state = nextState;
        sm->isBusy = true;
        printf("ENTER %s\r\n", sm->state->name);
        sm->state->entry(sm);
        printf("MAIN %s\r\n", sm->state->name);
        return;
    }

    sm->state->main(sm);
}