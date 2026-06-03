#include "routes.h"
#include <string.h>

extern State_t STATE_MENU;
extern State_t STATE_EXPERIENCE_MISSION;
extern State_t STATE_MATERIAL_QUESTION;
extern State_t STATE_MATERIAL_COLLECTION;
extern State_t STATE_BUILD_TEST;
extern State_t STATE_FINISH;

#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

/*
 * Beacon registry
 *
 * Physical beacon identity is separated from route meaning.
 *
 * All beacons use major 0B01.
 * The minor value identifies the physical beacon location.
 *
 * B01 = minor 0001
 * B02 = minor 0002
 * B03 = minor 0003
 * B04 = minor 0004
 * B05 = minor 0005
 * B06 = minor 0006
 * B07 = minor 0007
 * B08 = minor 0008
 * B09 = minor 0009
 * B10 = minor 000A
 */
const BeaconDefinition_t beaconDefinitions[] = {
    {BEACON_B01, "0B01", "0001", "B01", {}, 5, 0},
    {BEACON_B02, "0B01", "0002", "B02", {}, 5, 0},
    {BEACON_B03, "0B01", "0003", "B03", {}, 5, 0},
    {BEACON_B04, "0B01", "0004", "B04", {}, 5, 0},
    {BEACON_B05, "0B01", "0005", "B05", {}, 5, 0},
    {BEACON_B06, "0B01", "0006", "B06", {}, 5, 0},
    {BEACON_B07, "0B01", "0007", "B07", {}, 5, 0},
    {BEACON_B08, "0B01", "0008", "B08", {}, 5, 0},
    {BEACON_B09, "0B01", "0009", "B09", {}, 5, 0},
    {BEACON_B10, "0B01", "000A", "B10", {}, 5, 0}
};

const char beaconDefCount = 10;
/*
 * Route 1:
 * Start 0001 -> target 0006
 *
 * 0002, 0003 = pass-by
 * 0004 = turn left from corridor when close enough
 * 0005 = approach
 * 0006 = target room
 */
const RouteBeaconRule_t startToExperienceRules[] = {
    {BEACON_B02, BEACON_ROLE_PASS_BY,  -75, "Good direction", "Keep going", ""},
    {BEACON_B03, BEACON_ROLE_PASS_BY,  -75, "Good direction", "Keep going", ""},
    {BEACON_B04, BEACON_ROLE_PASS_BY,  -65, "Turn left", "From corridor", ""},
    {BEACON_B05, BEACON_ROLE_APPROACH, -70, "You are close", "Keep going", ""},
    {BEACON_B06, BEACON_ROLE_TARGET,   -65, "Experience room", "Reached", "Press ENTER"}
};

/*
 * Route 2:
 * From room 0006 -> target 0003
 *
 * 0005 and 0004 are on the way back.
 * 0003 is the target room.
 */
const RouteBeaconRule_t experienceToMaterialQuestionRules[] = {
    {BEACON_B05, BEACON_ROLE_PASS_BY,  -75, "Good direction", "Keep going", ""},
    {BEACON_B04, BEACON_ROLE_APPROACH, -70, "You are close", "Keep going", ""},
    {BEACON_B03, BEACON_ROLE_TARGET,   -65, "Question room", "Reached", "Press ENTER"}
};

/*
 * Route 3:
 * From room 0003 -> target 000A
 *
 * 0004, 0007, 0008, 0009 are on the route.
 * 0008 gives a turn-left instruction.
 * 0009 confirms the correct direction.
 * 000A is the target room.
 */
const RouteBeaconRule_t materialQuestionToCollectionRules[] = {
    {BEACON_B04, BEACON_ROLE_PASS_BY, -75, "Good direction", "Keep going", ""},
    {BEACON_B07, BEACON_ROLE_PASS_BY, -75, "Good direction", "Keep going", ""},
    {BEACON_B08, BEACON_ROLE_PASS_BY, -65, "Turn left", "From corridor", ""},
    {BEACON_B09, BEACON_ROLE_PASS_BY, -65, "Right track", "Keep going", ""},
    {BEACON_B10, BEACON_ROLE_TARGET,  -65, "Material room", "Reached", "Press ENTER"}
};

/*
 * Route 4:
 * From room 000A -> target 0008
 *
 * 0009 is a pass-by beacon.
 * 0007 gives the final turn-left hint.
 * 0008 is  the final target.
 */
const RouteBeaconRule_t collectionToBuildTestRules[] = {
    {BEACON_B09, BEACON_ROLE_PASS_BY, -75, "Good direction", "Keep going", ""},
    {BEACON_B07, BEACON_ROLE_PASS_BY, -65, "Turn left", "Room is nearby", ""},
    {BEACON_B08, BEACON_ROLE_TARGET,  -65, "Final room", "Reached", "Press ENTER"}
};

const RouteProfile_t routeProfiles[] = {
    {
        ROUTE_START_TO_EXPERIENCE,
        "Start to Experience Room",
        "Target:",
        "Experience room",
        "Follow the route",
        startToExperienceRules,
        ARRAY_COUNT(startToExperienceRules),
        &STATE_EXPERIENCE_MISSION
    },
    {
        ROUTE_EXPERIENCE_TO_MATERIAL_QUESTION,
        "Experience Room to Question Room",
        "Target:",
        "Question room",
        "Follow the route",
        experienceToMaterialQuestionRules,
        ARRAY_COUNT(experienceToMaterialQuestionRules),
        &STATE_MATERIAL_QUESTION
    },
    {
        ROUTE_MATERIAL_QUESTION_TO_COLLECTION,
        "Question Room to Material Room",
        "Target:",
        "Material room",
        "Follow the route",
        materialQuestionToCollectionRules,
        ARRAY_COUNT(materialQuestionToCollectionRules),
        &STATE_MATERIAL_COLLECTION
    },
    {
        ROUTE_COLLECTION_TO_BUILD_TEST,
        "Material Room to Final Room",
        "Target:",
        "Final room",
        "Follow the route",
        collectionToBuildTestRules,
        ARRAY_COUNT(collectionToBuildTestRules),
        &STATE_BUILD_TEST,
    }
};

const BeaconDefinition_t *routes_get_beacon_definition(BeaconId_t beaconId)
{
    for (uint8_t i = 0; i < ARRAY_COUNT(beaconDefinitions); i++)
    {
        if (beaconDefinitions[i].id == beaconId)
        {
            return &beaconDefinitions[i];
        }
    }

    return 0;
}

const BeaconDefinition_t *routes_find_beacon_by_major_minor(const char *major, const char *minor)
{
    if (major == 0 || minor == 0)
    {
        return 0;
    }

    for (uint8_t i = 0; i < ARRAY_COUNT(beaconDefinitions); i++)
    {
        if (strcmp(beaconDefinitions[i].major, major) == 0 &&
            strcmp(beaconDefinitions[i].minor, minor) == 0)
        {
            return &beaconDefinitions[i];
        }
    }

    return 0;
}

const RouteProfile_t *routes_get_profile(RouteId_t routeId)
{
    for (uint8_t i = 0; i < ARRAY_COUNT(routeProfiles); i++)
    {
        if (routeProfiles[i].id == routeId)
        {
            return &routeProfiles[i];
        }
    }

    return 0;
}

const RouteBeaconRule_t *routes_find_rule(const RouteProfile_t *profile, BeaconId_t beaconId)
{
    if (profile == 0)
    {
        return 0;
    }

    for (uint8_t i = 0; i < profile->ruleCount; i++)
    {
        if (profile->rules[i].beaconId == beaconId)
        {
            return &profile->rules[i];
        }
    }

    return 0;
}