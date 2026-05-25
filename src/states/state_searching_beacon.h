#ifndef STATE_SEARCHING_BEACON_H
#define STATE_SEARCHING_BEACON_H

#include "states.h"
#include <stdbool.h>

typedef struct
{
    char factoryId[9];
    char uuid[33];
    char major[5];
    char minor[5];
    char power[3];
    char mac[13];
    char rssi[5];
} IbeaconData_t;

extern State_t state_searching_beacon;

void search_entry(StateMachine_t *sm);
void search_main(StateMachine_t *sm);
void search_exit(StateMachine_t *sm);

bool parse_beacon_string(const char *raw_string, IbeaconData_t *parsed_data);

#endif