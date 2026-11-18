#ifndef HEAP_H
#define HEAP_H

#include "types.h"

typedef struct heap_block {
    u32int size;
    bool free;
    struct heap_block *next;
    u32int reserved; // Padding to ensure the structure is 16 bytes in size
} heap_block_t;

void *kmalloc(u32int size);
void kfree(void *ptr);

void heap_init(void);

#endif