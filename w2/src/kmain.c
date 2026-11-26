
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
#include "display.h"
#include "font.h"
#include "sound.h"

/*
    Kernel panic: display an error and halt.
    
    Interrupts are disabled to prevent hardware IRQs
    from continuing after a fatal error.
*/

void tetromino_debug(const tetromino_t *tetromino) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (tetromino->tiles[y][x]) {
                serial_write("[]");
            } else {
                serial_write("  ");
            }
        }
        serial_write("\n");
    }
}

static void kernel_panic(const char *message) {
    __asm__ volatile("cli");

    serial_write("KERNEL PANIC: ");
    serial_write(message);
    serial_write("\n");

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
    serial_write("OS2 serial debugging started\n");

    multiboot_info_t *mb_info =
        (multiboot_info_t *)ebx;

    serial_write("OS2 Kernel starting...\n");
    serial_write("Initialising...\n");

    // Initialise CPU segmentation
    setGDT();
    serial_write("[OK] CPU segmentation\n");

    // Initialise VGA terminal
    tui_init();
    serial_write("[OK] VGA terminal\n");

    serial_write("Starting OS2...\n");

    // Validate Multiboot information
    if (mb_info == 0) {
        kernel_panic("Invalid Multiboot pointer");
    }

    if (!(mb_info->flags & MULTIBOOT_INFO_MEM_MAP)) {
        kernel_panic("Multiboot memory map unavailable");
    }

    serial_write("[OK] Multiboot information\n");

    // Initialise physical memory management
    pmm_init(mb_info);
    serial_write("[OK] Physical memory manager\n");

    // Initialise interrupt infrastructure
    pic_remap();
    idt_init();

    serial_write("[OK] Interrupt descriptor table\n");

    // Initialise virtual memory and enable paging
    paging_init();
    heap_init();

    serial_write("[OK] Heap memory manager\n");
    serial_write("[OK] Paging enabled\n");

    serial_write("[OK] Graphics enabled\n");

    // Initialise graphics
    if (!(graphics_init(mb_info))) {
        kernel_panic("GPU Could not be enabled");
    }

    serial_write("Framebuffer virtual pointer: ");
    serial_write_hex((u32int)framebuffer.pixels);
    serial_write("\n");

    serial_write("Framebuffer address: ");
    serial_write_u32((u32int)mb_info->framebuffer_addr);
    serial_write("\n");

    serial_write("Framebuffer width: ");
    serial_write_u32(mb_info->framebuffer_width);
    serial_write("\n");

    serial_write("Framebuffer height: ");
    serial_write_u32(mb_info->framebuffer_height);
    serial_write("\n");

    serial_write("Framebuffer pitch: ");
    serial_write_u32(mb_info->framebuffer_pitch);
    serial_write("\n");

    serial_write("Framebuffer bpp: ");
    serial_write_u32(mb_info->framebuffer_bpp);
    serial_write("\n");

    serial_write("Framebuffer type: ");
    serial_write_u32(mb_info->framebuffer_type);
    serial_write("\n");

    display_init(DISPLAY_MODE_GRAPHICS);

    if (!(display_is_initialised())) {
        kernel_panic("Display not initialised");
    }

    font_init();

    display_setcolour(RGB(255, 255, 255), RGB(0, 0, 0));
    display_write("[OK] Font initialised\n");

    // Initialise the PIT
    pit_init(PIT_DEFAULT_FREQUENCY);

    if (!sb16_init()) kernel_panic("SB16 init failed");
    if (!sb16_dma_init()) kernel_panic("DMA init failed");

    sb16_generate_test_tone();

    pic_unmask_irq(0);
    pic_unmask_irq(1);

    __asm__ volatile("sti");

    serial_write_u32(sb16_debug_stage);
    serial_write("\n");

    serial_write_u32(sb16_write_value);
    serial_write("\n");

    serial_write_u32(sb16_last_write_status);
    serial_write("\n");

    serial_write_u32(sb16_write_attempts);
    serial_write("\n");

    // serial_write("Testing tetrominoes:\n");
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_I]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_O]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_J]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_L]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_Z]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_S]);
    // tetromino_debug(&tetrominoes[TETROMINO_TYPE_T]);

    // Shell should normally never return
    shell_init();

    kernel_panic("Shell unexpectedly returned");
}
