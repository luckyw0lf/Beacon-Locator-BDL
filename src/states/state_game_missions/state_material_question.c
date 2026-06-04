#include "state_game_missions.h"

#include "state_navigation.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"
#include "game_logger.h"


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

extern State_t STATE_NAVIGATION;
extern State_t STATE_MATERIAL_COLLECTION;

extern char keypadFlag;
extern uint16_t pressedKey;

extern volatile uint32_t puzzle_seconds_counter;

typedef enum
{
    MATERIAL_PAGE_INTRO_1,
    MATERIAL_PAGE_INTRO_2,
    MATERIAL_PAGE_INTRO_3,
    MATERIAL_PAGE_INPUT
} MaterialPage_t;

static MaterialPage_t currentPage = MATERIAL_PAGE_INTRO_1;

static char answerBuffer[4];
static uint8_t answerLength = 0;

static void delay_short(void)
{
    for (volatile int i = 0; i < 6000000; i++)
    {
    }
}

static void clear_answer(void)
{
    memset(answerBuffer, 0, sizeof(answerBuffer));
    answerLength = 0;
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

static void show_intro_page_1(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Verborgen");

    oled_set_cursor(0, 2);
    oled_puts("papieren");

    oled_set_cursor(0, 4);
    oled_puts("zijn in kamer");

    oled_set_cursor(0, 6);
    oled_puts("ENTER");
}

static void show_intro_page_2(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Vind papieren");

    oled_set_cursor(0, 2);
    oled_puts("met ventiel");

    oled_set_cursor(0, 4);
    oled_puts("onderdelen");

    oled_set_cursor(0, 6);
    oled_puts("ENTER");
}

static void show_intro_page_3(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Ontdek code");

    oled_set_cursor(0, 2);
    oled_puts("Voer 3 cijfers in");


    oled_set_cursor(0, 6);
    oled_puts("Druk op 1");
}

static void show_input_page(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Wat is code?");

    oled_set_cursor(0, 3);
    oled_puts("Code:");
    oled_puts(answerBuffer);
    oled_puts("_");

    oled_set_cursor(0, 6);
    oled_puts("ENTER klaar");
}

static void show_need_three_digits(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("3 cijfers nodig");

    oled_set_cursor(0, 2);
    oled_puts("Probeer weer");
}

static void show_wrong_order(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Juiste cijfers");

    oled_set_cursor(0, 2);
    oled_puts("Foute volgorde");

    oled_set_cursor(0, 4);
    oled_puts("Probeer weer");
}

static void show_wrong_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Foute code");

    oled_set_cursor(0, 2);
    oled_puts("Kijk opnieuw");

    oled_set_cursor(0, 4);
    oled_puts("Check papieren");
}

static void show_correct_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Juist!");

    oled_set_cursor(0, 2);
    oled_puts("Goed gedaan");

    oled_set_cursor(0, 5);
    oled_puts("Volgende route");
}

static bool has_correct_digits_wrong_order(void)
{
    if (answerLength != 3)
    {
        return false;
    }

    int count2 = 0;
    int count5 = 0;
    int count8 = 0;

    for (uint8_t i = 0; i < 3; i++)
    {
        if (answerBuffer[i] == '2')
        {
            count2++;
        }
        else if (answerBuffer[i] == '5')
        {
            count5++;
        }
        else if (answerBuffer[i] == '8')
        {
            count8++;
        }
        else
        {
            return false;
        }
    }

    return count2 == 1 && count5 == 1 && count8 == 1;
}

static void go_to_next_route(StateMachine_t *sm)
{
    //Logger_Record_Time(ROOM_QUESTION, puzzle_seconds_counter);

    addToQueue(sm, &STATE_NAVIGATION);
    sm->isBusy = false;
}

void material_question_entry(StateMachine_t *sm)
{
    printf("ENTER MATERIAL_QUESTION\r\n");

    currentPage = MATERIAL_PAGE_INTRO_1;
    clear_answer();

    show_intro_page_1();

    sm->isBusy = true;
}

void material_question_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (currentPage == MATERIAL_PAGE_INTRO_1)
        {
            if (pressedKey == ENTER)
            {
                currentPage = MATERIAL_PAGE_INTRO_2;
                show_intro_page_2();
            }

            return;
        }

        if (currentPage == MATERIAL_PAGE_INTRO_2)
        {
            if (pressedKey == ENTER)
            {
                currentPage = MATERIAL_PAGE_INTRO_3;
                show_intro_page_3();
            }

            return;
        }

        if (currentPage == MATERIAL_PAGE_INTRO_3)
        {
            if (pressedKey == ONE)
            {
                currentPage = MATERIAL_PAGE_INPUT;
                clear_answer();
                show_input_page();
            }

            return;
        }

        if (currentPage == MATERIAL_PAGE_INPUT)
        {
            if (pressedKey == LEFT)
            {
                clear_answer();
                show_input_page();
                return;
            }

            if (pressedKey == ENTER)
            {
                if (answerLength != 3)
                {
                    show_need_three_digits();
                    delay_short();
                    show_input_page();
                    return;
                }

                if (strcmp(answerBuffer, "258") == 0)
                {
                    printf("Material question correct\r\n");

                    show_correct_answer();
                    delay_short();

                    go_to_next_route(sm);
                    return;
                }

                if (has_correct_digits_wrong_order())
                {
                    printf("Material question wrong order: %s\r\n", answerBuffer);

                    clear_answer();
                    show_wrong_order();
                    delay_short();
                    show_input_page();
                    return;
                }

                printf("Material question wrong code: %s\r\n", answerBuffer);

                clear_answer();
                show_wrong_answer();
                delay_short();
                show_input_page();
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

                show_input_page();
            }

            return;
            addToQueue(sm, &STATE_NAVIGATION);
            sm->isBusy = false;
        }
    }
}

void material_question_exit(StateMachine_t *sm)
{
    oled_clear();
}
