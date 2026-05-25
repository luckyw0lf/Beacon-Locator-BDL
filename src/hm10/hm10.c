#include <stdio.h>
#include "hm10.h"
#include "lpuart2.h"
#include "sll.h"
#include "serial.h"
#include <stdbool.h>
#include <string.h>

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

bool hm10_read_line(char *buffer, uint16_t max_length) {
    static uint16_t index = 0;
    static char saved_char = 0; 

    while (lpuart2_rxcnt() > 0 || saved_char != 0) { 
        uint8_t data;

        if (saved_char != 0) {
            data = (uint8_t)saved_char;
            saved_char = 0;
        } else {
            data = lpuart2_getchar();
        }

        if (data == 'O' && index > 0) {
            buffer[index] = '\0';
            saved_char = 'O';   
            index = 0;          
            
            serial_putchar('\r');
            serial_putchar('\n');
            
            return true;
        }

        serial_putchar(data);

        if (data == '\n' || data == '\r') {
            if (index > 0) {
                buffer[index] = '\0';
                index = 0;
                return true;
            }
        } 

        else {
            if (index < max_length - 1) {
                buffer[index] = (char)data;
                index++;
            }
            if (index == 8 && strncmp (buffer, "OK+DISCE", 8) == 0)
            {
                buffer [index] = '\0';
                index = 0 ;

                serial_putchar('\r');
                serial_putchar('\n');

                return true;
            }
        }
    }

    return false; 
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