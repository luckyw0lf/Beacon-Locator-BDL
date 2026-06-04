#include "routes.h"
#include <string.h>
#include <stdio.h>
#include "ff.h"

#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

static char set_majors[10][6] = {"", "", "", "", "", "", "", "", "", ""};
static char set_minors[10][6] = {"", "", "", "", "", "", "", "", "", ""};
static char set_labels[10][6] = {"B01", "B02", "B03", "B04", "B05", "B06", "B07", "B08", "B09", "B10"};

static BeaconDefinition_t beaconDefinitions[] = {
    {BEACON_B01, set_majors[0], set_minors[0], set_labels[0]},
    {BEACON_B02, set_majors[0], set_minors[0], set_labels[0]},
    {BEACON_B03, set_majors[0], set_minors[0], set_labels[0]},
    {BEACON_B04, set_majors[0], set_minors[0], set_labels[0]},
    {BEACON_B05, set_majors[0], set_minors[0], set_labels[0]},
    {BEACON_B06, set_majors[0], set_minors[0], set_labels[0]},
    {BEACON_B07, set_majors[0], set_minors[0], set_labels[0]},
    {BEACON_B08, set_majors[0], set_minors[0], set_labels[0]},
    {BEACON_B09, set_majors[0], set_minors[0], set_labels[0]},
    {BEACON_B10, set_majors[0], set_minors[0], set_labels[0]},
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

void routes_update_beacon(const char* label, const char* major, const char* minor)
{
    for (int i = 0; i < 10; i++)
    {
        if (strcmp(set_labels[i], label) == 0) 
        {
            strcpy(set_majors[i], major);
            strcpy(set_minors[i], minor);
            break; 
        }
    }
}
//checking if the beacon addresses are pushed to the box
void routes_dump_config(void)
{
    printf("check address\r\n");
    for (int i = 0; i < 10; i++)
    {
        printf("Slot %d - %s | Maj: %s | Min: %s\r\n", 
               i, set_labels[i], set_majors[i], set_minors[i]);
    }
}

void routes_save_config(void)
{
    FIL cfg_file;
   if (f_open(&cfg_file, "config.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK)
    {
        for (int i = 0; i < 10; i++) 
        {
            f_printf(&cfg_file, "%s|%s|%s\n", set_labels[i], set_majors[i], set_minors[i]);
        }
        f_close(&cfg_file);
    } 
}

void routes_load_config(void)
{
    FIL cfg_file;
    char line_buffer[50];

    if (f_open(&cfg_file, "config.txt", FA_READ) == FR_OK) 
    {
        int i = 0;
        while (f_gets(line_buffer, sizeof(line_buffer), &cfg_file) != NULL && i < 10) 
        {
            char label[10] = {0}, major[10] = {0}, minor[10] = {0};
            if (sscanf(line_buffer, "%[^|]|%[^|]|%s", label, major, minor) >= 2) 
            {
                strcpy(set_majors[i], major);
                strcpy(set_minors[i], minor);
            }
            i++;
        }
        f_close(&cfg_file);
    } 
}