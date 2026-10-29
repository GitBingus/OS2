#include "gdt.h"
#include "keyboard.h"
#include "tui.h"
#include "interrupts.h"
#include "pic.h"

void kmain(void) {
    setGDT();
    tui_init();

    pic_remap();
    idt_init();

    tui_write("Welcome to OS2!\n");
    tui_write("Keyboard initialised.\n\n");
    tui_write("> ");

    // Interrupt enablement goes here once safe

    while (1) {
        keyboard_event_t event;

        if (keyboard_dequeue(&event)) {
            tui_handle_key(event);
        }
    }
}