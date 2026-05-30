#include "state_game_missions.h"

#include "state_navigation.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"

#include <stdio.h>

extern State_t STATE_NAVIGATION;
extern State_t STATE_FINISH;

extern char keypadFlag;
extern uint16_t pressedKey;

void build_test_entry(StateMachine_t *sm)
{
    printf("ENTER BUILD_TEST\r\n");

    oled_clear();
    oled_set_cursor(0, 0);
    oled_puts("BUILD");

    oled_set_cursor(0, 2);
    oled_puts("AND TEST");

    oled_set_cursor(0, 5);
    oled_puts("ENTER = NEXT");

    sm->isBusy = true;
}

void build_test_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (pressedKey == ENTER)
        {
            navigation_set_route(ROUTE_BUILD_TEST_TO_FINISH);
            navigation_set_next_state(&STATE_FINISH);

            addToQueue(sm, &STATE_NAVIGATION);
            sm->isBusy = false;
        }
    }
}

void build_test_exit(StateMachine_t *sm)
{
    oled_clear();
}