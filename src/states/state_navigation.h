#ifndef STATE_NAVIGATION_H
#define STATE_NAVIGATION_H

#include "states.h"
#include "routes.h"

void navigation_set_route(RouteId_t routeId);
void navigation_set_next_state(State_t *nextState);

void navigation_entry(StateMachine_t *sm);
void navigation_main(StateMachine_t *sm);
void navigation_exit(StateMachine_t *sm);

#endif