
#include "leds.h"
#include "serial.h"
#include "states.h"
#include <stdio.h>
#include "fifo.h"

#define FRAME_SIZE 4


typedef struct {
    char factoryId[8];
    char uuid[32];
    char major[4];
    char minor[4];
    char power[2];
    char mac[12];
    char rssi[4];
} IbeaconData_t;

// Still need to think about how to do this, We probs want a FIFO for reading the beacon
// So we can add more IbeaconData_t while we are doing other things since UART takes time.
// but we also want a rollng frame so we can avarage out scans.
// I think we could have an array that has the size of the frame.
// every time we process the FIFO buffer, we can itterate with modulo
// and just rotate it around. then when we need to set the distance we avarage them.

// we put the IbeaconData into a FIFO, so we can process all Ibeacons here while it's scanning

// For setting up we can 
IbeaconData_t targetBeacon;
fifo_t rollingScan;
IbeaconData_t buffer[FRAME_SIZE];

void search_entry(StateMachine_t *sm){
}
void search_main(StateMachine_t *sm){
    // start reading iBeacon
    // set busy flag to true
    // only re-initiate another readout when it has finished reading out everything
    // set busy flag to false whenever finished reading so new states can be added
    // thinking of nesting a state machine, so we can 
    sm->isBusy = false;
}
void search_exit(StateMachine_t *sm){

}

void setupHM10(StateMachine_t *sm){
    f_init(&rollingScan, &buffer, FRAME_SIZE, sizeof(IbeaconData_t));
}