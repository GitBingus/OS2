#include "gdt.h"
#include "keyboard.h"
#include "tui.h"
#include "interrupts.h"
#include "pic.h"
#include "input.h"

char input[128];

void test_div0(void) {
    tui_write("Testing division-by-zero exception...\n");

    __asm__ volatile (
        "movl $10, %%eax\n\t"
        "xorl %%edx, %%edx\n\t"
        "xorl %%ecx, %%ecx\n\t"
        "divl %%ecx"
        :
        :
        : "eax", "ecx", "edx", "cc"
    );
 
    tui_write("This should never print!\n");
}

void kmain(void)
{
    setGDT();
    tui_init();

    pic_remap();
    idt_init();

    // test_div0();

    pic_unmask_irq(1);
    __asm__ volatile("sti");

    tui_write("Welcome to OS2!\n");
    tui_write("Keyboard initialised.\n\n");
    tui_write("> ");

    while (1) {
        readline(input, sizeof(input));
        tui_write("\nYou entered: ");
        tui_write(input);
        tui_write("\n> ");
    }
}