#include "shell.h"
#include "tui.h" 
#include "kstring.h"
#include "input.h"
#include "types.h"
#include "pmm.h"
#include "paging.h"
#include "power.h"
#include "pit.h"
#include "heap.h"

#define SHELL_BUFFER 128
#define SHELL_MAX_ARGS 16
#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

#define SHELL_HISTORY_SIZE 16
#define SHELL_BUFFER_SIZE 128

static char history[SHELL_HISTORY_SIZE][SHELL_BUFFER_SIZE];
static u32int history_count = 0;

static void shell_help(int argc, cstring argv[]);
static void shell_clear(int argc, cstring argv[]);
static void shell_version(int argc, cstring argv[]);
static void shell_echo(int argc, cstring argv[]);
static void shell_cmd_memtest(int argc, cstring argv[]);
static void shell_cmd_pagetest(int argc, cstring argv[]);
static void ticks(int argc, cstring argv[]);
static void shell_cmd_shutdown(int argc, cstring argv[]);
static void sleep_test(int argc, cstring argv[]);
static void heap_test(int argc, cstring argv[]);
static void shell_cmd_vmtest(int argc, cstring argv[]);

static const shell_command_t commands[] = {
    {"help",        "Show available commands, use with 'help `<command>`'",             shell_help},
    {"clear",       "Clear the screen, use with 'clear'",                               shell_clear},
    {"version",     "Show OS2 version, use with 'version'",                             shell_version},
    {"keytest",     "Tests keyboard, use with 'keytest'",                               keyboard_test},
    {"div0",        "Tests division-by-zero exception, use with 'div0'",                test_div0},
    {"echo",        "Prints arguments, use with 'echo `options`'",                      shell_echo},
    {"meminfo",     "Displays memory information, use with 'meminfo'",                  shell_meminfo},
    {"memtest",     "Tests physical memory, use with 'memtest'",                        shell_cmd_memtest},
    {"pagetest",    "Tests virtual memory mapping, use with 'pagetest'",                shell_cmd_pagetest},
    {"pittest",     "Tests the PIT timer, use with 'pittest'",                          ticks},
    {"shutdown",    "Shuts down the system, use with 'shutdown'",                       shell_cmd_shutdown},
    {"sleep",       "Tests the PIT sleep function, use with 'sleep <milliseconds>'",    sleep_test},
    {"heap",        "Tests the heap memory manager, use with 'heap'",                   heap_test},
    {"vmtest",      "Tests the virtual memory manager, use with 'vmtest'",              shell_cmd_vmtest},
};

