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
#include "tetrominoes.h"
#include "graphics.h"

/*
    Kernel panic: display an error and halt.
    
    Interrupts are disabled to prevent hardware IRQs
    from continuing after a fatal error.
*/

void tetromino_debug(const tetromino_t *tetromino) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (tetromino->tiles[y][x]) {
                tui_write("[]");
            } else {
                tui_write("  ");
            }
        }
        tui_write("\n");
    }
}

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

    tui_write("OS2 Kernel starting...\n");
    tui_write("Initialising...\n");

    // Initialise CPU segmentation
    setGDT();
    tui_write("[OK] CPU segmentation\n");

    // Initialise VGA terminal
    tui_init();
    tui_write("[OK] VGA terminal\n");

    tui_write("Starting OS2...\n");

    // Validate Multiboot information
    if (mb_info == 0) {
        kernel_panic("Invalid Multiboot pointer");
    }

    if (!(mb_info->flags & MULTIBOOT_INFO_MEM_MAP)) {
        kernel_panic("Multiboot memory map unavailable");
    }

    tui_write("[OK] Multiboot information\n");
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

    tui_write("[OK] Heap memory manager\n");
    tui_write("[OK] Paging enabled\n");

    // Initialise graphics
    graphics_init(mb_info);

    tui_write("[OK] Graphics enabled\n");

    pit_init(PIT_DEFAULT_FREQUENCY);

    pic_unmask_irq(0); // enable PIT
    pic_unmask_irq(1); // enabled keyboard

    __asm__ volatile("sti");

    tui_write("[OK] PIT and keyboard interrupts enabled\n");

    // tui_write("Testing tetrominoes:\n");
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_I]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_O]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_J]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_L]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_Z]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_S]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_T]);

    shell_init();

    // Shell should normally never return
    kernel_panic("Shell unexpectedly returned");
}