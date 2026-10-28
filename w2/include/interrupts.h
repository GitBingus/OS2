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

#endif