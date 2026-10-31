#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"


extern void interrupt_handler_0(void);
extern void interrupt_handler_33(void);


struct idt_entry {
    u16int offset_low;
    u16int selector;
    u8int  reserved;
    u8int  type_attr;
    u16int offset_high;
} __attribute__((packed));

struct idtr_entry {
    u16int limit;
    u32int base;
} __attribute__((packed));

_Static_assert(sizeof(struct idt_entry) == 8, "IDT entry must be 8 bytes"); // check to make sure size of IDT   entry is correct
_Static_assert(sizeof(struct idtr_entry) == 6, "IDTR must be 6 bytes");     // check to make sure size of IDTR  entry is correct

void idt_set_gate(u8int vector, u32int handler, u16int selector, u8int attributes);

void idt_init(void);
void idt_load(struct idtr_entry *idtr);

#endif