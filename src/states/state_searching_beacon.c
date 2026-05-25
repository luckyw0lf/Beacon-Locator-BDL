
#include "leds.h"
#include "serial.h"
#include "states.h"
#include <stdio.h>
#include "fifo.h"
#include "state_searching_beacon.h"
#include "hm10.h"
#include "sll.h"
#include <string.h>
#include <stdlib.h>

#define FRAME_SIZE 4

// For setting up we can
IbeaconData_t targetBeacon;
fifo_t rollingScan;
IbeaconData_t buffer[FRAME_SIZE];

static node_t *pHead = NULL;
static node_t *currentNode = NULL;
static int current_step = 1;
char id[9] = "4C000215";

bool parse_beacon_string(const char *raw_string, IbeaconData_t *parsed_data)
{
    if (strlen(raw_string) < 78)
    {
        return false;
    }
    strncpy(parsed_data->major, raw_string + 50, 4);
    parsed_data->major[4] = '\0';
    strncpy(parsed_data->minor, raw_string + 54, 4);
    parsed_data->minor[4] = '\0';
    strncpy(parsed_data->rssi, raw_string + 74, 4);
    parsed_data->rssi[4] = '\0';
    strncpy(parsed_data->factoryId, raw_string + 8, 8);
    parsed_data->factoryId[8] = '\0';

    return true;
}

void search_entry(StateMachine_t *sm)
{
    printf("---START SEARCHING FOR IBEACONS---\r\n");
    f_init(&rollingScan, &buffer, FRAME_SIZE, sizeof(IbeaconData_t));

    addSLL(&pHead, "0B01", "0003", "-59");
    addSLL(&pHead, "0AEA", "0037", "-59");
    addSLL(&pHead, "0AEA", "0032", "-59");

    currentNode = pHead;

    hm10_send_command("AT+DISI?");

    sm->isBusy = true;
}

void search_main(StateMachine_t *sm)
{
    static char raw_beacon_string[150];

    if (hm10_read_line(raw_beacon_string, 150) == true)
    {
        if (strstr(raw_beacon_string, "OK+DISCE") != NULL)
        {
            printf("--- RESTARTING HM-10 ---\r\n");

            hm10_send_command("AT+DISI?");
        }
        else if (parse_beacon_string(raw_beacon_string, &targetBeacon) == true)
        {
            if (strcmp(targetBeacon.factoryId, id) == 0)
            {
                if (compareSLL(currentNode, targetBeacon.major, targetBeacon.minor, targetBeacon.rssi))
                {
                    if (current_step == 1)
                    {
                        GPIO3->PSOR = (1 << 15);
                        for (int i = 0; i < 2000000; i++)
                        {
                        }
                        GPIO3->PCOR = (1 << 15);
                    }
                    else if (current_step == 2)
                    {
                        GPIO3->PCOR = (1 << 15);
                    }
                    else if (current_step == 3)
                    {
                        GPIO3->PSOR = (1 << 14);
                    }

                    currentNode = currentNode->pNextNode;
                    current_step++;

                    if (currentNode == NULL)
                    {
                        printf("you have reached the end of the route");
                        sm->isBusy = false;
                    }
                }
            }
        }
        memset(raw_beacon_string, 0, sizeof(raw_beacon_string));
    }
}

void search_exit(StateMachine_t *sm)
{
    printf("---STOP SEARCHING FOR IBEACONS---");

    hm10_send_command("AT+SLEEP");
}

void setupHM10(StateMachine_t *sm)
{
    f_init(&rollingScan, &buffer, FRAME_SIZE, sizeof(IbeaconData_t));
}

State_t state_searching_beacon = {
    .entry = search_entry,
    .main = search_main,
    .exit = search_exit,
    .name = "SEARCHING_BEACON"};