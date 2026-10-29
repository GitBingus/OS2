#ifndef PIC_H
#define PIC_H

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21

#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC1_OFFSET  0x20
#define PIC2_OFFSET  0x28

#define PIC_EOI 0x20

#include "types.h"

void pic_remap(void);
void pic_acknowledge(u8int irq);
void pic_unmask_irq(u8int irq);

#endif // PIC_H