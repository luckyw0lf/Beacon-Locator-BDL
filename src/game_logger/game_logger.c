#include "game_logger.h"
#include "ff.h" 
#include <stdio.h>

static uint32_t room_times[MAX_ROOMS];
static FATFS fs;

static const char* room_names[MAX_ROOMS] = {
    "Experience Mission",
    "Material Question",
    "Material Collection",
    "Build Test"
};

bool Logger_Init(void) {
    for(int i = 0; i < MAX_ROOMS; i++) {
        room_times[i] = 0;
    }
    // mouting the SD card
    FRESULT fr = f_mount(&fs, "", 1); 
    if (fr != FR_OK) {
        printf("ERROR: SD Mount Failed! Code: %d\r\n", fr);
        return false;
    }
    printf("SUCCESS: SD Card Mounted!\r\n");
    return true;
}

void Logger_Record_Time(RoomID_t room, uint32_t seconds) {
    if (room < MAX_ROOMS) {
        room_times[room] = seconds;
    }
}

void Logger_Respond_To_PC(void) {
    for(int i = 0; i < MAX_ROOMS; i++) {
        printf("LOG:%d:%lu\r\n", i, room_times[i]);
    }
}

bool Logger_Export_Data(void) {
    FIL fil;
    FRESULT fr;

    // open gamelog.txt and save data 
    fr = f_open(&fil, "gamelog.txt", FA_WRITE | FA_OPEN_APPEND);
    if (fr != FR_OK) return false;

    f_printf(&fil, "--- NEW GAME SESSION ---\n");
    for(int i = 0; i < MAX_ROOMS; i++) {
        f_printf(&fil, "%s: %lu seconds\n", room_names[i], room_times[i]);
    }
    f_printf(&fil, "------------------------\n\n");

    f_close(&fil);
    printf("SUCCESS: Log saved to SD Card!\r\n");
    return true;
}