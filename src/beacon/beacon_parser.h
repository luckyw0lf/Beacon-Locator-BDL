#ifndef BEACON_PARSER_H
#define BEACON_PARSER_H

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

bool parse_beacon_string(const char *raw_string, IbeaconData_t *parsed_data);

#endif