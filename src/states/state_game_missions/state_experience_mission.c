#include "state_game_missions.h"

#include "state_navigation.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"
#include "game_logger.h"

#include <stdio.h>
#include <stdint.h>

extern State_t STATE_NAVIGATION;
extern State_t STATE_MATERIAL_QUESTION;

extern char keypadFlag;
extern uint16_t pressedKey;

typedef enum
{
    EXPERIENCE_PAGE_INTRO_1,
    EXPERIENCE_PAGE_INTRO_2,
    EXPERIENCE_PAGE_QUESTION
} ExperiencePage_t;

static ExperiencePage_t currentPage = EXPERIENCE_PAGE_INTRO_1;
static uint8_t selectedAnswer = 0;

/*
    selectedAnswer:
    0 = Sensor
    1 = Air valve / Valve
    2 = Display
    3 = Battery
*/

static void delay_short(void)
{
    for (volatile int i = 0; i < 5000000; i++)
    {
    }
}

static void show_intro_page_1(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Missie:");

    oled_set_cursor(0, 2);
    oled_puts("Bouw een");

    oled_set_cursor(0, 3);
    oled_puts("luchtventiel");

    oled_set_cursor(0, 6);
    oled_puts("ENTER");
}

static void show_intro_page_2(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Vind delen");

    oled_set_cursor(0, 2);
    oled_puts("Zet ze samen");

    oled_set_cursor(0, 4);
    oled_puts("Test daarna");

    oled_set_cursor(0, 6);
    oled_puts("ENTER");
}

static void show_question_page(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Wat bouw je?");

    oled_set_cursor(0, 2);

    if (selectedAnswer == 0)
    {
        oled_puts(">1 Sensor 2 Vent.");
    }
    else if (selectedAnswer == 1)
    {
        oled_puts(" 1 Sensor >2 Vent.");
    }
    else
    {
        oled_puts(" 1 Sensor  2 Vent");
    }

    oled_set_cursor(0, 4);

    if (selectedAnswer == 2)
    {
        oled_puts(">3 Scherm  4 Accu");
    }
    else if (selectedAnswer == 3)
    {
        oled_puts(" 3 Scherm >4 Accu");
    }
    else
    {
        oled_puts(" 3 Scherm 4 Accu");
    }

    oled_set_cursor(0, 6);
    oled_puts("ENTER kies");
}

static void show_wrong_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Fout antwoord");

    oled_set_cursor(0, 2);
    oled_puts("Probeer weer");
}

static void show_correct_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Goed!");

    oled_set_cursor(0, 2);
    oled_puts("luchtventiel");

    oled_set_cursor(0, 5);
    oled_puts("Volgende route");
}

static void go_to_next_route(StateMachine_t *sm)
{
    navigation_set_route(ROUTE_EXPERIENCE_TO_MATERIAL_QUESTION);
    navigation_set_next_state(&STATE_MATERIAL_QUESTION);

    addToQueue(sm, &STATE_NAVIGATION);
    sm->isBusy = false;
}
extern volatile uint32_t puzzle_seconds_counter;

void experience_entry(StateMachine_t *sm)
{
    printf("ENTER EXPERIENCE_MISSION\r\n");

    currentPage = EXPERIENCE_PAGE_INTRO_1;
    selectedAnswer = 0;

    show_intro_page_1();

    sm->isBusy = true;
}

void experience_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (currentPage == EXPERIENCE_PAGE_INTRO_1)
        {
            if (pressedKey == ENTER)
            {
                printf("PRESSED ENTER.");
                currentPage = EXPERIENCE_PAGE_INTRO_2;
                show_intro_page_2();
            }

            return;
        }

        if (currentPage == EXPERIENCE_PAGE_INTRO_2)
        {
            if (pressedKey == ENTER)
            {
                printf("PRESSED ENTER.");
                currentPage = EXPERIENCE_PAGE_QUESTION;
                selectedAnswer = 0;
                show_question_page();
            }

            return;
        }

        if (currentPage == EXPERIENCE_PAGE_QUESTION)
        {
            if (pressedKey == RIGHT)
            {
                selectedAnswer = (selectedAnswer + 1) % 4;
                show_question_page();
            }
            else if (pressedKey == LEFT)
            {
                selectedAnswer = (selectedAnswer + 3) % 4;
                show_question_page();
            }
            else if (pressedKey == ENTER)
            {
                if (selectedAnswer == 1)
                {
                    printf("Experience answer correct\r\n");

                    show_correct_answer();
                    delay_short();

                    go_to_next_route(sm);
                }
                else
                {
                    printf("Experience answer wrong\r\n");

                    show_wrong_answer();
                    delay_short();

                    show_question_page();
                }
            }

            return;
            addToQueue(sm, &STATE_NAVIGATION);
            sm->isBusy = false;
        }
    }
}

void experience_exit(StateMachine_t *sm)
{
    oled_clear();
}