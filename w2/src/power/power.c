#include "power.h"
#include "io.h"
#include "tui.h"

void system_shutdown_qemu(void) {
    tui_write("Shutting down OS2...\n");

    // Prevent further maskable hardware interrupts.
    __asm__ volatile("cli");

    // QEMU ACPI power-off request (standard pc machine).
    outw(0x604, 0x2000);

    // If shutdown fails, halt instead of returning to the shell.
    for (;;) {
        __asm__ volatile("hlt");
    }
}

void system_shutdown(void) {
    tui_write("Shutting down OS2...\n");

    // Prevent further maskable hardware interrupts.
    __asm__ volatile("cli");

    // ACPI power-off request (standard pc machine).
    outw(0xB004, 0x2000);

    // If shutdown fails, halt instead of returning to the shell.
    for (;;) {
        __asm__ volatile("hlt");
    }
}