static void shell_cmd_vmtest(int argc, cstring argv[]) {
    (void)argc;
    (void)argv;

    const u32int page_count = 3;
    const u32int test_flags = PAGE_WRITABLE;

    tui_write("\nVirtual page allocation test\n");

    /*
     * Warm up the allocator first.
     *
     * The first mapping may allocate a page-table frame. Warming it up
     * prevents that setup cost from affecting the later frame-count test.
     */

    void *warmup = vm_alloc_pages(1, test_flags);

    if (warmup == 0) {
        tui_write("[FAIL] Allocator warm-up\n");
        return;
    }

    if (!vm_free_pages(warmup, 1)) {
        tui_write("[FAIL] Allocator warm-up cleanup\n");
        return;
    }

    u32int frames_before = pmm_get_free_frames();

    /*
     * Test 1: zero-page allocation must fail.
     */
    if (vm_alloc_pages(0, test_flags) == 0) {
        tui_write("[PASS] Zero-page allocation rejected\n");
    } else {
        tui_write("[FAIL] Zero-page allocation rejected\n");
    }

    /*
     * Test 2: allocate three contiguous pages.
     */
    void *allocation = vm_alloc_pages(page_count, test_flags);

    if (allocation == 0) {
        tui_write("[FAIL] Multi-page allocation\n");
        return;
    }

    tui_write("[PASS] Multi-page allocation\n");

    u8int *base = (u8int *)allocation;

    /*
     * Test 3: verify that allocation consumes exactly three frames.
     */
    u32int frames_after_alloc = pmm_get_free_frames();

    if (frames_before >= page_count &&
        frames_after_alloc == frames_before - page_count) {
        tui_write("[PASS] Physical frame accounting\n");
    } else {
        tui_write("[FAIL] Physical frame accounting\n");
    }

    /*
     * Test 4: write to and read from each page.
     *
     * Writing at different offsets proves that all pages are mapped,
     * rather than only the first page being valid.
     */
    volatile u32int *page0 = (volatile u32int *)(base);
    volatile u32int *page1 =
        (volatile u32int *)(base + PAGE_SIZE);
    volatile u32int *page2 =
        (volatile u32int *)(base + (2 * PAGE_SIZE));

    *page0 = 0x11111111;
    *page1 = 0x22222222;
    *page2 = 0x33333333;

    if (*page0 == 0x11111111 &&
        *page1 == 0x22222222 &&
        *page2 == 0x33333333) {
        tui_write("[PASS] Cross-page read/write\n");
    } else {
        tui_write("[FAIL] Cross-page read/write\n");
    }

    /*
     * Test 5: unaligned free must be rejected.
     */
    if (!vm_free_pages((void *)(base + 1), 1)) {
        tui_write("[PASS] Unaligned free rejected\n");
    } else {
        tui_write("[FAIL] Unaligned free rejected\n");
    }

    /*
     * Test 6: freeing beyond the allocation must fail.
     *
     * This address is immediately after the three allocated pages.
     */
    if (!vm_free_pages((void *)(base + (page_count * PAGE_SIZE)), 1)) {
        tui_write("[PASS] Invalid-range free rejected\n");
    } else {
        tui_write("[FAIL] Invalid-range free rejected\n");
    }

    /*
     * Test 7: free the valid allocation.
     */
    if (vm_free_pages(allocation, page_count)) {
        tui_write("[PASS] Multi-page free\n");
    } else {
        tui_write("[FAIL] Multi-page free\n");
        return;
    }

    /*
     * Test 8: double-free must be rejected.
     */
    if (!vm_free_pages(allocation, page_count)) {
        tui_write("[PASS] Double free rejected\n");
    } else {
        tui_write("[FAIL] Double free rejected\n");
    }

    /*
     * Test 9: all three physical frames should be available again.
     */
    u32int frames_after_free = pmm_get_free_frames();

    if (frames_after_free == frames_before) {
        tui_write("[PASS] Physical frames reclaimed\n");
    } else {
        tui_write("[FAIL] Physical frames reclaimed\n");
    }

    /*
     * Test 10: the allocator should be able to reuse the freed region.
     */
    void *reuse = vm_alloc_pages(page_count, test_flags);

    if (reuse != 0) {
        tui_write("[PASS] Freed virtual range reused\n");

        if (!vm_free_pages(reuse, page_count)) {
            tui_write("[FAIL] Reused allocation cleanup\n");
        }
    } else {
        tui_write("[FAIL] Freed virtual range reused\n");
    }

    tui_write("Virtual page allocation test complete.\n");
}

static void heap_test(int argc, cstring argv[]) {
    (void)argc;
    (void)argv;

    tui_write("Testing heap memory allocation...\n");

    // Allocate a block of memory
    u32int size = 1024; // 1 KiB
    void *ptr = kmalloc(size);

    if (ptr == 0) {
        tui_write("[FAIL] Memory allocation\n");
        return;
    } else {
        tui_write("[PASS] Memory allocation\n");
    }

    // Write to the allocated memory
    char *char_ptr = (char *)ptr;
    for (u32int i = 0; i < size; i++) {
        char_ptr[i] = (char)(i % 256);
    }

    // Verify the written data
    bool passed = true;
    for (u32int i = 0; i < size; i++) {
        if (char_ptr[i] != (char)(i % 256)) {
            passed = false;
            break;
        }
    }

    if (passed) {
        tui_write("[PASS] Memory read/write\n");
    } else {
        tui_write("[FAIL] Memory read/write\n");
    }

    // Free the allocated memory
    kfree(ptr);
    tui_write("[INFO] Memory freed\n");
}

