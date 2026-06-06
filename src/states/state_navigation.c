#include "state_navigation.h"

#include "beacon_parser.h"
#include "hm10.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"
#include "game_logger.h"
#include "lpuart2.h"

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

static uint32_t cooldownTimer;
static bool cooldownTimerTriggered;

static RouteId_t activeRouteId = ROUTE_START_TO_EXPERIENCE;
static volatile uint32_t timeout_ms = 0;
static uint32_t checkbeacon_timeout = 0;

static int routeProgressIndex = 0;
static const RouteBeaconRule_t *lastShownRule = 0;

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

static void show_rule_message_once(const RouteBeaconRule_t *rule)
{
    if (rule == lastShownRule)
    {
        return;
    }

    lastShownRule = rule;
    show_game_message(rule->line1, rule->line2, rule->line3);
}

void navigation_entry(StateMachine_t *sm){
    checkbeacon_timeout = ms;
    routeProgressIndex = 0;
    lastShownRule = 0;

    keypadFlag = false;
    cooldownTimerTriggered = false;

    show_game_message(
        (char *)routeProfiles[activeRouteId].targetLine1,
        (char *)routeProfiles[activeRouteId].targetLine2,
        (char *)routeProfiles[activeRouteId].targetLine3
    );
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
        while(lpuart2_rxcnt()){
            lpuart2_getchar();
        }

        if(cooldownTimerTriggered == false){
            cooldownTimer = ms;
            cooldownTimerTriggered = true;
        }

        if (keypadFlag && ((ms - cooldownTimer) > 5000))
        {
            cooldownTimerTriggered = false;
            keypadFlag = false;

            if (pressedKey == ENTER)
            {
                printf("Room arrival confirmed by player\r\n");

                if(activeRouteId < (ROUTE_END)){
                    Logger_Record_Time(activeRouteId, puzzle_seconds_counter);
                    addToQueue(sm, routeProfiles[activeRouteId].puzzleState);
                    activeRouteId++;

                    // reset the beacon rssis
                    for(int x = 0; x < beaconDefCount; x++){
                        for(int y = 0; y < beaconDefinitions[x].rssiSize; y++){
                            beaconDefinitions[x].rssi[y] = -86;
                        }
                    }
                }
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
    
    if(hm10_isBusy == false && targetReached == false){
        hm10_send_command("AT+DISI?\r\n");
        printf("AT+DISI?\r\n");
        hm10_isBusy = true;
        timeout_ms = ms;
    }

    if(hm10_isBusy == true){
        hm10_read_beacons();
    }

    if(ms - timeout_ms >= 6000){
        hm10_isBusy = false;
    }

    // return when target is reached so we don't do excess code
    if(targetReached)
        return;

    // apply the beacon rules
    if(ms - checkbeacon_timeout > 1000){
        checkbeacon_timeout = ms;
        for(int x = 0; x < 3; x++){

            if (recentBeacons[x] == 0)
            {
            continue;
            }

            if(strcmp(recentBeacons[x]->minor, "H") == 0){
                continue;
            }
            const RouteBeaconRule_t *rule = routes_find_rule(
                &routeProfiles[activeRouteId],
                recentBeacons[x]->major,
                recentBeacons[x]->minor
            );

            if (rule == 0)
            {
             continue;
            }              
            
            int ruleIndex = rule - routeProfiles[activeRouteId].rules;

           if (ruleIndex < routeProgressIndex)
             {
              continue;
             }
            //calc avarage rssi
            int averageRssi = 0;
            for (int r = 0; r < recentBeacons[x]->rssiSize; r++) {
                averageRssi += recentBeacons[x]->rssi[r];
            }
            averageRssi = averageRssi/recentBeacons[x]->rssiSize;
            //printf("average rssi: %d on beacon: %s\r\n", averageRssi, recentBeacons[x]->minor);

            // printf("rule beaconid: %d", rule->beaconId);
            if (averageRssi > rule->rssiThreshold) {
                routeProgressIndex = ruleIndex;

                show_rule_message_once(rule);
                if (rule->role == BEACON_ROLE_TARGET)
{
                targetReached = true;
                keypadFlag = false;
                cooldownTimerTriggered = false;
}
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