#include "state_navigation.h"

#include "beacon_parser.h"
#include "hm10.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern State_t STATE_EXPERIENCE_MISSION;

extern char keypadFlag;
extern uint16_t pressedKey;

static RouteId_t activeRouteId = ROUTE_START_TO_EXPERIENCE;
static State_t *nextStateAfterNavigation = 0;

static const RouteProfile_t *activeProfile = 0;
static IbeaconData_t detectedBeacon;

static bool targetReached = false;

void navigation_set_route(RouteId_t routeId)
{
    activeRouteId = routeId;
}

void navigation_set_next_state(State_t *nextState)
{
    nextStateAfterNavigation = nextState;
}

static const char *role_to_text(BeaconRole_t role)
{
    switch (role)
    {
        case BEACON_ROLE_TARGET:
            return "TARGET";

        case BEACON_ROLE_PASS_BY:
            return "PASS BY";

        case BEACON_ROLE_APPROACH:
            return "APPROACH";

        case BEACON_ROLE_WRONG_WAY:
            return "WRONG WAY";

        case BEACON_ROLE_IGNORE:
            return "IGNORE";

        default:
            return "UNKNOWN";
    }
}

static int beacon_rssi_to_int(const char *rssiText)
{
    if (rssiText == 0)
    {
        return -999;
    }

    return atoi(rssiText);
}

static void show_game_message(const char *line1, const char *line2, const char *line3)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts(line1);

    oled_set_cursor(0, 2);
    oled_puts(line2);

    oled_set_cursor(0, 5);
    oled_puts(line3);
}

void navigation_entry(StateMachine_t *sm)
{
    activeProfile = routes_get_profile(activeRouteId);
    targetReached = false;

    if (nextStateAfterNavigation == 0)
    {
        nextStateAfterNavigation = &STATE_EXPERIENCE_MISSION;
    }

    printf("ENTER NAVIGATION\r\n");

    if (activeProfile != 0)
    {
        printf("Active route: %s\r\n", activeProfile->name);

        show_game_message(
            activeProfile->targetLine1,
            activeProfile->targetLine2,
            activeProfile->targetLine3
        );
    }
    else
    {
        printf("ERROR: Active route profile not found\r\n");
    }

    hm10_send_command("AT+DISI?");

    sm->isBusy = true;
}

void navigation_main(StateMachine_t *sm)
{
    static char raw_beacon_string[150];

    /*
      Phase 2:
      Target beacon was already found.
      Now wait for the player to confirm that they reached the room.
     */
    if (targetReached == true)
    {
        if (keypadFlag)
        {
            keypadFlag = false;

            if (pressedKey == ENTER)
            {
                printf("Room arrival confirmed by player\r\n");

                addToQueue(sm, nextStateAfterNavigation);

                nextStateAfterNavigation = 0;
                targetReached = false;
                sm->isBusy = false;
            }
        }

        return;
    }

    /*
      Phase 1:
      Read HM-10 beacon scan output.
     */
    if (hm10_read_line(raw_beacon_string, sizeof(raw_beacon_string)) == false)
    {
        return;
    }

    /*
     HM-10 sends OK+DISCE when discovery has ended.
     Restart scanning so navigation keeps looking for beacons.
     */
    if (strstr(raw_beacon_string, "OK+DISCE") != 0)
    {
        printf("HM-10 scan finished, restarting scan\r\n");

        hm10_send_command("AT+DISI?");
        memset(raw_beacon_string, 0, sizeof(raw_beacon_string));
        return;
    }

    /*
     Try to parse the HM-10 line as beacon data.
     If the line is not a beacon line, ignore it.
     */
    if (parse_beacon_string(raw_beacon_string, &detectedBeacon) == false)
    {
        memset(raw_beacon_string, 0, sizeof(raw_beacon_string));
        return;
    }

    printf("Beacon detected\r\n");
    printf("major: %s\r\n", detectedBeacon.major);
    printf("minor: %s\r\n", detectedBeacon.minor);
    printf("rssi : %s\r\n", detectedBeacon.rssi);

    /*
    Find which physical beacon this is.
     Unknown major/minor combinations are ignored.
     */
    const BeaconDefinition_t *beacon =
        routes_find_beacon_by_major_minor(detectedBeacon.major, detectedBeacon.minor);

    if (beacon == 0)
    {
        printf("Beacon is not registered, ignoring it\r\n");

        memset(raw_beacon_string, 0, sizeof(raw_beacon_string));
        return;
    }

    /*
      Find what this beacon means in the active route.
     */
    const RouteBeaconRule_t *rule =
        routes_find_rule(activeProfile, beacon->id);

    if (rule == 0)
    {
        printf("%s detected but not relevant for this route\r\n", beacon->label);

        memset(raw_beacon_string, 0, sizeof(raw_beacon_string));
        return;
    }

    int rssi = beacon_rssi_to_int(detectedBeacon.rssi);

    printf("Beacon: %s\r\n", beacon->label);
    printf("Role  : %s\r\n", role_to_text(rule->role));
    printf("RSSI  : %d, threshold: %d\r\n", rssi, rule->rssiThreshold);

    /*
      RSSI is negative.
      Example:
      -55 is stronger/closer than -75.
     
      If RSSI is weaker than the threshold, do nothing on the OLED.
     
     */
    if (rssi < rule->rssiThreshold)
    {
        printf("%s detected but signal is too weak\r\n", beacon->label);

        memset(raw_beacon_string, 0, sizeof(raw_beacon_string));
        return;
    }

    if (rule->role == BEACON_ROLE_IGNORE)
    {
        printf("Ignored beacon: %s\r\n", beacon->label);

        memset(raw_beacon_string, 0, sizeof(raw_beacon_string));
        return;
    }

    
    show_game_message(rule->line1, rule->line2, rule->line3);

    if (rule->role == BEACON_ROLE_TARGET)
    {
        printf("TARGET REACHED: %s\r\n", beacon->label);
        printf("Waiting for player confirmation...\r\n");

        targetReached = true;

        memset(raw_beacon_string, 0, sizeof(raw_beacon_string));
        return;
    }

    memset(raw_beacon_string, 0, sizeof(raw_beacon_string));
}

void navigation_exit(StateMachine_t *sm)
{
    printf("EXIT NAVIGATION\r\n");

    oled_clear();
}