#include "state_game_missions.h"

#include "state_navigation.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"
#include "game_logger.h"

#include <stdio.h>


extern State_t STATE_NAVIGATION;
extern State_t STATE_MATERIAL_QUESTION;

extern char keypadFlag;
extern uint16_t pressedKey;

extern volatile uint32_t puzzle_seconds_counter;

void experience_entry(StateMachine_t *sm)
{
    printf("ENTER EXPERIENCE_MISSION\r\n");

    oled_clear();
    oled_set_cursor(0, 0);
    oled_puts("EXPERIENCE");

    oled_set_cursor(0, 2);
    oled_puts("MISSION");

    oled_set_cursor(0, 5);
    oled_puts("ENTER = NEXT");

    sm->isBusy = true;
}

void experience_main(StateMachine_t *sm)
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

void experience_exit(StateMachine_t *sm)
{
    oled_clear();
}

