#include "state_game_missions.h"

#include "state_navigation.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"

#include <stdio.h>

extern State_t STATE_NAVIGATION;
extern State_t STATE_MATERIAL_COLLECTION;

extern char keypadFlag;
extern uint16_t pressedKey;

void material_question_entry(StateMachine_t *sm)
{
    printf("ENTER MATERIAL_QUESTION\r\n");

    oled_clear();
    oled_set_cursor(0, 0);
    oled_puts("MATERIAL");

    oled_set_cursor(0, 2);
    oled_puts("QUESTION");

    oled_set_cursor(0, 5);
    oled_puts("ENTER = NEXT");

    sm->isBusy = true;
}

void material_question_main(StateMachine_t *sm)
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

void material_question_exit(StateMachine_t *sm)
{
    oled_clear();
}