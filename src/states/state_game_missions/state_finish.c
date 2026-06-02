#include "state_game_missions.h"

#include "keypad.h"
#include "oled.h"

#include <stdio.h>

extern State_t STATE_BOOT_MENU;

extern char keypadFlag;
extern uint16_t pressedKey;

void finish_entry(StateMachine_t *sm)
{
    printf("ENTER FINISH\r\n");

    oled_clear();
    oled_set_cursor(0, 0);
    oled_puts("THANKS FOR");

    oled_set_cursor(0, 2);
    oled_puts("PLAYING");

    oled_set_cursor(0, 5);
    oled_puts("ENTER = MENU");

    /*
     * Later, the final box opening logic can be triggered here.
     * For now, this state only shows the final message.
     */

    sm->isBusy = true;
}

void finish_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (pressedKey == ENTER)
        {
            addToQueue(sm, &STATE_BOOT_MENU);
            sm->isBusy = false;
        }
    }
}

void finish_exit(StateMachine_t *sm)
{
    oled_clear();
}