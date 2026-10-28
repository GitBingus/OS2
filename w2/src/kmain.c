#include "gdt.h"
#include "keyboard.h"
#include "tui.h"
#include "kstring.h"

void kmain(void)
{
    setGDT();
    tui_init();

    tui_write("Welcome to OS2!\n");
    tui_write("Keyboard initialised.\n\n");
    tui_write("> ");
    for (unsigned int i = 1; i <= 30; i++) {
        char buffer[11];

        uint_to_string(i, buffer);

        tui_write("Line ");
        tui_write(buffer);
        tui_write("\n");
    }

    while (1) {
        keyboard_event_t event = keyboard_poll();

        if (event.key == KEY_NONE)
            continue;

        tui_handle_key(event);
    }
}