#ifdef GAME_LOGGER_H
#define GAME_LOGGER_H

#include <stdint.h>

#define MAX_LOG_BEACON 10

typedef struct
{
    char beaconID[20];
    uint32_t startTime;
    uint32_t finishTime;
} RouteLog;

void init_logger(void);
void saveLogEntry(const char *id, uint32_t start, uint32_t finish);
void printToPC(void);

#endif 
