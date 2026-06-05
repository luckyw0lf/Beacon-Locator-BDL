#include "state_game_missions.h"

#include "keypad.h"
#include "oled.h"
#include "lock.h"
#include "game_logger.h"
#include "routes.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

extern State_t STATE_FINISH;

extern char keypadFlag;
extern uint16_t pressedKey;

extern volatile uint32_t puzzle_seconds_counter;

typedef enum
{
    BUILD_PAGE_INTRO_1,
    BUILD_PAGE_BUILD_HELP,
    BUILD_PAGE_CONGRATS,
    BUILD_PAGE_FINAL_QUESTION
} BuildPage_t;

static BuildPage_t currentPage = BUILD_PAGE_INTRO_1;

static char answerBuffer[4];
static uint8_t answerLength = 0;

static void clear_answer(void)
{
    memset(answerBuffer, 0, sizeof(answerBuffer));
    answerLength = 0;
}

static void show_intro_page_1(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Nu gaan we");

    oled_set_cursor(0, 2);
    oled_puts("het ventiel");

    oled_set_cursor(0, 3);
    oled_puts("bouwen");

    oled_set_cursor(0, 6);
    oled_puts("ENTER");
}

static void show_build_help_page(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Vraag hulp");

    oled_set_cursor(0, 2);
    oled_puts("bouw ventiel");

    oled_set_cursor(0, 4);
    oled_puts("Test daarna");

    oled_set_cursor(0, 6);
    oled_puts("Druk op 1");
}

static void show_congrats_page(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Gefeliciteerd!");

    oled_set_cursor(0, 2);
    oled_puts("Ventiel werkt");

    oled_set_cursor(0, 4);
    oled_puts("Als engineer!");

    oled_set_cursor(0, 7);
    oled_puts("ENTER");
}

static void show_final_question(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Laatste vraag:");

    oled_set_cursor(0, 2);
    oled_puts("Hoeveel delen");

    oled_set_cursor(0, 3);
    oled_puts("in ventiel?");

    oled_set_cursor(0, 6);
    oled_puts("Code: ");
    oled_puts(answerBuffer);
    oled_puts("_");
}

static void show_wrong_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Foute code");

    oled_set_cursor(0, 2);
    oled_puts("Probeer weer");

    oled_set_cursor(0, 5);
    oled_puts("Code: _");
}

static void show_correct_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Goed!");

    oled_set_cursor(0, 2);
    oled_puts("Doos opent...");

    oled_set_cursor(0, 5);
    oled_puts("Knap gedaan");
}

static char key_to_digit(uint16_t key)
{
    switch (key)
    {
        case ONE:
            return '1';

        case TWO:
            return '2';

        case THREE:
            return '3';

        case FOUR:
            return '4';

        case FIVE:
            return '5';

        case SIX:
            return '6';

        case SEVEN:
            return '7';

        case EIGHT:
            return '8';

        case NINE:
            return '9';

        default:
            return '\0';
    }
}

void build_test_entry(StateMachine_t *sm)
{
    printf("ENTER BUILD_TEST\r\n");

    currentPage = BUILD_PAGE_INTRO_1;
    clear_answer();

    keypadFlag = false;

    show_intro_page_1();

    sm->isBusy = true;
}

void build_test_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (currentPage == BUILD_PAGE_INTRO_1)
        {
            if (pressedKey == ENTER)
            {
                currentPage = BUILD_PAGE_BUILD_HELP;
                show_build_help_page();
            }

            return;
        }

        if (currentPage == BUILD_PAGE_BUILD_HELP)
        {
            if (pressedKey == ONE)
            {
                currentPage = BUILD_PAGE_CONGRATS;
                show_congrats_page();
            }

            return;
        }

        if (currentPage == BUILD_PAGE_CONGRATS)
        {
            if (pressedKey == ENTER)
            {
                currentPage = BUILD_PAGE_FINAL_QUESTION;
                clear_answer();
                show_final_question();
            }

            return;
        }

        if (currentPage == BUILD_PAGE_FINAL_QUESTION)
        {
            if (pressedKey == LEFT)
            {
                clear_answer();
                show_final_question();
                return;
            }

            if (pressedKey == ENTER)
            {
                if (strcmp(answerBuffer, "9") == 0)
                {
                    printf("Final code correct\r\n");

                    Logger_Record_Time(ROUTE_COLLECTION_TO_BUILD_TEST, puzzle_seconds_counter);

                    show_correct_answer();
                    lock_open();

                    addToQueue(sm, &STATE_FINISH);
                    sm->isBusy = false;
                }
                else
                {
                    printf("Wrong final code: %s\r\n", answerBuffer);

                    clear_answer();
                    show_wrong_answer();
                }

                return;
            }

            char digit = key_to_digit(pressedKey);

            if (digit != '\0')
            {
                if (answerLength < sizeof(answerBuffer) - 1)
                {
                    answerBuffer[answerLength] = digit;
                    answerLength++;
                    answerBuffer[answerLength] = '\0';
                }

                show_final_question();
            }

            return;
        }
    }
}

void build_test_exit(StateMachine_t *sm)
{
    oled_clear();
}