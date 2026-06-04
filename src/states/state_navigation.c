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
extern State_t STATE_BOOT_MENU;
extern bool hm10_isBusy;

extern char keypadFlag;
extern uint16_t pressedKey;

extern const RouteProfile_t routeProfiles[];
extern BeaconDefinition_t *recentBeacons[3];
extern BeaconDefinition_t beaconDefinitions[];
extern const char beaconDefCount;
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

void navigation_entry(StateMachine_t *sm){
    checkbeacon_timeout = ms;
}

void navigation_main(StateMachine_t *sm)
{
    // static char raw_beacon_string[150];

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

                if(activeRouteId+1 <= ROUTE_END)
                    addToQueue(sm, routeProfiles[activeRouteId+1].puzzleState);
                else {
                    addToQueue(sm, &STATE_BOOT_MENU);
                    activeRouteId = ROUTE_START_TO_EXPERIENCE;
                }

                targetReached = false;
                sm->isBusy = false;
            }
        }

        return;
    }

    if(hm10_isBusy == false){
        hm10_send_command("AT+DISI?\r\n");
        printf("AT+DISI?\r\n");
        hm10_isBusy = true;
        timeout_ms = ms;
    } else {
        hm10_read_beacons();
    }

    if(ms - timeout_ms == 5000){
        hm10_isBusy = false;
    }

    // apply the beacon rules
    if(ms - checkbeacon_timeout > 1000){
        checkbeacon_timeout = ms;
        for(int x = 0; x < 3; x++){

            if(strcmp(recentBeacons[x]->minor, "H") == 0){
                // breakpoint
                continue;
            }
            const RouteBeaconRule_t *rule = routes_find_rule(
                &routeProfiles[activeRouteId],
                recentBeacons[x]->major,
                recentBeacons[x]->minor
            );

            //calc avarage rssi
            int averageRssi = 0;
            for (int r = 0; r < recentBeacons[x]->rssiSize; r++) {
                averageRssi += recentBeacons[x]->rssi[r];
            }
            averageRssi = averageRssi/recentBeacons[x]->rssiSize;
            // printf("average rssi: %d on beacon: %s\r\n", averageRssi, recentBeacons[x]->minor);

            // printf("rule beaconid: %d", rule->beaconId);
            if (averageRssi > rule->rssiThreshold) {
                show_game_message(rule->line1, rule->line2, rule->line3);
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