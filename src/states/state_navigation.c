#include "state_navigation.h"

#include "beacon_parser.h"
#include "hm10.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"
#include "game_logger.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern State_t STATE_EXPERIENCE_MISSION;
extern State_t STATE_BOOT_MENU;
extern bool hm10_isBusy;

extern char keypadFlag;
extern uint16_t pressedKey;

extern const RouteProfile_t routeProfiles[];
extern BeaconDefinition_t *recentBeacons[3];
extern BeaconDefinition_t beaconDefinitions[];
extern const char beaconDefCount;

extern volatile uint32_t puzzle_seconds_counter;
extern volatile uint32_t ms;

static RouteId_t activeRouteId = ROUTE_START_TO_EXPERIENCE;
static volatile uint32_t timeout_ms = 0;
static uint32_t checkbeacon_timeout = 0;

static bool targetReached = false;

static void show_game_message(char *line1, char *line2, char *line3)
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
    checkbeacon_timeout = ms;
}

static int compare(const void *a, const void *b)
{
    BeaconDefinition_t *beaconA = *(BeaconDefinition_t **)a;
    BeaconDefinition_t *beaconB = *(BeaconDefinition_t **)b;

    // Sort descending: larger values (closer to 0) come first
    if (beaconA->averageRssi < beaconB->averageRssi) return 1;
    if (beaconA->averageRssi > beaconB->averageRssi) return -1;
    
    return 0;
}

void navigation_main(StateMachine_t *sm)
{
    // static char raw_beacon_string[150];

    /*
      Phase 2:
      Target beacon was already found.
      Now wait for the player to confirm that they reached the room.
     */
    if (targetReached == true && hm10_isBusy == false)
    {
        if (keypadFlag)
        {
            keypadFlag = false;

            if (pressedKey == ENTER)
            {
                printf("Room arrival confirmed by player\r\n");

                if (activeRouteId + 1 <= ROUTE_END)
                {
                    Logger_Record_Time(activeRouteId, puzzle_seconds_counter);
                    addToQueue(sm, routeProfiles[activeRouteId + 1].puzzleState);
                }
                else
                {
                    addToQueue(sm, &STATE_BOOT_MENU);
                    activeRouteId = ROUTE_START_TO_EXPERIENCE;
                }

                targetReached = false;
                sm->isBusy = false;
            }
        }

        return;
    }

    if (hm10_isBusy == false && targetReached == false)
    {
        hm10_send_command("AT+DISI?\r\n");
        printf("AT+DISI?\r\n");
        hm10_isBusy = true;
        timeout_ms = ms;
    }

    if (hm10_isBusy == true)
    {
        hm10_read_beacons();
    }

    if (ms - timeout_ms == 8000)
    {
        hm10_isBusy = false;
    }

    // apply the beacon rules
    if (ms - checkbeacon_timeout > 1000)
    {
        checkbeacon_timeout = ms;
        BeaconDefinition_t *sortedRecentBeacons[3];
        memcpy(sortedRecentBeacons, recentBeacons, sizeof(recentBeacons));

        // 2. Calculate average RSSI for each beacon
        for (int x = 0; x < 3; x++)
        {
            // Defensive check: Avoid crashing with an accidental division by zero
            if (sortedRecentBeacons[x]->rssiSize == 0)
            {
                sortedRecentBeacons[x]->averageRssi = 0; // or a default low value like -100
                continue;
            }

            int totalRssi = 0;
            for (int r = 0; r < sortedRecentBeacons[x]->rssiSize; r++)
            {
                totalRssi += sortedRecentBeacons[x]->rssi[r];
            }

            // Calculate and assign the actual average to the struct
            sortedRecentBeacons[x]->averageRssi = totalRssi / sortedRecentBeacons[x]->rssiSize;

            // Using %s for minor assumes it's a string/char array. If it's an integer, change %s to %d
            printf("Average RSSI: %d on beacon minor: %s\r\n",
                   sortedRecentBeacons[x]->averageRssi,
                   sortedRecentBeacons[x]->minor);
        }

        // 3. Sort the array of pointers using your updated compare function
        qsort(sortedRecentBeacons,
              sizeof(sortedRecentBeacons) / sizeof(sortedRecentBeacons[0]),
              sizeof(sortedRecentBeacons[0]),
              compare);
        for (int x = 0; x < 3; x++)
        {
            if (strcmp(sortedRecentBeacons[x]->minor, "H") == 0)
            {
                continue;
            }
            const RouteBeaconRule_t *rule = routes_find_rule(
                &routeProfiles[activeRouteId],
                sortedRecentBeacons[x]->major,
                sortedRecentBeacons[x]->minor);

            // printf("average rssi: %d on beacon: %s\r\n", averageRssi, sortedRecentBeacons[x]->minor);

            // printf("rule beaconid: %d", rule->beaconId);
            if (sortedRecentBeacons[x]->averageRssi > rule->rssiThreshold)
            {
                show_game_message(rule->line1, rule->line2, rule->line3);

                // if(rule->role == BEACON_ROLE_TARGET)
                //     targetReached = true;
                break;
            }
        }
    }
}

void navigation_exit(StateMachine_t *sm)
{
    printf("EXIT NAVIGATION\r\n");

    oled_clear();
}