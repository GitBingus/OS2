#include "interrupts.h"
#include "types.h"
#include "tui.h"

#define IDT_ENTRIES 256


#define IDT_SET_VECTOR  33
#define SELECTOR        0x08
#define ATTR            0x8E


static struct idt_entry idt[IDT_ENTRIES];
static struct idtr_entry idtr;


static void (*const exception_handlers[32])(void) = {
    interrupt_handler_0,
    interrupt_handler_1,
    interrupt_handler_2,
    interrupt_handler_3,
    interrupt_handler_4,
    interrupt_handler_5,
    interrupt_handler_6,
    interrupt_handler_7,
    interrupt_handler_8,
    interrupt_handler_9,
    interrupt_handler_10,
    interrupt_handler_11,
    interrupt_handler_12,
    interrupt_handler_13,
    interrupt_handler_14,
    interrupt_handler_15,
    interrupt_handler_16,
    interrupt_handler_17,
    interrupt_handler_18,
    interrupt_handler_19,
    interrupt_handler_20,
    interrupt_handler_21,
    interrupt_handler_22,
    interrupt_handler_23,
    interrupt_handler_24,
    interrupt_handler_25,
    interrupt_handler_26,
    interrupt_handler_27,
    interrupt_handler_28,
    interrupt_handler_29,
    interrupt_handler_30,
    interrupt_handler_31
};


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


void idt_init(void)
{
    // Configure the IDTR
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (u32int)idt;

    // Register all 32 CPU exceptions
    for (u32int i = 0; i < 32; i++) {
        idt_set_gate(
            (u8int)i,
            (u32int)exception_handlers[i],
            0x08,
            0x8E
        );
    }

    // Register keyboard IRQ1 (vector 33)
    idt_set_gate(
        33,
        (u32int)interrupt_handler_33,
        0x08,
        0x8E
    );

    // Load the IDT into the processor
    idt_load(&idtr);
}

void exception_handler(u32int exception_number)
{
    tui_write("CPU EXCEPTION: ");

    char buffer[12];                            // buffer is 12 bytes because the largest unsigned 32-bit integer has 10 decimal digits, plus the null terminator
    uint_to_string(exception_number, buffer);
    tui_write(buffer);

    tui_write("\n");

    for (;;) {
        __asm__ volatile("cli; hlt");
    }
}