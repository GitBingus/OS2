#include "fb.h"
#include "gdt.h"
#include "keyboard.h"


void kmain(void) {
    // Initialise the framebuffer and GDT
    fb_clear(COLOUR.BLACK);
    cursor = 0;

    setGDT();

    fb_move_cursor(cursor);

    monitor_kb();
}