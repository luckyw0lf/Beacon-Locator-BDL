#include "state_game_missions.h"

#include "state_navigation.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"

#include <stdio.h>
#include <stdint.h>

extern State_t STATE_NAVIGATION;
extern State_t STATE_BUILD_TEST;

extern char keypadFlag;
extern uint16_t pressedKey;

extern volatile uint32_t puzzle_seconds_counter;

typedef enum
{
    COLLECTION_PAGE_INTRO_1,
    COLLECTION_PAGE_INTRO_2,
    COLLECTION_PAGE_SCALE_INFO,
    COLLECTION_PAGE_READY_INPUT,
    COLLECTION_PAGE_QUESTION
} CollectionPage_t;

static CollectionPage_t currentPage = COLLECTION_PAGE_INTRO_1;
static uint8_t selectedAnswer = 0;

/*
    selectedAnswer:
    0 = 60.42  correct
    1 = 30.42
    2 = 40.42
    3 = 75.42
*/

static void delay_short(void)
{
    for (volatile int i = 0; i < 8000000; i++)
    {
    }
}

static void show_intro_page_1(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Find all valve");

    oled_set_cursor(0, 2);
    oled_puts("parts");

    oled_set_cursor(0, 4);
    oled_puts("Total: 9 parts");

    oled_set_cursor(0, 6);
    oled_puts("ENTER");
}

static void show_intro_page_2(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Use papers");

    oled_set_cursor(0, 2);
    oled_puts("you found");

    oled_set_cursor(0, 4);
    oled_puts("for the parts");

    oled_set_cursor(0, 6);
    oled_puts("Press 1 ready");
}

static void show_scale_info_page(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Congrats!");

    oled_set_cursor(0, 2);
    oled_puts("Find the scale");

    oled_set_cursor(0, 4);
    oled_puts("Weigh pieces");

    oled_set_cursor(0, 6);
    oled_puts("ENTER");
}

static void show_ready_input_page(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("There is your");

    oled_set_cursor(0, 2);
    oled_puts("answer");

    oled_set_cursor(0, 5);
    oled_puts("Press 1 submit");
}

static void show_question_page(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Whats weight?");

    oled_set_cursor(0, 2);

    if (selectedAnswer == 0)
    {
        oled_puts(">1 60.42 2 30.42");
    }
    else if (selectedAnswer == 1)
    {
        oled_puts(" 1 60.42>2 30.42");
    }
    else
    {
        oled_puts(" 1 60.42 2 30.42");
    }

    oled_set_cursor(0, 4);

    if (selectedAnswer == 2)
    {
        oled_puts(">3 40.42 4 75.42");
    }
    else if (selectedAnswer == 3)
    {
        oled_puts(" 3 40.42>4 75.42");
    }
    else
    {
        oled_puts(" 3 40.42 4 75.42");
    }

    oled_set_cursor(0, 6);
    oled_puts("ENTER select");
}

static void show_wrong_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Wrong weight");

    oled_set_cursor(0, 2);
    oled_puts("Try again");

    oled_set_cursor(0, 4);
    oled_puts("Check scale");
}

static void show_correct_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Correct!");

    oled_set_cursor(0, 2);
    oled_puts("Weight ok");

    oled_set_cursor(0, 5);
    oled_puts("Next route");
}

static void go_to_next_route(StateMachine_t *sm)
{
    // Logger_Record_Time(ROOM_COLLECTION, puzzle_seconds_counter);

    navigation_set_route(ROUTE_COLLECTION_TO_BUILD_TEST);
    navigation_set_next_state(&STATE_BUILD_TEST);

    addToQueue(sm, &STATE_NAVIGATION);
    sm->isBusy = false;
}

void material_collection_entry(StateMachine_t *sm)
{
    printf("ENTER MATERIAL_COLLECTION\r\n");

    currentPage = COLLECTION_PAGE_INTRO_1;
    selectedAnswer = 0;

    show_intro_page_1();

    sm->isBusy = true;
}

void material_collection_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (currentPage == COLLECTION_PAGE_INTRO_1)
        {
            if (pressedKey == ENTER)
            {
                currentPage = COLLECTION_PAGE_INTRO_2;
                show_intro_page_2();
            }

            return;
        }

        if (currentPage == COLLECTION_PAGE_INTRO_2)
        {
            if (pressedKey == ONE)
            {
                currentPage = COLLECTION_PAGE_SCALE_INFO;
                show_scale_info_page();
            }

            return;
        }

        if (currentPage == COLLECTION_PAGE_SCALE_INFO)
        {
            if (pressedKey == ENTER)
            {
                currentPage = COLLECTION_PAGE_READY_INPUT;
                show_ready_input_page();
            }

            return;
        }

        if (currentPage == COLLECTION_PAGE_READY_INPUT)
        {
            if (pressedKey == ONE)
            {
                currentPage = COLLECTION_PAGE_QUESTION;
                selectedAnswer = 0;
                show_question_page();
            }

            return;
        }

        if (currentPage == COLLECTION_PAGE_QUESTION)
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
                if (selectedAnswer == 0)
                {
                    printf("Material collection correct weight\r\n");

                    show_correct_answer();
                    delay_short();

                    go_to_next_route(sm);
                }
                else
                {
                    printf("Material collection wrong weight option: %d\r\n", selectedAnswer);

                    show_wrong_answer();
                    delay_short();

                    show_question_page();
                }
            }

            return;
        }
    }
}

void material_collection_exit(StateMachine_t *sm)
{
    oled_clear();
}