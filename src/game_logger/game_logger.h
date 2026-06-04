#ifndef GAME_LOGGER_H
#define GAME_LOGGER_H

#include <stdint.h>
#include <stdbool.h>
#include "routes.h"

bool Logger_Init(void);
void Logger_Record_Time(RouteId_t room, uint32_t seconds);
bool Logger_Save_Data(void);
void Logger_Respond_To_PC(void);

#endif