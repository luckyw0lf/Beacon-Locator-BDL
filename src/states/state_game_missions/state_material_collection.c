#include "state_game_missions.h"
#include "state_navigation.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"
#include <string.h>
#include <stdio.h>
#include "game_logger.h"

extern State_t STATE_NAVIGATION;
extern State_t STATE_BUILD_TEST;

extern char keypadFlag;
extern uint16_t pressedKey;

// this element use to count the time that kids spend on play game each state
extern volatile uint32_t puzzle_seconds_counter;

static InputBuffer_t input;
static const char* correct_answer = "1234"; // change later after have the quiz

void material_collection_entry(StateMachine_t *sm)
{
    printf("ENTER MATERIAL_COLLECTION\r\n");
    // reset the buffer whenever we enter the room
    memset(input.buffer, 0, sizeof(input.buffer));
    input.index = 0;
    input.submitted = false;

    oled_clear();
    oled_set_cursor(0, 0);
    oled_puts("MATERIAL COLLECTION"); //

    oled_set_cursor(0, 2);
    oled_puts("ENTER THE ANSWER:");

    oled_set_cursor(0, 6);
    oled_puts("< = DEL  ENTER = OK");

    sm->isBusy = true;
}

void material_collection_main(StateMachine_t *sm)
{
    if (keypadFlag)
    {
        keypadFlag = false;

        if (pressedKey >= '0' && pressedKey <= '9') 
        {
            if (input.index < 15)
            { 
                input.buffer[input.index] = (char)pressedKey;
                input.index++;
                
                oled_set_cursor(0, 4);
                oled_puts(input.buffer); 
            }
        }       
        else if (pressedKey == LEFT) 
        {
            if (input.index > 0) 
            {
                input.index--;
                input.buffer[input.index] = '\0';
                
               
                oled_set_cursor(0, 4);
                oled_puts("                "); 
                oled_set_cursor(0, 4);
                oled_puts(input.buffer);
            }
        }        
        else if (pressedKey == ENTER)
        {
            if (strcmp(input.buffer, correct_answer) == 0)
            {
                printf("CORRECT ANSWER!\r\n");
                Logger_Record_Time(ROOM_COLLECTION, puzzle_seconds_counter);

                oled_clear();
                oled_set_cursor(0, 2);
                oled_puts("CORRECT!");
                oled_set_cursor(0, 4);
                oled_puts("Moving to next...");
                
               
                navigation_set_route(ROUTE_COLLECTION_TO_BUILD_TEST);
                navigation_set_next_state(&STATE_BUILD_TEST);
                
                addToQueue(sm, &STATE_NAVIGATION); // move back to navigation mode
                sm->isBusy = false;
            }
            else 
            {
                printf("WRONG ANSWER!\r\n");               
                
                oled_clear();
                oled_set_cursor(0, 0); oled_puts("MATERIAL COLLECTION");
                oled_set_cursor(0, 2); oled_puts("WRONG! Try again:");
                oled_set_cursor(0, 6); oled_puts("*=DEL  ENTER=OK");
                
                memset(input.buffer, 0, sizeof(input.buffer));
                input.index = 0;
            }
        }
    }
}

void material_collection_exit(StateMachine_t *sm)
{
    oled_clear();
}