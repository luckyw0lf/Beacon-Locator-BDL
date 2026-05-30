#ifndef GAME_LOGGER_H
#define GAME_LOGGER_H

#include <stdint.h>
#include <stdbool.h>
#include "routes.h"

#define MAX_LOG_BEACON_EACH_ROUTE 15

typedef struct
{
    RouteId_t routeID;
    char   routeName[20]; // displayName
    uint32_t startTime;
    uint32_t finishTime;
    uint32_t totalTime;
} RouteLog;

extern RouteLog logMemory[MAX_LOG_BEACON_EACH_ROUTE];
uint8_t currentRouteCount; // increment when finish one Route!

void init_logger(void);

//this will directly receive data from struct RouteProfile_t in the UML diagram
void saveLogEntry(RouteProfile_t *completeRoute, uint32_t start, uint32_t finish);
void clear_all_logs(void);
void printToPC(void); // export data to communitate with the GUI

#endif 
