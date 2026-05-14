#include "fifo.h"
#include "state_init.h"
#include "state_idle.h"

#ifndef STATES_H
#define STATES_H

#define QUEUE_CAPACITY 10

// this tells the compiler that StateMachine_t will exist in the future.
typedef struct StateMachine_t StateMachine_t;
typedef struct State_t State_t;

struct State_t {
    void (*entry)(StateMachine_t *sm);
    void (*main)(StateMachine_t *sm);
    void (*exit)(StateMachine_t *sm);
    char name[20];
};


struct StateMachine_t {
    const State_t *state;
    bool isBusy;
    const State_t *buffer[QUEUE_CAPACITY];
    fifo_t queue;
};


void initStateMachine(StateMachine_t *sm, State_t *firstState);
void addToQueue(StateMachine_t *sm, State_t *newState);
void updateStateMachine(StateMachine_t *sm);

#endif