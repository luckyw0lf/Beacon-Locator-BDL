#include "state_game_missions.h"
#include "state_navigation.h"
#include "keypad.h"
#include "oled.h"
#include "routes.h"
#include "helperFunctions.h" 
#include <stdio.h>
#include <string.h>
#include "game_logger.h"

extern State_t STATE_NAVIGATION;
extern State_t STATE_FINISH; 

extern char keypadFlag;
extern uint16_t pressedKey;

extern volatile uint32_t puzzle_seconds_counter;

static InputBuffer_t input;
static const char* correct_answer = "9999"; 

void build_test_entry(StateMachine_t *sm)
{
    printf("ENTER BUILD_TEST\r\n");

    memset(input.buffer, 0, sizeof(input.buffer));
    input.index = 0;
    input.submitted = false;

    oled_clear();
    oled_set_cursor(0, 0); oled_puts("BUILD AND TEST");
    oled_set_cursor(0, 2); oled_puts("ENTER VALVE CODE:");
    oled_set_cursor(0, 6); oled_puts("<- = DEL  ENTER = OK");

    sm->isBusy = true;
}

void build_test_main(StateMachine_t *sm)
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
                printf("CORRECT ANSWER! UNLOCKING...\r\n");
                Logger_Record_Time(ROOM_BUILD_TEST, puzzle_seconds_counter);                

                setUpPin(PORT3, 15, 1, 0); 
                
                GPIO3->PDOR &= ~(1 << 15);
                GPIO3->PSOR = (1 << 15);    
                

                oled_clear();
                oled_set_cursor(0, 0); oled_puts("UNLOCKED!"); 
                oled_set_cursor(0, 2); oled_puts("TEST PASSED!");
                oled_set_cursor(0, 4); oled_puts("Moving to finish..");
                
                navigation_set_route(ROUTE_BUILD_TEST_TO_FINISH);
                navigation_set_next_state(&STATE_FINISH);
                
                addToQueue(sm, &STATE_NAVIGATION); 
                sm->isBusy = false;
            }
            else 
            {
                printf("WRONG ANSWER!\r\n");               
                
                oled_clear();
                oled_set_cursor(0, 0); oled_puts("BUILD AND TEST"); 
                oled_set_cursor(0, 2); oled_puts("FAILED! Try again:");
                oled_set_cursor(0, 6); oled_puts("<- = DEL  ENTER = OK");
                
                memset(input.buffer, 0, sizeof(input.buffer));
                input.index = 0;
            }
        }
    }
}

void build_test_exit(StateMachine_t *sm)
{
    oled_clear();
}