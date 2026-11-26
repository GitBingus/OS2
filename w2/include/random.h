#ifndef RANDOM_H
#define RANDOM_H

#include "types.h"

void srand(u32int seed);
u32int rand(void);
u32int rand_range(u32int min, u32int max);

#endif