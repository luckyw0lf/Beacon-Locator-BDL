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
    for (volatile int i = 0; i < 1000000; i++)
    {
    }
}

static void show_intro_page_1(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Mission:");

    oled_set_cursor(0, 2);
    oled_puts("Build an");

    oled_set_cursor(0, 3);
    oled_puts("air valve");

    oled_set_cursor(0, 6);
    oled_puts("ENTER");
}

static void show_intro_page_2(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Find parts");

    oled_set_cursor(0, 2);
    oled_puts("Assemble valve");

    oled_set_cursor(0, 4);
    oled_puts("Then test it");

    oled_set_cursor(0, 6);
    oled_puts("ENTER");
}

static void show_question_page(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("What building?");

    oled_set_cursor(0, 2);

    if (selectedAnswer == 0)
    {
        oled_puts(">1 Sensor  2 Valve");
    }
    else if (selectedAnswer == 1)
    {
        oled_puts(" 1 Sensor >2 Valve");
    }
    else
    {
        oled_puts(" 1 Sensor  2 Valve");
    }

    oled_set_cursor(0, 4);

    if (selectedAnswer == 2)
    {
        oled_puts(">3 Display 4 Battery");
    }
    else if (selectedAnswer == 3)
    {
        oled_puts(" 3 Display >4 Battery");
    }
    else
    {
        oled_puts(" 3 Display 4 Battery");
    }

    oled_set_cursor(0, 6);
    oled_puts("ENTER select");
}

static void show_wrong_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Wrong");

    oled_set_cursor(0, 2);
    oled_puts("Try again");
}

static void show_correct_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Correct!");

    oled_set_cursor(0, 2);
    oled_puts("Air valve");

    oled_set_cursor(0, 5);
    oled_puts("Next route");
}

static void go_to_next_route(StateMachine_t *sm)
{
    navigation_set_route(ROUTE_EXPERIENCE_TO_MATERIAL_QUESTION);
    navigation_set_next_state(&STATE_MATERIAL_QUESTION);

    addToQueue(sm, &STATE_NAVIGATION);
    sm->isBusy = false;
}

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
        }
    }
}

void experience_exit(StateMachine_t *sm)
{
    oled_clear();
}