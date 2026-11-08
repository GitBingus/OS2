#include "shell.h"
#include "tui.h" 
#include "kstring.h"
#include "input.h"
#include "types.h"

#define SHELL_BUFFER 128
#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

static void shell_help(void);
static void shell_clear(void);
static void shell_version(void);

static const shell_command_t commands[] = {
    {"help",    "Show available commands",  shell_help},
    {"clear",   "Clear the screen",         shell_clear},
    {"version", "Show OS2 version",         shell_version}
};

void shell_execute(cstring input) {
    if (input[0] == '\0')
        return;

    u32int command_count = sizeof(commands) / sizeof(commands[0]);

    for (u32int i = 0; i < command_count; i++)
    {
        if (kstrcmp(input, commands[i].name) == 0)
        {
            commands[i].handler();
            return;
        }
    }

    tui_write("Unknown command: ");
    tui_write(input);
    tui_write("\n");
}

void shell_init(void) {
    char buffer[SHELL_BUFFER];
    
    tui_write("Welcome to OS2!\n");
    tui_write("Keyboard initialised.\n\n");

    for (;;) {
        tui_write("> ");
        readline(buffer, SHELL_BUFFER);

        shell_execute(buffer);
        tui_write("\n");
    }
}

static void shell_help(void) {
    tui_write("Available commands:\n");

    u32int command_count = sizeof(commands) / sizeof(commands[0]);

    for (u32int i = 0; i < command_count; i++)
    {
        tui_write("  ");
        tui_write(commands[i].name);
        tui_write(" - ");
        tui_write(commands[i].description);
        tui_write("\n");
    }
}

static void shell_clear(void) {
    tui_clear();
}

static void shell_version(void) {
    tui_write(VERSION);
}

void keyboard_test(void) {
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