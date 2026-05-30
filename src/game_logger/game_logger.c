#include "game_logger.h"
#include <stdio.h>
#include <string.h>
#include "fsl_flashiap.h"
#include <MCXA153.h>



#define LOG_FLASH_ADDRESS 0x0001E000 // the register for internal flash memory
#define FLASH_SECTOR_SIZE 8192

RouteLog logMemory[MAX_LOG_BEACON];
flash_config_t flashConfig;

// unlock MBC (memory block checker to access to the internal flash mem inside the board)
static void unlock_flash_mbc (void) 
{
    GLIKEY0->CTRL_0 = 0x00060000U; GLIKEY0->CTRL_0 = 0x0002000FU;
    GLIKEY0->CTRL_0 = 0x0001000FU; GLIKEY0->CTRL_1 = 0x00290000U;
    GLIKEY0->CTRL_0 = 0x0002000FU; GLIKEY0->CTRL_1 = 0x00280000U;
    GLIKEY0->CTRL_0 = 0x0000000FU; MBC0->MBC_MEMN_GLBAC[0] = 0x7700U;
    for (uint8_t i = 0; i < 2U; i++) 
    {
        MBC0->MBC_INDEX[0].MBC_DOM0_MEM0_BLK_CFG_W[i] = 0x00000000U;
    }
    GLIKEY0->CTRL_0 = 0x0002000FU;
} 

void init_logger(void)
{
    unlock_flash_mbc();
    FLASH_Init(&flashConfig);
    memcpy(logMemory, (void*) LOG_FLASH_ADDRESS, size_t(logMemory));

    currentRouteCount = 0;
    for(int i = 0; i < MAX_LOG_BEACON_EACH_ROUTE; i++)
    {
        if (logMemory[i].startTime != 0 && logMemory[i].startTime != 0xFFFFFFFF)
        {
            currentRouteCount++;
        }
        else
        {
            break;
        }
    }
}

void saveLogEntry(RouteProfile_t *completeRoute, uint32_t start, uint32_t finish)
{
    if(currentRouteCount < MAX_LOG_BEACON_EACH_ROUTE && completeRoute != NULL)
    {
        // getting ID and routeName 
        logMemory[currentRouteCount].routeID = completeRoute -> id;

        strncpy (logMemory[currentRouteCount].routeName, completeRoute -> displayName, 19)
        logMemory[currentRouteCount].routeName[19] = '\0';

        // save time and calculate finish time for each route, this data used for GUI
        logMemory[currentRouteCount].startTime = start;
        logMemory[currentRouteCount].finishTime = finish;
        logMemory[currentRouteCount].totalTime = finish - start;

        currentRouteCount++; 
        
        //clear the flash memory and write down new data on it
        __disable_irq();
        FLASH_Erase(&flashConfig, LOG_FLASH_ADDRESS, FLASH_SECTOR_SIZE, kFLASH_ApiEraseKey);
        FLASH_Program(&flashConfig, LOG_FLASH_ADDRESS, (uint8_t*)logMemory, sizeof(logMemory));
        __enable_irq(); 
    }
}
void printToPC(void)
{
    printf("LOG_START");
    for(int i = 0; i < currentRouteCount; i++)
    {
        logMemory[i].routeId,
        logMemory[i].routeName,
        logMemory[i].startTime,
        logMemory[i].finishTime,
        logMemory[i].totalTime;
    }
    printf("LOG_END\r\n");
}
