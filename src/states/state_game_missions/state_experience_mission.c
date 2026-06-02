#include "state_game_missions.h"

#include "state_navigation.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"

#include <stdio.h>
#include <stdint.h>

extern State_t STATE_NAVIGATION;
extern State_t STATE_MATERIAL_QUESTION;

extern char keypadFlag;
extern uint16_t pressedKey;

static char answerBuffer[2];
static uint8_t answerLength = 0;

static void show_experience_question(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("QUESTION");

    oled_set_cursor(0, 2);
    oled_puts("What is 3x3?");

    oled_set_cursor(0, 4);
    oled_puts("Ans:");
    oled_puts(answerBuffer);
    oled_puts("_");

    oled_set_cursor(0, 6);
    oled_puts("ENTER submit");
}

void experience_entry(StateMachine_t *sm)
{
    printf("ENTER EXPERIENCE_MISSION\r\n");

    answerLength = 0;
    answerBuffer[0] = '\0';

    show_experience_question();

    sm->isBusy = true;
}

void experience_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (pressedKey == NINE)
        {
            answerBuffer[0] = '9';
            answerBuffer[1] = '\0';
            answerLength = 1;

            show_experience_question();
        }
        else if (pressedKey == ENTER)
        {
            if (answerLength == 1 && answerBuffer[0] == '9')
            {
                oled_clear();

                oled_set_cursor(0, 0);
                oled_puts("CORRECT!");

                oled_set_cursor(0, 2);
                oled_puts("Next route");

                navigation_set_route(ROUTE_EXPERIENCE_TO_MATERIAL_QUESTION);
                navigation_set_next_state(&STATE_MATERIAL_QUESTION);

                addToQueue(sm, &STATE_NAVIGATION);
                sm->isBusy = false;
            }
            else
            {
                answerLength = 0;
                answerBuffer[0] = '\0';

                oled_clear();

                oled_set_cursor(0, 0);
                oled_puts("WRONG");

                oled_set_cursor(0, 2);
                oled_puts("Try again");

                for (volatile int i = 0; i < 1000000; i++)
                {
                }

                show_experience_question();
            }
        }
    }
}

void experience_exit(StateMachine_t *sm)
{
    oled_clear();
}