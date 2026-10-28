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

    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void pic_acknowledge(u8int irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}