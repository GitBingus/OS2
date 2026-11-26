#include "random.h"

static u32int random_seed = 1;

void srand(u32int seed) {
    if (seed == 0)
        seed = 1;

    random_seed = seed;
}

u32int rand(void) {
    random_seed ^= random_seed << 13;
    random_seed ^= random_seed >> 17;
    random_seed ^= random_seed << 5;

    return random_seed;
}

u32int rand_range(u32int min, u32int max) {
    if (min > max)
        return min;

    return min + (rand() % (max - min + 1));
}