#ifndef GAME_LOGGER_H
#define GAME_LOGGER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ROOM_EXPERIENCE = 0,
    ROOM_QUESTION,
    ROOM_COLLECTION,
    ROOM_BUILD_TEST,
    MAX_ROOMS
} RoomID_t;

bool Logger_Init(void);
void Logger_Record_Time(RoomID_t room, uint32_t seconds);
bool Logger_Save_Data(void);
void Logger_Respond_To_PC(void);

#endif