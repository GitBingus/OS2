#ifndef GDT_H
#define GDT_H

/*
Defines GDT array of sizes 2, 2, 1, 1, 1, 1 respectively
*/

#include "types.h"

struct gdt_entry
{
    u16int      limit_low;
    u16int      base_low;
    u8int       base_mid;
    u8int       access;
    u8int       granularity;
    u8int       base_high;
} __attribute__((packed));

struct gdtr_entry
{
    u16int      limit;
    u32int      base;
} __attribute__((packed));

void setGDT(void);

// Assembly function that loads the GDTR and reloads segment registers
extern void lgdt_init(struct gdtr_entry *gdtr);

#endif