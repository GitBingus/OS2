#ifndef PIT_H
#define PIT_H

#include "types.h"

#define PIT_BASE_FREQUENCY 1193182
#define PIT_DEFAULT_FREQUENCY 100

void pit_init(u32int frequency);

void pit_handler(void);
u32int pit_get_ticks(void);
u32int pit_get_uptime_ms(void);

#endif