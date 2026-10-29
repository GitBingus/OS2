#include "pic.h"
#include "io.h"
#include "types.h"

void pic_remap(void) {
    // PIC initialisation

    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    // set vector offsets

    outb(PIC1_DATA, PIC1_OFFSET);
    outb(PIC2_DATA, PIC2_OFFSET);

    // cascade config

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    // 8086 mode

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // add IRQ masking

    outb(PIC1_DATA, 0xFD);  // unmask IRQ1
    outb(PIC2_DATA, 0xFF);
}

void pic_acknowledge(u8int irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_unmask_irq(u8int irq) {
    if (irq >= 16) {
        return;
    }

    u16int port = (irq >= 8) ? PIC2_DATA : PIC1_DATA;   // decide which PIC to send too
    u8int mask = inb(port);                             // reads PICs current input mask
    u8int bit = irq % 8;                                // decide which bit to use for which port

    mask &= (u8int)~(1u << bit);                        // flips the result of moving the mask down 'bit' places

    outb(port, mask);
}