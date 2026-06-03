#ifndef HM_10
#define HM_10

#include <stdint.h>
#include <stdbool.h>

void hm10_init (uint32_t baudrate);
void hm10_read_beacons();
void hm10_send_command (const char *cmd);

#endif