#include <stdio.h>
#include <stdlib.h>
#include "hm10.h"
#include "lpuart2.h"
#include "sll.h"
#include "serial.h"
#include <stdbool.h>
#include <string.h>
#include "routes.h"
#include "state_searching_beacon.h"

#define TARGET_FACTORY_ID "4C000215"

extern BeaconDefinition_t beaconDefinitions[];
BeaconDefinition_t *recentBeacons[3] = {&beaconDefinitions[0],&beaconDefinitions[0],&beaconDefinitions[0]};
static int recentBeaconIndex = 0;
extern char beaconDefCount;

bool hm10_isBusy = false;
static uint16_t string_index = 0;
static IbeaconData_t current_beacon;
// static uint8_t timeoutCounter = 0;
static char buffer[80]; // length of a full beacon read string
void hm10_init(uint32_t baudrate)
{

    lpuart2_init(baudrate);

    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_PORT3(1);

    PORT3->PCR[15] = 0x00008000;
    PORT3->PCR[14] = 0x00008000;
    GPIO3->PDOR &= ~(1 << 15);
    GPIO3->PDOR &= ~(1 << 14);

    GPIO3->PDDR |= (1 << 15);
    GPIO3->PDDR |= (1 << 14);
}

void hm10_read_beacons() {

    while (lpuart2_rxcnt() > 0) { 
        uint8_t data = lpuart2_getchar();
        buffer[string_index] = (char)data;
        printf("%c", data);
        string_index++;
        if (data == '\n' || data == '\r') {
            // parse the data, reset buffer
            if(strncmp(&buffer[0], "OK+DISCE", 8) == 0){
                string_index = 0;
                memset(buffer, 0, 80); // clear the buffer
                hm10_isBusy = false;
            }

            if (strncmp(&buffer[8], TARGET_FACTORY_ID, 8) != 0) { // is not a beacon (or matched one)
                string_index = 0;
                memset(buffer, 0, 80); // clear the buffer
                continue; // skip a step in the while loop
            }

            if (strlen(buffer) < 78)
            {
                continue;
            }
            // clear struct
            memset(&current_beacon, 0, sizeof(IbeaconData_t));
            char rssi[5];

            
            strncpy(current_beacon.major, buffer + 50, 4);
            strncpy(current_beacon.minor, buffer + 54, 4);
            strncpy(rssi, buffer + 74, 4);
            strncpy(current_beacon.factoryId, buffer + 8, 8);
            // printf(buffer);
            // sscanf(buffer, 
            //     "OK+DISC:%8[^:]:%32[^:]:%4[0-9A-Fa-f]%4[0-9A-Fa-f]%2[0-9A-Fa-f]:%12[^:]:%4s",
            //     current_beacon.factoryId,
            //     current_beacon.uuid,
            //     current_beacon.major,
            //     current_beacon.minor,
            //     current_beacon.power,
            //     current_beacon.mac,
            //     rssi
            // );

            int converted = atoi(rssi);
            recentBeacons[recentBeaconIndex] = routes_find_beacon_by_major_minor(current_beacon.major, current_beacon.minor);
            recentBeaconIndex = (recentBeaconIndex+1) % 3;
            string_index = 0;
            memset(buffer, 0, 80); // clear the buffer



            printf("\r\nmajor: %s\r\nminor: %s\r\npower: %s\r\nrssi: %d\r\n", current_beacon.major, current_beacon.minor, current_beacon.power, converted);
            for(int x = 0; x < beaconDefCount; x++){

                if(strcmp(beaconDefinitions[x].major, current_beacon.major) != 0){
                    continue;
                } 
                if(strcmp(beaconDefinitions[x].minor, current_beacon.minor) != 0){
                    continue;
                } 

                // printf("beacondef: %s, curr_beacon: %s\r\n", beaconDefinitions[x].minor, current_beacon.minor);
                // printf("beacondef idx: %d, curr_beacon: %d\r\n", beaconDefinitions[x].index, converted);


                beaconDefinitions[x].rssi[beaconDefinitions[x].index] = converted;
                beaconDefinitions[x].index = (beaconDefinitions[x].index+1) % beaconDefinitions[x].rssiSize;
            }
        }
    }
}

void hm10_send_command(const char *cmd)
{
    if (cmd == NULL)
    {
        return;
    }

    while (*cmd != '\0')
    {
        lpuart2_putchar(*cmd);
        cmd++;
    }
}