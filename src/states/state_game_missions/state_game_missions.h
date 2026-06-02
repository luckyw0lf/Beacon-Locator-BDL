#ifndef STATE_GAME_MISSIONS_H
#define STATE_GAME_MISSIONS_H

#include "states.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
    char buffer [16];
    uint8_t index;
    bool submitted;
}InputBuffer_t;


void experience_entry(StateMachine_t *sm);
void experience_main(StateMachine_t *sm);
void experience_exit(StateMachine_t *sm);

void material_question_entry(StateMachine_t *sm);
void material_question_main(StateMachine_t *sm);
void material_question_exit(StateMachine_t *sm);

void material_collection_entry(StateMachine_t *sm);
void material_collection_main(StateMachine_t *sm);
void material_collection_exit(StateMachine_t *sm);

void build_test_entry(StateMachine_t *sm);
void build_test_main(StateMachine_t *sm);
void build_test_exit(StateMachine_t *sm);

void finish_entry(StateMachine_t *sm);
void finish_main(StateMachine_t *sm);
void finish_exit(StateMachine_t *sm);


#endif