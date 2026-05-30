#include <stdint.h>
#include <board.h>
#include <stdio.h>
#include "system_timer.h"

volatile uint32_t ms = 0;

void SysTick_Init(void)
{
    SysTick_Config(SystemCoreClock/1000);
}

void Systick_Handler(void)
{
    ms++;
}

uint32_t get_system_time(void)
{
    return ms;
}