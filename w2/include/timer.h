#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void timer_init(u32int frequency);
void timer_irq_handler(void);

u32int timer_get_ticks(void);
u32int timer_get_uptime(void);

#endif