static void sleep_test(int argc, cstring argv[]) {  
    if (argc != 2 && argc != 3) {
        tui_write("Usage: sleep [-m|-s|-M|-h] <value>\n");
        tui_write("  -m: milliseconds (default)\n");
        tui_write("  -s: seconds\n");
        tui_write("  -M: minutes\n");
        tui_write("  -h: hours\n");
        return;
    }

    u32int multiplier = 1;
    cstring duration_str;

    // Determine time unit and duration argument.
    if (argc == 3) {
        duration_str = argv[2];

        if (kstrcmp(argv[1], "-m") == 0) {
            multiplier = 1;
        } else if (kstrcmp(argv[1], "-s") == 0) {
            multiplier = 1000;
        } else if (kstrcmp(argv[1], "-M") == 0) {
            multiplier = 60000;
        } else if (kstrcmp(argv[1], "-h") == 0) {
            multiplier = 3600000;
        } else {
            tui_write("Error: Invalid time unit.\n");
            return;
        }
    } else {
        // No flag: interpret duration as milliseconds.
        duration_str = argv[1];
    }

    // Validate and parse duration.
    if (duration_str[0] == '\0') {
        tui_write("Error: Duration is empty.\n");
        return;
    }

    u32int value = 0;

    for (u32int i = 0; duration_str[i] != '\0'; i++) {
        char c = duration_str[i];

        if (c < '0' || c > '9') {
            tui_write("Error: Invalid duration.\n");
            return;
        }

        u32int digit = c - '0';

        // Prevent integer overflow during parsing.
        if (value > (0xFFFFFFFFu - digit) / 10) {
            tui_write("Error: Duration is too large.\n");
            return;
        }

        value = value * 10 + digit;
    }

    // Prevent overflow when converting to milliseconds.
    if (value > 0xFFFFFFFFu / multiplier) {
        tui_write("Error: Duration is too large.\n");
        return;
    }

    u32int milliseconds = value * multiplier;

    char buffer[12];

    uint_to_string(milliseconds, buffer);
    tui_write("Sleeping for ");
    tui_write(buffer);
    tui_write(" milliseconds...\n");

    // Measure actual elapsed time.
    u32int start = pit_get_uptime_ms();

    pit_sleep_ms(milliseconds);

    u32int elapsed = pit_get_uptime_ms() - start;

    uint_to_string(elapsed, buffer);
    tui_write("Woke up after ");
    tui_write(buffer);
    tui_write(" milliseconds.\n");
}



static void ticks(int argc, cstring argv[]) {
    (void)argc;
    (void)argv;

    u32int ticks = pit_get_ticks();
    u32int uptime_ms = pit_get_uptime_ms();

    char buffer[64];

    uint_to_string(ticks, buffer);
    tui_write("PIT Ticks: ");
    tui_write(buffer);
    tui_write("\n");

    uint_to_string(uptime_ms, buffer);
    tui_write("System Uptime: ");
    tui_write(buffer);
    tui_write(" ms\n");
}

static void shell_cmd_shutdown(int argc, cstring argv[]) {
    (void)argv;

    if (argc != 1) {
        tui_write("Usage: shutdown\n");
        return;
    }

    tui_write("Shutting down OS2...\n");
    system_shutdown_qemu();
}

