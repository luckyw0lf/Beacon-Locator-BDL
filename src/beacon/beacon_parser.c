#include "beacon_parser.h"
#include <string.h>

bool parse_beacon_string(const char *raw_string, IbeaconData_t *parsed_data)
{
    if (raw_string == NULL || parsed_data == NULL)
    {
        return false;
    }

    if (strlen(raw_string) < 78)
    {
        return false;
    }

    strncpy(parsed_data->major, raw_string + 50, 4);
    parsed_data->major[4] = '\0';

    strncpy(parsed_data->minor, raw_string + 54, 4);
    parsed_data->minor[4] = '\0';

    strncpy(parsed_data->rssi, raw_string + 74, 4);
    parsed_data->rssi[4] = '\0';

    strncpy(parsed_data->factoryId, raw_string + 8, 8);
    parsed_data->factoryId[8] = '\0';

    return true;
}