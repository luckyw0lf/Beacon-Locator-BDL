#include <board.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <MCXA153.h>
#include <string.h>

#include "leds.h"
#include "serial.h"
#include "states.h"
#include "state_init.h"
#include "state_menu.h"
#include "keypad.h"
#include "hm10.h"
#include "oled.h"
#include "state_navigation.h"
#include "state_game_missions.h"
#include "game_logger.h"
#include "beacon_parser.h"

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------
#ifdef DEBUG
#define TARGETSTR "Debug"
#else
#define TARGETSTR "Release"
#endif

char pc_rx_buffer[50];
int pc_rx_index = 0;

StateMachine_t sm;

// this element use to count the time that kids spend on play game each state
volatile uint32_t puzzle_seconds_counter = 0    ;
volatile uint32_t systick_counter = 0;

extern char keypadFlag;
extern uint16_t pressedKey;


// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------
void emptyFunc(StateMachine_t *sm);
// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------
volatile uint32_t ms = 0;
static volatile uint32_t previous_ms = 0;
static StateMachine_t systemSM;
// define states here
State_t STATE_INIT = {&init_entry, &init_main, &init_exit, "INIT"};
State_t STATE_BOOT_MENU = {&menu_entry, &menu_main, &menu_exit, "BOOT_MENU"};
State_t STATE_ADMIN_MODE = {&menu_entry, &menu_main, &menu_exit, "ADMIN_MODE"};
State_t STATE_SEARCHING_BEACON = {&navigation_entry, &navigation_main, &navigation_exit, "SEARCHING_BEACON"};

// in game states
State_t STATE_NAVIGATION = {
    &navigation_entry,
    &navigation_main,
    &navigation_exit,
    "NAVIGATION"};

State_t STATE_EXPERIENCE_MISSION = {
    &experience_entry,
    &experience_main,
    &experience_exit,
    "EXPERIENCE_MISSION"};

State_t STATE_MATERIAL_QUESTION = {
    &material_question_entry,
    &material_question_main,
    &material_question_exit,
    "MATERIAL_QUESTION"};

State_t STATE_MATERIAL_COLLECTION = {
    &material_collection_entry,
    &material_collection_main,
    &material_collection_exit,
    "MATERIAL_COLLECTION"};

State_t STATE_BUILD_TEST = {
    &build_test_entry,
    &build_test_main,
    &build_test_exit,
    "BUILD_TEST"};

State_t STATE_FINISH = {
    &finish_entry,
    &finish_main,
    &finish_exit,
    "FINISH"};
// For menu
State_t STATE_RECONFIG_KEYPAD = {&emptyFunc, &keypadReInit, &emptyFunc, "ADMIN_MODE"};
volatile char handle_touch_interrupt = false;
volatile uint32_t touch_interrupt_now = 0;
// Also serves for debouncing, initially only needed for a small delay but this works nicely for debouncing too.
// The lower the value the more responsive but more chance on double inputs.
static unsigned char touch_ms_delay = 20;
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Main application
int main(void)
{
    initStateMachine(&systemSM, &STATE_INIT);
    serial_init(115200);
    hm10_init(9600);
    initKeypad();
    Logger_Init();

    while (1)
    {
        if (handle_touch_interrupt)
        {
            if (ms - touch_interrupt_now >= touch_ms_delay)
            {
                readKeypad();
                handle_touch_interrupt = false;
            }
        }
        updateStateMachine(&systemSM);

        char c = serial_rxcnt();

        pc_rx_buffer[pc_rx_index++] = c;

        if (c == '\n' || c == '\r' || pc_rx_index >= 49)
        {
            pc_rx_buffer[pc_rx_index] = '\0';

            // reading the request from GUI to get data from SD card
            if (strstr(pc_rx_buffer, "---REQUEST_LOG_DATA---") != NULL)
            {
                Logger_Respond_To_Pc();
            }

            memset(pc_rx_buffer, 0, sizeof(pc_rx_buffer));
            pc_rx_index = 0;
        }
    }
}
// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
void SysTick_Handler(void)
{
    ms++;
}

void emptyFunc(StateMachine_t *sm) {}