#ifndef KEYPAD_H
#define KEYPAD_H
#include "states.h"

enum keys {
    TWO,
    FIVE,
    EIGHT,
    ENTER,
    ONE, 
    FOUR,
    SEVEN,
    LEFT,
    ___UNKOWN_KEY_0,
    ___UNKOWN_KEY_1,
    ___UNKOWN_KEY_2,
    ___UNKOWN_KEY_3,
    THREE,
    SIX, 
    NINE,
    RIGHT
};

void initKeypad();
void readKeypad();
void keypadReInit(StateMachine_t *sm);
#endif