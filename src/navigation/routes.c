#include "routes.h"
#include <string.h>

#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

/*
  Route and beacon configuration
 
  This file separates two concepts:
 
  1. Physical beacon identity
  2. Beacon meaning inside a route
 
  A physical beacon is identified by its iBeacon major/minor values.
  In the code, we give each physical beacon a stable internal ID:
 
      BEACON_B01
      BEACON_B02
      BEACON_B03
 
  Example:
 
      BEACON_B01 = major 0B01, minor 0003
 
  These values come from the actual beacon configuration or from
  scanning the beacon with the HM-10 module.
 
  A route profile does not define what a beacon physically is.
  Instead, it defines what that beacon means during one specific route.
 
  Example:

      In route A:
          B01 = TARGET
          B02 = PASS_BY
 
      In route B:
          B01 = PASS_BY
          B02 = TARGET
 
  
  The same physical beacon can have a different role depending on the
  active route.
 */



/*
  Beacon registry
 
 
  This table defines the physical beacons.
 
  The label B01/B02/B03 is our internal name.
  The major/minor values are the real iBeacon identifiers.
 
  If the beacon configuration changes, update the major/minor values here.
  The route profiles can stay the same as long as B01, B02, B03 still
  represent the same physical locations.
 */
static const BeaconDefinition_t beaconDefinitions[] = {
    {BEACON_B01, "0B01", "0003", "B01"},
    {BEACON_B02, "0AEA", "0037", "B02"},
    {BEACON_B03, "0AEA", "0032", "B03"},

    /*
      Placeholder beacons.
     
     */
    {BEACON_B04, "0000", "0000", "B04"},
    {BEACON_B05, "0000", "0000", "B05"},
    {BEACON_B06, "0000", "0000", "B06"},
    {BEACON_B07, "0000", "0000", "B07"},
    {BEACON_B08, "0000", "0000", "B08"},
    {BEACON_B09, "0000", "0000", "B09"},
    {BEACON_B10, "0000", "0000", "B10"}
};

/*
  Route rules
 
 
  A route rule defines what a beacon means in a specific route.
 
  beaconId:
      Which physical beacon this rule is about.
 
  role:
      What the beacon means during this route.
 
  rssiThreshold:
      How strong the signal should be before we consider this rule relevant.
 
  RSSI is normally negative:
 
      -90 = weak / far
      -70 = closer
      -55 = strong / near
 
  These threshold values are starting values and should be tested in the
  real environment.
 */

static const RouteBeaconRule_t startToExperienceRules[] = {
    {BEACON_B01, BEACON_ROLE_TARGET, -65},
    {BEACON_B02, BEACON_ROLE_IGNORE, -75},
    {BEACON_B03, BEACON_ROLE_IGNORE, -75}
};

static const RouteBeaconRule_t experienceToMaterialQuestionRules[] = {
    {BEACON_B01, BEACON_ROLE_PASS_BY, -75},
    {BEACON_B02, BEACON_ROLE_TARGET, -65},
    {BEACON_B03, BEACON_ROLE_WRONG_WAY, -75}
};

static const RouteBeaconRule_t materialQuestionToCollectionRules[] = {
    {BEACON_B01, BEACON_ROLE_IGNORE, -75},
    {BEACON_B02, BEACON_ROLE_PASS_BY, -75},
    {BEACON_B03, BEACON_ROLE_TARGET, -65}
};

static const RouteBeaconRule_t collectionToBuildTestRules[] = {
    {BEACON_B01, BEACON_ROLE_WRONG_WAY, -75},
    {BEACON_B02, BEACON_ROLE_PASS_BY, -75},
    {BEACON_B03, BEACON_ROLE_TARGET, -65}
};

static const RouteBeaconRule_t buildTestToFinishRules[] = {
    {BEACON_B01, BEACON_ROLE_IGNORE, -75},
    {BEACON_B02, BEACON_ROLE_IGNORE, -75},
    {BEACON_B03, BEACON_ROLE_TARGET, -65}
};

/*
 Route profiles
 
  A RouteProfile groups the rules for one route.
 
  The navigation state will use the active RouteProfile to interpret
  detected beacons.
 
  The navigation state should not hardcode beacon meanings.
  It should ask this module:
 
      "For the active route, what does this beacon mean?"
 */
static const RouteProfile_t routeProfiles[] = {
    {
        ROUTE_START_TO_EXPERIENCE,
        "Start to Experience",
        startToExperienceRules,
        ARRAY_COUNT(startToExperienceRules)
    },
    {
        ROUTE_EXPERIENCE_TO_MATERIAL_QUESTION,
        "Experience to Material Question",
        experienceToMaterialQuestionRules,
        ARRAY_COUNT(experienceToMaterialQuestionRules)
    },
    {
        ROUTE_MATERIAL_QUESTION_TO_COLLECTION,
        "Material Question to Collection",
        materialQuestionToCollectionRules,
        ARRAY_COUNT(materialQuestionToCollectionRules)
    },
    {
        ROUTE_COLLECTION_TO_BUILD_TEST,
        "Collection to Build Test",
        collectionToBuildTestRules,
        ARRAY_COUNT(collectionToBuildTestRules)
    },
    {
        ROUTE_BUILD_TEST_TO_FINISH,
        "Build Test to Finish",
        buildTestToFinishRules,
        ARRAY_COUNT(buildTestToFinishRules)
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