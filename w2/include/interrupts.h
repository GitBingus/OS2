#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"


extern void interrupt_handler_0(void);
extern void interrupt_handler_1(void);
extern void interrupt_handler_2(void);
extern void interrupt_handler_3(void);
extern void interrupt_handler_4(void);
extern void interrupt_handler_5(void);
extern void interrupt_handler_6(void);
extern void interrupt_handler_7(void);
extern void interrupt_handler_8(void);
extern void interrupt_handler_9(void);
extern void interrupt_handler_10(void);
extern void interrupt_handler_11(void);
extern void interrupt_handler_12(void);
extern void interrupt_handler_13(void);
extern void interrupt_handler_14(void);
extern void interrupt_handler_15(void);
extern void interrupt_handler_16(void);
extern void interrupt_handler_17(void);
extern void interrupt_handler_18(void);
extern void interrupt_handler_19(void);
extern void interrupt_handler_20(void);
extern void interrupt_handler_21(void);
extern void interrupt_handler_22(void);
extern void interrupt_handler_23(void);
extern void interrupt_handler_24(void);
extern void interrupt_handler_25(void);
extern void interrupt_handler_26(void);
extern void interrupt_handler_27(void);
extern void interrupt_handler_28(void);
extern void interrupt_handler_29(void);
extern void interrupt_handler_30(void);
extern void interrupt_handler_31(void);

extern void interrupt_handler_33(void);

void exception_handler(u32int exception_number);

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