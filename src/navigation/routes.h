#ifndef ROUTES_H
#define ROUTES_H

#include <stdint.h>
#include "states.h"

typedef enum
{
    BEACON_B01,   // minor 0001
    BEACON_B02,   // minor 0002
    BEACON_B03,   // minor 0003
    BEACON_B04,   // minor 0004
    BEACON_B05,   // minor 0005
    BEACON_B06,   // minor 0006
    BEACON_B07,   // minor 0007
    BEACON_B08,   // minor 0008
    BEACON_B09,   // minor 0009
    BEACON_B10,   // minor 000A
    BEACON_UNKNOWN
} BeaconId_t;

typedef enum
{
    BEACON_ROLE_IGNORE,
    BEACON_ROLE_PASS_BY,
    BEACON_ROLE_APPROACH,
    BEACON_ROLE_TARGET,
    BEACON_ROLE_WRONG_WAY
} BeaconRole_t;

typedef enum
{
    ROUTE_START_TO_EXPERIENCE,
    ROUTE_EXPERIENCE_TO_MATERIAL_QUESTION,
    ROUTE_MATERIAL_QUESTION_TO_COLLECTION,
    ROUTE_COLLECTION_TO_BUILD_TEST,
    ROUTE_END
} RouteId_t;

typedef struct
{
    BeaconId_t id;
    const char *major;
    const char *minor;
    const char *label;
    int rssi[3];
    int rssiSize;
    int index;
    int averageRssi;
} BeaconDefinition_t;

typedef struct
{
    BeaconId_t beaconId;
    BeaconRole_t role;
    int rssiThreshold;

    char *line1;
    char *line2;
    char *line3;
} RouteBeaconRule_t;

typedef struct
{
    RouteId_t id;
    const char *name;

    const char *targetLine1;
    const char *targetLine2;
    const char *targetLine3;

    const RouteBeaconRule_t *rules;
    uint8_t ruleCount;
    State_t *puzzleState;

} RouteProfile_t;

const BeaconDefinition_t *routes_get_beacon_definition(BeaconId_t beaconId);
BeaconDefinition_t *routes_find_beacon_by_major_minor(const char *major, const char *minor);

const RouteProfile_t *routes_get_profile(RouteId_t routeId);
const RouteBeaconRule_t *routes_find_rule(const RouteProfile_t *profile, char const *major, const char *minor);
//getting data from GUI 
void routes_update_beacon(const char* label, const char* major, const char* minor);
void routes_dump_config(void);

#endif
