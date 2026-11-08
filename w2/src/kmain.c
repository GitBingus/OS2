#include "gdt.h"
#include "keyboard.h"
#include "tui.h"
#include "interrupts.h"
#include "pic.h"
#include "input.h"
#include "shell.h"

char input[128];
u8int scancodes[128];

extern volatile u32int keyboard_irq_count;
extern volatile u32int keyboard_event_count;
extern volatile u32int keyboard_dropped_count;
extern volatile u32int keyboard_scancodes_read;
extern volatile u32int keyboard_output_pending;



void test() {
    tui_write("\nKeyboard IRQ count: ");
    char irq_count_str[12];
    uint_to_string(keyboard_irq_count, (irq_count_str));
    tui_write(irq_count_str);
    
    tui_write("\nKeyboard event count: ");
    char event_count_str[12];
    uint_to_string(keyboard_event_count, (event_count_str));
    tui_write(event_count_str);
    
    tui_write("\nKeyboard dropped count: ");
    char dropped_count_str[12];
    uint_to_string(keyboard_dropped_count, (dropped_count_str));
    tui_write(dropped_count_str);
    
    tui_write("\nKeyboard scancodes read: ");
    char scancodes_read_str[12];
    uint_to_string(keyboard_scancodes_read, (scancodes_read_str));
    tui_write(scancodes_read_str);
    
    tui_write("\nKeyboard output pending: ");
    char output_pending_str[12];
    uint_to_string(keyboard_output_pending, (output_pending_str));
    tui_write(output_pending_str);
    
    u8int scancodes[128];
    
    u32int count = keyboard_get_scancode_log(scancodes, 128);
    
    tui_write("\nScancode log:\n");
    
    for (u32int i = 0; i < count; i++) {
        char hex[3];
        
        byte_to_hex(scancodes[i], hex);
        
        tui_write(hex);
        tui_putchar(' ');
    }
    
    tui_newline();
}

void kmain(void) {
    setGDT();
    tui_init();
    
    pic_remap();
    idt_init();
    
    // test_div0();
    
    pic_unmask_irq(1);
    __asm__ volatile("sti");
    
    shell_init();
}