static void shell_cmd_pagetest(int argc, cstring argv[]) {
    (void)argc;
    (void)argv;

    tui_write("Testing virtual memory mapping...\n");

    // Allocate a physical frame
    u32int frame = pmm_alloc_frame();

    if (frame == 0) {
        tui_write("[FAIL] Physical frame allocation\n");
        return;
    }

    // Map it at a virtual address outside the identity-mapped region
    u32int virtual_addr = 0x08000000;

    if (!paging_map(virtual_addr, frame, PAGE_WRITABLE)) {
        tui_write("[FAIL] Virtual memory mapping\n");
        pmm_free_frame(frame);
        return;
    }

    tui_write("[PASS] Virtual memory mapping\n");

    // Write and read through the virtual address
    volatile u32int *ptr = (volatile u32int *)virtual_addr;

    *ptr = 0x12345678;

    if (*ptr == 0x12345678) {
        tui_write("[PASS] Virtual memory read/write\n");
    } else {
        tui_write("[FAIL] Virtual memory read/write\n");
    }

    u32int physical;

    // Remove the mapping
    if (paging_unmap(virtual_addr, &physical)) {
        tui_write("[PASS] Virtual memory unmapping\n");
    } else {
        tui_write("[FAIL] Virtual memory unmapping\n");
        return; // Don't free a frame that might still be mapped.
    }

    // Return the physical frame to the PMM
    if (pmm_free_frame(frame)) {
        tui_write("[PASS] Physical frame release\n");
    } else {
        tui_write("[FAIL] Physical frame release\n");
    }

    tui_write("Paging test complete.\n");
}

static void shell_cmd_memtest(int argc, cstring argv[]) {
    (void)argc;
    (void)argv;

    tui_write("\nOS2 Physical Memory Test\n");

    u32int before = pmm_get_free_frames();

    // Allocate two frames
    u32int frame1 = pmm_alloc_frame();
    u32int frame2 = pmm_alloc_frame();

    bool passed = true;

    // Check that allocations succeeded
    if (frame1 == 0 || frame2 == 0) {
        tui_write("[FAIL] Frame allocation\n");
        passed = false;
    } else {
        tui_write("[PASS] Frame allocation\n");
    }

    // Check that frames are different
    if (frame1 != 0 && frame2 != 0 && frame1 != frame2) {
        tui_write("[PASS] Unique frame addresses\n");
    } else {
        tui_write("[FAIL] Unique frame addresses\n");
        passed = false;
    }

    // Check 4 KiB alignment
    if (frame1 != 0 && frame2 != 0 &&
        frame1 % 4096 == 0 && frame2 % 4096 == 0) {
        tui_write("[PASS] Frame alignment\n");
    } else {
        tui_write("[FAIL] Frame alignment\n");
        passed = false;
    }

    // Verify bitmap changes
    u32int after = pmm_get_free_frames();
    u32int successful = (frame1 != 0) + (frame2 != 0);

    if (before >= successful && after == before - successful) {
        tui_write("[PASS] Bitmap allocation tracking\n");
    } else {
        tui_write("[FAIL] Bitmap allocation tracking\n");
        passed = false;
    }

    if (passed) {
        tui_write("\nAll memory tests passed!\n");
    } else {
        tui_write("\nMemory test failed!\n");
    }

    // cleanup

    if (frame1 != 0)
        pmm_free_frame(frame1);

    if (frame2 != 0 && frame2 != frame1)
        pmm_free_frame(frame2);
}

void shell_meminfo(int argc, cstring argv[]) {
    (void)argc;
    (void)argv;

    u32int free_frames = pmm_get_free_frames();
    u32int total_memory = 128 * 1024 * 1024; // 128 MB
    u32int used_memory = total_memory - (free_frames * 4096); // Each frame is 4 KB

    char buffer[64];

    tui_write("Memory Information:\n");
    
    uint_to_string(total_memory / (1024 * 1024), buffer);
    tui_write("Total Memory Capacity: ");
    tui_write(buffer);
    tui_write(" MB\n");

    uint_to_string(used_memory / (1024 * 1024), buffer);
    tui_write("Unavailable / Unallocated Memory: ");
    tui_write(buffer);
    tui_write(" MB\n");

    uint_to_string(free_frames * 4096 / (1024 * 1024), buffer);
    tui_write("Free Memory: ");
    tui_write(buffer);
    tui_write(" MB\n");
}

void shell_history_add(cstring command) {
    if (command == 0 || command[0] == '\0')
        return;

    // If full, shift every command one position left.
    if (history_count >= SHELL_HISTORY_SIZE)
    {
        for (u32int i = 1; i < SHELL_HISTORY_SIZE; i++)
        {
            for (u32int j = 0; j < SHELL_BUFFER_SIZE; j++)
            {
                history[i - 1][j] = history[i][j];
            }
        }

        history_count--;
    }

    // Copy the new command into the next available slot.
    u32int i = 0;

    while (command[i] != '\0' && i < SHELL_BUFFER_SIZE - 1)
    {
        history[history_count][i] = command[i];
        i++;
    }

    history[history_count][i] = '\0';

    history_count++;
}

