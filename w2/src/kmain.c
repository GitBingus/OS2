#include "gdt.h"
#include "keyboard.h"
#include "tui.h"

void kmain(void)
{
    setGDT();
    tui_init();

    tui_write("Welcome to OS2!\n");
    tui_write("Keyboard initialised.\n\n");
    tui_write("> ");

    while (1) {
        keyboard_event_t event = keyboard_poll();

        if (event.key == KEY_NONE)
            continue;

        tui_handle_key(event);
    }
}