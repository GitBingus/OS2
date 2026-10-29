#include "interrupts.h"
#include "types.h"

#define IDT_ENTRIES 256


#define IDT_SET_VECTOR  33
#define SELECTOR        0x08
#define ATTR            0x8E


static struct idt_entry idt[IDT_ENTRIES];
static struct idtr_entry idtr;


void idt_set_gate(u8int vector, u32int handler, u16int selector, u8int attributes) {
    // 1. Store the lower 16 bits of handler
    idt[vector].offset_low = handler & 0xFFFF;

    // 2. Store the code segment selector
    idt[vector].selector = selector;

    // 3. Set reserved to zero
    idt[vector].reserved = 0;

    // 4. Store the interrupt gate attributes
    idt[vector].type_attr = attributes;

    // 5. Store the upper 16 bits of handler
    idt[vector].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_init(void) {
    // Configure the IDTR
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (u32int)&idt;

    // Register interrupt handlers

    idt_set_gate(
        IDT_SET_VECTOR,
        (u32int)interrupt_handler_33,
        SELECTOR,
        ATTR
    );

    // Load the IDT
    idt_load(&idtr);
}