cstring shell_history_get(u32int index) {
    if (index >= history_count)
        return 0;

    return history[index];
}

u32int shell_history_count(void) {
    return history_count;
}

void shell_execute(string input) {
    cstring argv[SHELL_MAX_ARGS];

    int argc = kstrsplit(input, argv, SHELL_MAX_ARGS);

    if (argc == 0) return;

    // char buffer[12]; // Buffer to hold the string representation of the index

    tui_write("Command: ");
    for (int i = 0; i < argc; i++) {
        // tui_write(argv[i]);
        // tui_write(" ");
        // tui_write("index: "); // Print the index of the argument
        // uint_to_string(i, buffer);
        // tui_write(buffer);
        // tui_write(")");

        if (i < argc - 1) {
            tui_write(" ");
        }
    }
    tui_write("\n");

    for (u32int i = 0; i < COMMAND_COUNT; i++) {
        if (kstrcmp(argv[0], commands[i].name) == 0)
        {
            commands[i].handler(argc, argv);
            return;
        }
    }

    tui_write("Unknown command: ");
    tui_write(argv[0]);
    tui_write("\n");
}

void shell_init(void) {
    char buffer[SHELL_BUFFER_SIZE];

    tui_write("Welcome to OS2!\n");

    for (;;)
    {
        tui_write("> ");

        input_action_t action = readline_history (
            buffer,
            sizeof(buffer),
            shell_history_get,
            shell_history_count()
        );

        if (action == INPUT_SUBMITTED)
        {
            if (buffer[0] != '\0')
            {
                shell_history_add(buffer);
                shell_execute(buffer);
            }
        }
    }
}

static void shell_help(int argc, cstring argv[]) {
    if (argc > 1) {
        for (u32int i = 0; i < COMMAND_COUNT; i++) {

            if (kstrcmp(argv[1], commands[i].name) == 0) {
                tui_write(commands[i].name);
                tui_write(" - ");
                tui_write(commands[i].description);
                tui_write("\n");
                return;
            }
        }

        tui_write("Unknown command: ");
        tui_write(argv[1]);
        tui_write("\n");
        return;
    }

    tui_write("Available commands:\n");

    u32int command_count = sizeof(commands) / sizeof(commands[0]);

    for (u32int i = 0; i < command_count; i++) {
        tui_write("  ");
        tui_write(commands[i].name);
        tui_write(" - ");
        tui_write(commands[i].description);
        tui_write("\n");
    }
}

static void shell_clear(int argc, cstring argv[]) {
    (void)argc;
    (void)argv;

    tui_clear();
}

static void shell_version(int argc, cstring argv[]) {
    (void)argc;
    (void)argv;

    tui_write(VERSION);
}

void keyboard_test(int argc, cstring argv[]) {
    (void)argc;
    (void)argv;

    char buffer[SHELL_BUFFER];
    tui_write("Press keys to see their scancodes. Press ESC to exit.\n");
    
    for (;;) {
        readline(buffer, SHELL_BUFFER);
        
        if (kstrcmp(buffer, "ESC") == 0) {
            tui_write("Exiting keyboard test...\n");
            break;
        }
        
        tui_write("Scancode: ");
        tui_write(buffer);
        tui_write("\n");
    }
}

void test_div0(int argc, cstring argv[]) {
    (void)argc;
    (void)argv;
    
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

static void shell_echo(int argc, cstring argv[]) {
    bool newline = true;
    int start = 1;

    if (argc > 1 && kstrcmp(argv[1], "-n") == 0) {
        newline = false;
        start = 2;
    }

    for (int i = start; i < argc; i++)
    {
        tui_write(argv[i]);

        if (i < argc - 1)
            tui_putchar(' ');
    }

    if (newline)
        tui_newline();
}