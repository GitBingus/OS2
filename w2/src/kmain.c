#include "types.h"
#include "multiboot.h"
#include "gdt.h"
#include "interrupts.h"
#include "pic.h"
#include "tui.h"
#include "shell.h"
#include "pmm.h"
#include "paging.h"
#include "pit.h"
#include "heap.h"
#include "fb.h"
#include "io.h"
#include "kstring.h"

#define COM1 0x3F8
#define VGA_TEXT_MEMORY ((volatile u16int *)0xB8000)

static u32int boot_trace_column;

static void serial_putc(char c) {
    while ((inb(COM1 + 5) & 0x20) == 0)
        ;
    outb(COM1, (u8int)c);
}

static void serial_write(const char *message) {
    while (*message) {
        serial_putc(*message);
        message++;
    }
}

#if FB_MODE
static void serial_write_uint(u32int value) {
    char buffer[11];
    uint_to_string(value, buffer);
    serial_write(buffer);
}
#endif

#if FB_MODE
static void tui_write_hex(u32int value) {
    const char digits[] = "0123456789ABCDEF";

    tui_write("0x");
    for (int shift = 28; shift >= 0; shift -= 4) {
        char digit[2] = {digits[(value >> shift) & 0x0F], '\0'};
        tui_write(digit);
    }
}

static void tui_write_uint(u32int value) {
    char buffer[11];
    uint_to_string(value, buffer);
    tui_write(buffer);
}
#endif

#if FB_MODE
static void boot_trace_hex(const char *label, u32int value) {
    serial_write(label);
    serial_write("0x");

    for (int shift = 28; shift >= 0; shift -= 4) {
        u8int digit = (u8int)((value >> shift) & 0x0F);
        serial_putc(digit < 10 ? (char)('0' + digit) : (char)('A' + digit - 10));
    }

    serial_putc('\n');
}
#endif

static void boot_trace(const char *message) {
    volatile u16int *cursor = VGA_TEXT_MEMORY + boot_trace_column;

    serial_write(message);
    serial_putc('\n');

    while (*message) {
        if (*message == '\n') {
            boot_trace_column =
                ((boot_trace_column / 80) + 1) * 80;
        } else {
            *cursor = (u16int)(0x0F00 | (u8int)*message);
            cursor++;
            boot_trace_column++;
        }
        message++;
    }
}

static void serial_init(void) {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
}

/*
    Kernel panic: display an error and halt.
    
    Interrupts are disabled to prevent hardware IRQs
    from continuing after a fatal error.
*/

static void kernel_panic(const char *message) {
    __asm__ volatile("cli");
    boot_trace("\nKERNEL PANIC: ");
    boot_trace(message);

    for (;;) {
        __asm__ volatile("hlt");
    }
}

/*
    * Main kernel entry point.
    *
    * EBX contains the physical address of the Multiboot
    * information structure supplied by GRUB.
*/

void kmain(u32int ebx) {
    __asm__ volatile("cli");
    serial_init();
    boot_trace_column = 0;
    boot_trace("[BOOT] kmain entered\n");

    multiboot_info_t *mb_info =
        (multiboot_info_t *)ebx;

    // Initialise CPU segmentation
    boot_trace("[BOOT] Loading GDT\n");
    setGDT();
    boot_trace("[BOOT] GDT loaded\n");

    // Validate Multiboot information
    if (mb_info == 0) {
        kernel_panic("Multiboot information structure is NULL");
    }

    if (!(mb_info->flags & MULTIBOOT_INFO_MEM_MAP)) {
        kernel_panic("Multiboot memory map is not available");
    }

    // Initialise physical memory management
    boot_trace("[BOOT] Initialising PMM\n");
    pmm_init(mb_info);
    boot_trace("[BOOT] PMM initialised\n");

    // Initialise interrupt infrastructure
    boot_trace("[BOOT] Initialising PIC/IDT\n");
    pic_remap();
    idt_init();
    boot_trace("[BOOT] PIC/IDT initialised\n");

    // Initialise virtual memory and enable paging
    boot_trace("[BOOT] Initialising paging\n");
    paging_init();
    boot_trace("[BOOT] Paging initialised\n");

    boot_trace("[BOOT] Initialising heap\n");
    heap_init();
    boot_trace("[BOOT] Heap initialised\n");

#if FB_MODE
    boot_trace("[BOOT] Initialising framebuffer\n");
    boot_trace_hex("[FB] flags=", mb_info->flags);
    boot_trace_hex("[FB] type=", mb_info->framebuffer_type);
    boot_trace_hex("[FB] bpp=", mb_info->framebuffer_bpp);
    boot_trace_hex("[FB] width=", mb_info->framebuffer_width);
    boot_trace_hex("[FB] height=", mb_info->framebuffer_height);
    boot_trace_hex("[FB] pitch=", mb_info->framebuffer_pitch);
    boot_trace_hex("[FB] address=", (u32int)mb_info->framebuffer_addr);
    if (!framebuffer_init(mb_info)) kernel_panic("Framebuffer unavailable");
    boot_trace("[BOOT] Framebuffer initialised\n");
    serial_write("[FB] resolution=");
    serial_write_uint(mb_info->framebuffer_width);
    serial_putc('x');
    serial_write_uint(mb_info->framebuffer_height);
    serial_write(" bpp=");
    serial_write_uint(mb_info->framebuffer_bpp);
    serial_write(" pitch=");
    serial_write_uint(mb_info->framebuffer_pitch);
    serial_write(" type=");
    serial_write_uint(mb_info->framebuffer_type);
    serial_write(" address=0x");
    for (int shift = 28; shift >= 0; shift -= 4) {
        u8int digit = (u8int)(((u32int)mb_info->framebuffer_addr >> shift) & 0x0F);
        serial_putc(digit < 10 ? (char)('0' + digit) : (char)('A' + digit - 10));
    }
    serial_putc('\n');
#endif
    boot_trace("[BOOT] Initialising TUI\n");
    tui_init();
    boot_trace("[BOOT] TUI initialised\n");

#if FB_MODE
    tui_write("Framebuffer: ");
    tui_write_uint(mb_info->framebuffer_width);
    tui_write("x");
    tui_write_uint(mb_info->framebuffer_height);
    tui_write(" ");
    tui_write_uint(mb_info->framebuffer_bpp);
    tui_write("bpp pitch=");
    tui_write_uint(mb_info->framebuffer_pitch);
    tui_write(" type=");
    tui_write_uint(mb_info->framebuffer_type);
    tui_write(" addr=");
    tui_write_hex((u32int)mb_info->framebuffer_addr);
    tui_write("\n");
#else
    tui_write("Display: VGA text mode\n");
    serial_write("[BOOT] Display: VGA text mode\n");
#endif

    tui_write("Starting OS2...\n");
    tui_write("[OK] Physical memory manager\n");
    tui_write("[OK] Interrupt descriptor table\n");
    tui_write("[OK] Heap memory manager\n");
    tui_write("[OK] Paging enabled\n");

    boot_trace("[BOOT] Initialising PIT\n");
    pit_init(PIT_DEFAULT_FREQUENCY);
    boot_trace("[BOOT] PIT initialised\n");

    pic_unmask_irq(0); // enable PIT
    pic_unmask_irq(1); // enabled keyboard

    __asm__ volatile("sti");

    tui_write("[OK] PIT and keyboard interrupts enabled\n");

    boot_trace("[BOOT] Starting shell\n");
    shell_init();

    // Shell should normally never return
    kernel_panic("Shell unexpectedly returned");
}
