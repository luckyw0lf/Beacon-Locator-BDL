
#include "leds.h"
#include "serial.h"
#include "states.h"
#include <stdio.h>
#include "lcd.h"
#include "keypad.h"

#ifdef DEBUG
#define TARGETSTR "Debug"
#else
#define TARGETSTR "Release"
#endif

extern State_t STATE_BOOT_MENU;
extern volatile uint32_t ms;
static uint32_t bootDelay = 2000;
static uint32_t now;

void init_entry(StateMachine_t *sm){
    serial_init(115200);
    
    printf("BDL REVERSE GEOCAHCE BOOTING.....\r\n");
    printf("%s build %s %s\r\n", TARGETSTR, __DATE__, __TIME__);
    
    printf("ENTER %s\r\n", sm->state->name);
    
    //initialize keypad
    initKeypad();

    // Globally enable interrupts
    __enable_irq();
    
    // Generate SysTick interrupt every 1ms
    SysTick_Config(48000);

    // set time for min bootup delay
    now = ms;

    lcd_init();
    lcd_backlight(1);
    lcd_clear();
    lcd_return_home();
    lcd_put("BDL");
    lcd_set_cursor(0,1);
    lcd_put("beacon finder");


    addToQueue(sm, &STATE_BOOT_MENU);
}
void init_main(StateMachine_t *sm){
    // wait 2s for startup screen
    if(ms - now > bootDelay)
        sm->isBusy = false;
}
void init_exit(StateMachine_t *sm){
}