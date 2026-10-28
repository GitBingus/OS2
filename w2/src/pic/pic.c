#include "pic.h"
#include "io.h"

void pic_remap(void)
{
    // 1. Start PIC initialisation.
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    // 2. Set vector offsets.
    outb(PIC1_DATA, PIC1_OFFSET);
    outb(PIC2_DATA, PIC2_OFFSET);

    void pic_acknowledge(unsigned char irq) {
        // TODO: If IRQ is from slave, acknowledge slave.

        // TODO: Always acknowledge master.
    }

    // TODO: Configure master/slave wiring (ICW3).

    // TODO: Configure 8086 mode (ICW4).

    // TODO: Set safe interrupt masks.
}