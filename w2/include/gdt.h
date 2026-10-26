#ifndef GDT_H
#define GDT_H

/*
Defines GDT array of sizes 2, 2, 1, 1, 1, 1 respectively
*/

struct gdt_entry
{
    unsigned short      limit_low;
    unsigned short      base_low;
    unsigned char       base_mid;
    unsigned char       access;
    unsigned char       granularity;
    unsigned char       base_high;
} __attribute__((packed));

struct gdtr_entry
{
    unsigned short  limit;
    unsigned int    base;
} __attribute__((packed));

void setGDT(void);

// Assembly function that loads the GDTR and reloads segment registers
extern void lgdt_init(struct gdtr_entry *gdtr);

#endif