#include "pit.h"
#include "pic.h"
#include "io.h"

/*
Port	Purpose
0x40	Channel 0 data register
0x41	Channel 1 data register
0x42	Channel 2 data register
0x43	Mode/command register
*/

#define PIT_CHANNEL_0   0x40
#define PIT_COMMAND     0x43

static volatile u32int pit_ticks = 0;
static u32int pit_frequency = 0;

void pit_handler(void){
    pit_ticks++;

    pic_acknowledge(0);
}

u32int pit_get_ticks(void){
    return pit_ticks;
}


void pit_sleep_ms(u32int milliseconds) {
    if (milliseconds == 0 || pit_frequency == 0)
        return;

    // Convert milliseconds into PIT ticks, rounding up.
    u32int delay_ticks =
        (milliseconds / 1000) * pit_frequency +
        ((milliseconds % 1000) * pit_frequency + 999) / 1000;

    if (delay_ticks == 0)
        return;

    // Assumes interrupts were enabled when called.
    u32int start_ticks = pit_get_ticks();

    for (;;) {
        __asm__ volatile("cli" ::: "memory");

        if ((u32int)(pit_get_ticks() - start_ticks) >= delay_ticks) {
            __asm__ volatile("sti" ::: "memory");
            break;
        }

        // STI enables interrupts immediately before HLT.
        // The next timer interrupt wakes the CPU.
        __asm__ volatile("sti; hlt" ::: "memory");
    }
}


u32int pit_get_uptime_ms(void) {
    if (pit_frequency == 0)
        return 0;

    u32int ticks = pit_get_ticks();

    // Whole seconds, converted into milliseconds
    u32int milliseconds =
        (ticks / pit_frequency) * 1000;

    // Remaining ticks converted into milliseconds
    milliseconds +=
        ((ticks % pit_frequency) * 1000)
        / pit_frequency;

    return milliseconds;
}


void pit_init(u32int frequency) {
    // validation (the 20 - 1000hz limit is in place for testing)
    if (frequency < 20 || frequency > 1000) {
        frequency = PIT_DEFAULT_FREQUENCY;
    }

    u32int divisor = PIT_BASE_FREQUENCY / frequency;

    // Ensure divisor is in the range 1–65536
    if (divisor < 1 || divisor > 65536) return;

    // Configure PIT mode
    outb(PIT_COMMAND, 0x34);    // configures channel 0 to operate as a periodic rate generator
                                // accepting the low and high bytes of the divisor

    // Write the divisor
    u8int low = divisor & 0xFF;
    u8int high = (divisor >> 8) & 0xFF;

    outb(PIT_CHANNEL_0, low);
    outb(PIT_CHANNEL_0, high);

    pit_frequency = frequency;

    // Unmask IRQ0
    pic_unmask_irq(0);
}