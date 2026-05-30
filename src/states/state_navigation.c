#include "state_navigation.h"

#include "keypad.h"
#include "oled.h"
#include "routes.h"

#include <stdio.h>

extern State_t STATE_EXPERIENCE_MISSION;

extern char keypadFlag;
extern uint16_t pressedKey;

static RouteId_t activeRouteId = ROUTE_START_TO_EXPERIENCE;
static State_t *nextStateAfterNavigation = 0;

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

    if (nextStateAfterNavigation == 0)
    {
        nextStateAfterNavigation = &STATE_EXPERIENCE_MISSION;
    }

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

    oled_set_cursor(0, 5);
    oled_puts("ENTER = ARRIVED");

    sm->isBusy = true;
}

void navigation_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (pressedKey == ENTER)
        {
            addToQueue(sm, nextStateAfterNavigation);

            nextStateAfterNavigation = 0;
            sm->isBusy = false;
        }
    }
}

void navigation_exit(StateMachine_t *sm)
{
    oled_clear();
}