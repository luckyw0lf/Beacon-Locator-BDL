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
BeaconDefinition_t beaconDefinitions[] = {
    {BEACON_B01, "0B01", "0001", "B01", {-86, -86, -86}, 3, 0},
    {BEACON_B02, "0B01", "0002", "B02", {-86, -86, -86}, 3, 0},
    {BEACON_B03, "0B01", "0003", "B03", {-86, -86, -86}, 3, 0},
    {BEACON_B04, "0B01", "0004", "B04", {-86, -86, -86}, 3, 0},
    {BEACON_B05, "0B01", "0005", "B05", {-86, -86, -86}, 3, 0},
    {BEACON_B06, "0B01", "0006", "B06", {-86, -86, -86}, 3, 0},
    {BEACON_B07, "0B01", "0007", "B07", {-86, -86, -86}, 3, 0},
    {BEACON_B08, "0B01", "0008", "B08", {-86, -86, -86}, 3, 0},
    {BEACON_B09, "0B01", "0009", "B09", {-86, -86, -86}, 3, 0},
    {BEACON_B10, "0B01", "000A", "B10", {-86, -86, -86}, 3, 0}
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
    {BEACON_B01, BEACON_ROLE_PASS_BY,  -75, "Signal gevonden", "Ga door", ""},
    {BEACON_B02, BEACON_ROLE_PASS_BY,  -75, "Goede richting", "Ga door", ""},
    {BEACON_B03, BEACON_ROLE_PASS_BY,  -75, "Goede richting", "Ga door", ""},
    {BEACON_B04, BEACON_ROLE_PASS_BY,  -70, "Sla linksaf", "Vanaf de gang", ""},
    {BEACON_B05, BEACON_ROLE_APPROACH, -70, "Je bent dichtbij", "Ga door", ""},
    {BEACON_B06, BEACON_ROLE_TARGET,   -65, "Ervaringskamer", "Bereikt", "Druk ENTER"}
};

/*
 * Route 2:
 * From room 0006 -> target 0003
 *
 * 0005 and 0004 are on the way back.
 * 0003 is the target room.
 */
const RouteBeaconRule_t experienceToMaterialQuestionRules[] = {
    {BEACON_B05, BEACON_ROLE_PASS_BY,  -75, "Goede richting", "Volg de gang", ""},
    {BEACON_B04, BEACON_ROLE_APPROACH, -75, "Je bent dichtbij", "Zoek de kamer", "aan je rechterkant"},
    {BEACON_B03, BEACON_ROLE_TARGET,   -75, "Vraag kamer", "Bereikt", "Druk ENTER"}
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
    {BEACON_B04, BEACON_ROLE_PASS_BY, -75, "Goede richting", "Ga door", ""},
    {BEACON_B07, BEACON_ROLE_PASS_BY, -75, "Ga door", "Volg de route", ""},
    {BEACON_B08, BEACON_ROLE_PASS_BY, -70, "Sla linksaf", "Vanaf de gang", ""},
    {BEACON_B09, BEACON_ROLE_PASS_BY, -70, "Goed bezig", "Ga door", ""},
    {BEACON_B10, BEACON_ROLE_TARGET,  -70, "Materiaal kamer", "Bereikt", "Druk ENTER"}
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
    {BEACON_B09, BEACON_ROLE_PASS_BY, -75, "Goed bezig", "Zoek verder", ""},
    {BEACON_B07, BEACON_ROLE_PASS_BY, -75, "Je bent dichtbij", "Zoek de kamer", ""},
    {BEACON_B08, BEACON_ROLE_TARGET,  -70, "Test kamer", "Bereikt", "Druk ENTER"}
};

const RouteProfile_t routeProfiles[] = {
    {
        ROUTE_START_TO_EXPERIENCE,
        "Start naar Ervaringskamer",
        "Doel:",
        "Ervaringskamer",
        "Volg de route",
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

BeaconDefinition_t *routes_find_beacon_by_major_minor(const char *major, const char *minor)
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

const RouteBeaconRule_t *routes_find_rule(const RouteProfile_t *profile, const char *major, const char *minor)
{
    if (profile == 0)
    {
        return 0;
    }

    BeaconDefinition_t *beacon = routes_find_beacon_by_major_minor(major, minor);

     if (beacon == 0)
    {
        return 0;
    }

    for (uint8_t i = 0; i < profile->ruleCount; i++)
    {
        if(profile->rules[i].beaconId == beacon->id){
            return &profile->rules[i];
        }
    }

    return 0;
}