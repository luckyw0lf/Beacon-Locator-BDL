#ifdef SYSTEM_TIMER_H
#include SYSTEM_TIMER_H

#include <stdint.h>

void SysTick_Init(void);
uint32_t get_system_time(void);

#endif