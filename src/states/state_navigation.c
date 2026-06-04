#include "state_navigation.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"
#include "beacon_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hm10.h"

#define IMPROVEMENT_MARGIN 4
#define GETTING_FURTHER_THRESHOLD 12
#define WRONG_DIRECTION_THRESHOLD 20
#define FILTER_SIZE 5

extern State_t STATE_EXPERIENCE_MISSION;

extern char keypadFlag;
extern uint16_t pressedKey;

// this element will then be changed to another state
// via the navigation_set_route function
static RouteId_t activeRouteId = ROUTE_START_TO_EXPERIENCE;
static State_t *nextStateAfterNavigation = 0;

// flag indicating that targeted location is found
static bool targetLocationFound = false;

// average RSSI filter
static int rssi_buffer[FILTER_SIZE]; // this value equal to the average value of 5 current RSSI
static int rssi_index = 0;
static int rssi_count = 0;
static bool targetSeen = false;
static int bestRssi = -127;

void reset_rssi_filter()
{
    rssi_index = 0;
    rssi_count = 0;
    targetSeen = false;
    bestRssi = -127;
}

int get_averaged_rssi(int new_rssi)
{
    // push new data to the array
    rssi_buffer[rssi_index] = new_rssi;
    rssi_index = (rssi_index + 1) % FILTER_SIZE;

    if (rssi_count < FILTER_SIZE)
    {
        rssi_count++;
    }

    int sum = 0;
    for (int i = 0; i < rssi_count; i++)
    {
        sum += rssi_buffer[i];
    }
    return sum / rssi_count;
}

void navigation_set_route(RouteId_t routeId)
{
    activeRouteId = routeId;
}

void navigation_set_next_state(State_t *nextState)
{
    nextStateAfterNavigation = nextState;
}

void navigation_entry(StateMachine_t *sm)
{
    const RouteProfile_t *activeProfile = routes_get_profile(activeRouteId);

    // this function will be skipped on the next states
    if (nextStateAfterNavigation == 0)
    {
        nextStateAfterNavigation = &STATE_EXPERIENCE_MISSION;
    }

    targetLocationFound = false; // reset the flag to find new targeted beacon
    reset_rssi_filter();         // reset the current rssi values

    printf("NAVIGATION route: ");

    if (activeProfile != 0)
    {
        printf("%s\r\n", activeProfile->displayName);
    }
    else
    {
        printf("UNKNOWN\r\n");
    }

    oled_clear();
    oled_set_cursor(0, 0);
    oled_puts("NAVIGATION");

    oled_set_cursor(0, 2);

    if (activeProfile != 0)
    {
        oled_puts(activeProfile->displayName);
    }
    else
    {
        oled_puts("UNKNOWN ROUTE");
    }

    hm10_send_command("AT+DISI?");

    sm->isBusy = true;
}

void navigation_main(StateMachine_t *sm)
{
    static char raw_beacon_string[150];
    IbeaconData_t targetBeacon;

    // scanning for targerted beacon
    if (!targetLocationFound)
    {
        if (hm10_read_line(raw_beacon_string, 150) == true)
        {
            if (strstr(raw_beacon_string, "OK+DISCE") != NULL)
            {
                hm10_send_command("AT+DISI?");  
            }
            else if (parse_beacon_string(raw_beacon_string, &targetBeacon) == true)
            {
                if (strcmp(targetBeacon.factoryId, "4C000215") == 0)
                {
                    const BeaconDefinition_t *foundDef = routes_find_beacon_by_major_minor(targetBeacon.major, targetBeacon.minor);

                    if (foundDef != 0)
                    {
                        const RouteProfile_t *activeProfile = routes_get_profile(activeRouteId);
                        const RouteBeaconRule_t *rule = routes_find_rule(activeProfile, foundDef->id);

                        if (rule != 0)  
                        {
                            int raw_rssi = atoi(targetBeacon.rssi);
                            int targetAvgRssi = get_averaged_rssi(raw_rssi); 

                            // RULE 1: if the box reach to the targeted beacon
                            if (rule->role == BEACON_ROLE_TARGET)
                            {
                                // set the new data for RSSI
                                if (!targetSeen)
                                {
                                    bestRssi = targetAvgRssi;
                                    targetSeen = true;
                                }
                        
                                if (targetAvgRssi >= rule->rssiThreshold)
                                {
                                    printf("TARGET FOUND! Avg RSSI: %d\r\n", targetAvgRssi);
                                    targetLocationFound = true;

                                    oled_clear();
                                    oled_set_cursor(0, 0);
                                    oled_puts("LOCATION FOUND");

                                    //print this line abo
                                    printf("LOCATION FOUND");
                                    oled_set_cursor(0, 3);
                                    oled_puts("PRESS ENTER");
                                    oled_set_cursor(0, 5);
                                    oled_puts("TO CONFIRM");
                                }
                                // if the box hasn't reached to the targeted beacon
                                else
                                {
                                    int dropFromBest = bestRssi - targetAvgRssi;

                                    oled_clear();
                                    oled_set_cursor(0, 0);
                                    oled_puts("NAVIGATING...");

                                    oled_set_cursor(0, 2);
                                    if (dropFromBest >= WRONG_DIRECTION_THRESHOLD)
                                    {
                                        oled_puts("WRONG DIRECTION");
                                    }
                                    else if (dropFromBest >= GETTING_FURTHER_THRESHOLD)
                                    {
                                        oled_puts("GETTING FURTHER");
                                    }
                                    else if (targetAvgRssi > bestRssi + IMPROVEMENT_MARGIN)
                                    {
                                        bestRssi = targetAvgRssi; 
                                        oled_puts("GETTING CLOSER");
                                    }
                                    else
                                    {                                    
                                        oled_puts("KEEP GOING");
                                    }
                                 
                                    char debugStr[20];
                                    sprintf(debugStr, "Avg RSSI: %d", targetAvgRssi);
                                    oled_set_cursor(0, 5);
                                    oled_puts(debugStr);
                                }
                            }
                            // RULE 2: if the box only pass the PASS_BY beacon
                            else if (rule->role == BEACON_ROLE_PASS_BY && targetAvgRssi >= rule->rssiThreshold)
                            {
                                oled_clear();
                                oled_set_cursor(0, 0);
                                oled_puts("NAVIGATING...");
                                oled_set_cursor(0, 2);
                                oled_puts("ON ROUTE");
                                oled_set_cursor(0, 5);
                                oled_puts("Keep walking!");
                            }
                        }
                    }
                }
            }
            memset(raw_beacon_string, 0, sizeof(raw_beacon_string));
        }
    }

    // confirmation step when reach the targeted beacon
    else
    {
        if (keypadFlag)
        {
            keypadFlag = false;
            if (pressedKey == ENTER)
            {
                printf("Arrival Confirmed. Transitioning to Puzzle state\r\n");
                addToQueue(sm, nextStateAfterNavigation);
                nextStateAfterNavigation = 0;
                sm->isBusy = false;
            }
        }
    }
}

void navigation_exit(StateMachine_t *sm)
{
    printf("STOP NAVIGATION\r\n");
    hm10_send_command("AT+SLEEP");
    oled_clear();
}