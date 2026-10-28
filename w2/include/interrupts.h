#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"

struct idt_entry
{
    u16int offset_low;
    u16int selector;
    u8int  reserved;
    u8int  type_attr;
    u16int offset_high;
} __attribute__((packed));

_Static_assert(sizeof(struct idt_entry) == 8, "IDT entry must be 8 bytes"); // check to make sure size of IDT entry is correct

void idt_set_gate(unsigned int vector, unsigned int handler_address);

#endif