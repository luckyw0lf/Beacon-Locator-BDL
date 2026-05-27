
#include "leds.h"
#include "serial.h"
#include "states.h"
#include <stdio.h>
#include "lcd.h"
#include "state_menu.h"
#include "keypad.h"
#include "mpr121.h"
#include "oled.h"


// private function prototype
void printMenu();

extern State_t STATE_ADMIN_MODE;
extern State_t STATE_INIT;
extern State_t STATE_RECONFIG_KEYPAD;
extern State_t STATE_SEARCHING_BEACON;
extern char keypadFlag;
extern uint16_t touch_reg;
extern uint16_t pressedKey;


static menuItem_t menuItems[] = {
    {&STATE_ADMIN_MODE, "Admin mode"},
    {&STATE_INIT, "Re-initialize"},
    {&STATE_RECONFIG_KEYPAD, "Reconfig Touch"},
    {&STATE_SEARCHING_BEACON, "Beacon search"}
};

// item count
#define MENU_ITEM_COUNT 4

static unsigned char menuPos;

void menu_entry(StateMachine_t *sm){
    // lcd_backlight(0);
    //lcd_return_home();
    //lcd_clear();
    oled_clear();
    menuPos = 0;
    printMenu();
}
void menu_main(StateMachine_t *sm){
    
    if(keypadFlag){
        keypadFlag = false;

        switch (pressedKey)
        {
        case ENTER:
            addToQueue(sm, menuItems[menuPos].selState);
            break;
        default:
            break;
        case RIGHT:
            menuPos = (menuPos+1) % MENU_ITEM_COUNT;
            break;
        case LEFT:
            menuPos = (menuPos+MENU_ITEM_COUNT-1) % MENU_ITEM_COUNT;
            break;
            
        }
        printMenu();
    }
    sm->isBusy = false;
}

void printMenu(){
    /*lcd_clear();
    char buffer[17]; // 17 with null terminator
    snprintf(buffer, sizeof(buffer), "> %s", menuItems[menuPos].name);
    lcd_set_cursor(0,0);
    lcd_put(buffer);*/
    oled_clear();

    oled_set_cursor(0, 0);
    oled_puts("MENU");

    char buffer[22];
    snprintf(buffer, sizeof(buffer), "> %s", menuItems[menuPos].name);

    oled_set_cursor(0, 2);
    oled_puts(buffer);
}
void menu_exit(StateMachine_t *sm){
    oled_clear();
}