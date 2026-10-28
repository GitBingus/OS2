#include "gdt.h"
#include "keyboard.h"
#include "tui.h"

void kmain(void)
{
    setGDT();
    tui_init();

    tui_putchar('>');

    monitor_kb();
}