#include "state_game_missions.h"

#include "state_navigation.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"

#include <stdio.h>

extern State_t STATE_NAVIGATION;
extern State_t STATE_BUILD_TEST;

extern char keypadFlag;
extern uint16_t pressedKey;

void material_collection_entry(StateMachine_t *sm)
{
    printf("ENTER MATERIAL_COLLECTION\r\n");

    oled_clear();
    oled_set_cursor(0, 0);
    oled_puts("MATERIAL");

    oled_set_cursor(0, 2);
    oled_puts("COLLECTION");

    oled_set_cursor(0, 5);
    oled_puts("ENTER = NEXT");

    sm->isBusy = true;
}

void material_collection_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (pressedKey == ENTER)
        {
            addToQueue(sm, &STATE_NAVIGATION);
            sm->isBusy = false;
        }
    }
}

void material_collection_exit(StateMachine_t *sm)
{
    oled_clear();
}