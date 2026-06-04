#include "state_game_missions.h"

#include "keypad.h"
#include "oled.h"
#include "lock.h"

#include <stdio.h>
#include <string.h>

extern State_t STATE_FINISH;

extern char keypadFlag;
extern uint16_t pressedKey;

extern volatile uint32_t puzzle_seconds_counter;

static char answerBuffer[4];
static uint8_t answerLength = 0;

static void clear_answer(void)
{
    memset(answerBuffer, 0, sizeof(answerBuffer));
    answerLength = 0;
}

static void show_final_question(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("FINAL CODE");

    oled_set_cursor(0, 2);
    oled_puts("How many parts?");

    oled_set_cursor(0, 5);
    oled_puts("Code: _");
}

static void show_answer_input(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("FINAL CODE");

    oled_set_cursor(0, 2);
    oled_puts("How many parts?");

    oled_set_cursor(0, 5);
    oled_puts("Code: ");
    oled_puts(answerBuffer);
    oled_puts("_");
}

static void show_wrong_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Wrong code");

    oled_set_cursor(0, 2);
    oled_puts("Try again");

    oled_set_cursor(0, 5);
    oled_puts("Code: _");
}

static void show_correct_answer(void)
{
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("Correct!");

    oled_set_cursor(0, 2);
    oled_puts("Box opening...");

    oled_set_cursor(0, 5);
    oled_puts("Well done");
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

    clear_answer();
    show_final_question();

    sm->isBusy = true;
}

void build_test_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (pressedKey == ENTER)
        {
            if (strcmp(answerBuffer, "9") == 0)
            {
                printf("Final code correct\r\n");
                //save finish time into SD card
                Logger_Record_Time(ROOM_BUILD_TEST, puzzle_seconds_counter);           
                
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

            show_answer_input();
        }
    }
}

void build_test_exit(StateMachine_t *sm)
{
    oled_clear();
}
