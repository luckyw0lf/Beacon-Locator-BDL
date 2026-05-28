#ifndef BEACON_PARSER_H
#define BEACON_PARSER_H

#include <stdbool.h>
#include "state_searching_beacon.h"

bool parse_beacon_string(const char *raw_string, IbeaconData_t *parsed_data);

#endif