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

/*
    Kernel panic: display an error and halt.
    
    Interrupts are disabled to prevent hardware IRQs
    from continuing after a fatal error.
*/

static void kernel_panic(const char *message) {
    __asm__ volatile("cli");

    tui_write("\nKERNEL PANIC: ");
    tui_write(message);
    tui_newline();

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

    multiboot_info_t *mb_info =
        (multiboot_info_t *)ebx;

    // Initialise CPU segmentation
    setGDT();

    // Initialise VGA terminal
    tui_init();

    tui_write("Starting OS2...\n");

    // Validate Multiboot information
    if (mb_info == 0) {
        kernel_panic("Invalid Multiboot pointer");
    }

    if (!(mb_info->flags & MULTIBOOT_INFO_MEM_MAP)) {
        kernel_panic("Multiboot memory map unavailable");
    }

    // Initialise physical memory management
    pmm_init(mb_info);

    tui_write("[OK] Physical memory manager\n");

    // Initialise interrupt infrastructure
    pic_remap();
    idt_init();

    tui_write("[OK] Interrupt descriptor table\n");

    // Initialise virtual memory and enable paging
    paging_init();
    heap_init();

    void *a = kmalloc(64);
    void *b = kmalloc(128);

    if (!a || !b) {
        tui_write("[FAIL] Initial allocation\n");
        return;
    }

    kfree(a);

    void *c = kmalloc(64);

    if (c != 0) {
        tui_write("[PASS] Allocation after free\n");
    } else {
        tui_write("[FAIL] Allocation after free\n");
    }

    kfree(b);
    kfree(c);

    void *a2 = kmalloc(64);
    void *b2 = kmalloc(128);
    void *c2 = kmalloc(256);

    if (!a2 || !b2 || !c2) {
        tui_write("[FAIL] Initial allocations\n");
        return;
    }

    kfree(a2);
    kfree(b2);

    // This allocation is larger than either freed block alone.
    void *d = kmalloc(180);

    if (d == a2) {
        tui_write("[PASS] Adjacent free blocks coalesced\n");
    } else {
        tui_write("[FAIL] Coalescing test\n");
    }

    kfree(c2);
    kfree(d);

    tui_write("[OK] Heap memory manager\n");
    tui_write("[OK] Paging enabled\n");

    pit_init(PIT_DEFAULT_FREQUENCY);

    pic_unmask_irq(0); // enable PIT
    pic_unmask_irq(1); // enabled keyboard

    __asm__ volatile("sti");

    tui_write("[OK] PIT and keyboard interrupts enabled\n");

    shell_init();

    // Shell should normally never return
    kernel_panic("Shell unexpectedly returned");
}
