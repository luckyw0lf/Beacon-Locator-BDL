#include "game_logger.h"
#include <stdio.h>
#include <string.h>

RouteLog logMemory[MAX_LOG_BEACON];

uint8_t currentBeacon = 0; // increment when reach to one beacon

void init_logger(void)
{
    currrentBeacon = 0;
}

void saveLogEntry(const char *id, uint32_t start, uint32_t finish)
{
    if (currentBeacon < MAX_LOG_BEACON)
    {
        strcpy(logMemory[MAX_LOG_BEACON].beaconID, id);
        logMemory[MAX_LOG_BEACON].startTime = start;
        logMemory[MAX_LOG_BEACON].finishTime = finish;

        currentBeacon++;
    }
}
void printToPC(void)
{
    for (unit8_t i = 0; i < MAX_LOG_BEACON; i++)
    {
        printf("LOG:%s:%lu:%lu\r\n", logMemory[MAX_LOG_BEACON].beaconID, 
                                     logMemory[MAX_LOG_BEACON].startTime, 
                                     logMemory[MAX_LOG_BEACON].finishTime);
    }
    printf("LOG_END\r\n");
}
