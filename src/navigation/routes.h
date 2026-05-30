#ifndef ROUTES_H
#define ROUTES_H

#include <stdint.h>

typedef enum
{
    BEACON_B01,
    BEACON_B02,
    BEACON_B03,
    BEACON_B04,
    BEACON_B05,
    BEACON_B06,
    BEACON_B07,
    BEACON_B08,
    BEACON_B09,
    BEACON_B10,
    BEACON_UNKNOWN
} BeaconId_t;
// Ignore still included in case of we would like deliberatly ignore a beacon.We dont have to use it.
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
    ROUTE_BUILD_TEST_TO_FINISH,
    ROUTE_UNKNOWN
} RouteId_t;

typedef struct
{
    BeaconId_t id;
    const char *major;
    const char *minor;
    const char *label;
} BeaconDefinition_t;

typedef struct
{
    BeaconId_t beaconId;
    BeaconRole_t role;
    int rssiThreshold;
} RouteBeaconRule_t;

typedef struct
{
    RouteId_t id;
    const char *displayName;
    const RouteBeaconRule_t *rules;
    uint8_t ruleCount;
} RouteProfile_t;

const BeaconDefinition_t *routes_get_beacon_definition(BeaconId_t beaconId);
const BeaconDefinition_t *routes_find_beacon_by_major_minor(const char *major, const char *minor);

const RouteProfile_t *routes_get_profile(RouteId_t routeId);
const RouteBeaconRule_t *routes_find_rule(const RouteProfile_t *profile, BeaconId_t beaconId);

#endif