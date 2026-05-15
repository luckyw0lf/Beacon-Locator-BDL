
#include "leds.h"
#include "serial.h"
#include "states.h"
#include <stdio.h>
#include "lcd.h"
#include "state_menu.h"
#include "keypad.h"

// private function prototype
void printMenu();

extern State_t STATE_ADMIN_MODE;
extern State_t STATE_INIT;
extern char keypadFlag;
extern char pressedKey;

static menuItem_t menuItems[] = {
    {&STATE_ADMIN_MODE, "Admin mode"},
    {&STATE_INIT, "Re-initialize"}
};

// item counnt - 1
#define MENU_ITEM_COUNT 2

static unsigned char menuPos;

void menu_entry(StateMachine_t *sm){
    // lcd_backlight(0);
    lcd_return_home();
    lcd_clear();
    menuPos = 0;
    printMenu();
}
void menu_main(StateMachine_t *sm){
    
    if(keypadFlag){
        keypadFlag = false;

        printf("proccessing keypress, key: %d \r\n", pressedKey);
        switch (pressedKey)
        {
        case ENTER:
            addToQueue(sm, menuItems[menuPos].selState);
            break;
        default:
            break;
        case DOWN:
            menuPos = (menuPos+1) % MENU_ITEM_COUNT;
            break;
        }
        printMenu();
    }
    sm->isBusy = false;
}

void printMenu(){
    lcd_clear();
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "> %s", menuItems[menuPos].name);
    lcd_set_cursor(0,0);
    lcd_put(buffer);
}
void menu_exit(StateMachine_t *sm){
    lcd_clear();
}