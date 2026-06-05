#include "state_game_intro.h"

#include "keypad.h"
#include "oled.h"
#include "state_navigation.h"
#include "routes.h"

#include <stdio.h>

extern State_t STATE_NAVIGATION;
extern State_t STATE_EXPERIENCE_MISSION;

extern char keypadFlag;
extern uint16_t pressedKey;

static uint8_t introPage = 0;

static void show_intro_page(void)
{
    oled_clear();

    if (introPage == 0)
    {
        oled_set_cursor(0, 0);
        oled_puts("Welkom");
        oled_set_cursor(0, 2);
        oled_puts("avonturiers!");
        oled_set_cursor(0, 5);
        oled_puts("Druk ENTER");
    }
    else if (introPage == 1)
    {
        oled_set_cursor(0, 0);
        oled_puts("Los puzzels op");
        oled_set_cursor(0, 2);
        oled_puts("and learn");
        oled_set_cursor(0, 5);
        oled_puts("Druk ENTER");
    }
    else if (introPage == 2)
    {
        oled_set_cursor(0, 0);
        oled_puts("Volg hints");
        oled_set_cursor(0, 2);
        oled_puts("Vind kamers");
        oled_set_cursor(0, 5);
        oled_puts("Druk ENTER");
    }
    else
    {
        oled_set_cursor(0, 0);
        oled_puts("Succes!");
        oled_set_cursor(0, 2);
        oled_puts("Druk 1 start");
        oled_set_cursor(0, 5);
        oled_puts("ENTER = opnieuw");
    }
}

static void start_game(StateMachine_t *sm)
{
    addToQueue(sm, &STATE_NAVIGATION);
    sm->isBusy = false;
}

void game_intro_entry(StateMachine_t *sm)
{
    printf("ENTER GAME_INTRO\r\n");

    introPage = 0;
    show_intro_page();

    sm->isBusy = true;
}

void game_intro_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (introPage < 3)
        {
            if (pressedKey == ENTER)
            {
                introPage++;
                show_intro_page();
            }

            return;
        }

        /*
          Last intro page:
          - Press 1 to start the game.
          - Press ENTER to restart the intro from page 0.
         */
        if (pressedKey == ONE)
        {
            printf("Intro finished, starting game\r\n");
            start_game(sm);
        }
        else if (pressedKey == ENTER)
        {
            printf("Restarting intro pages\r\n");

            introPage = 0;
            show_intro_page();
        }
    }
}

void game_intro_exit(StateMachine_t *sm)
{
    oled_clear();
}