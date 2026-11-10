#include "shell.h"
#include "tui.h" 
#include "kstring.h"
#include "input.h"
#include "types.h"

#define SHELL_BUFFER 128
#define SHELL_MAX_ARGS 16
#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

static void shell_help(int argc, cstring argv[]);
static void shell_clear(int argc, cstring argv[]);
static void shell_version(int argc, cstring argv[]);
static void shell_echo(int argc, cstring argv[]);

static const shell_command_t commands[] = {
    {"help",    "Show available commands, use with 'help `<command>`'",     shell_help},
    {"clear",   "Clear the screen, use with 'clear'",                       shell_clear},
    {"version", "Show OS2 version, use with 'version'",                     shell_version},
    {"keytest", "Tests keyboard, use with 'keytest'",                       keyboard_test},
    {"div0",    "Tests division-by-zero exception, use with 'div0'",        test_div0},
    {"echo",    "Prints arguments, use with 'echo `options`'",              shell_echo}
};

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
    char buffer[SHELL_BUFFER];
    
    tui_write("Welcome to OS2!\n");
    tui_write("Keyboard initialised.\n\n");

    for (;;) {
        tui_write("> ");
        readline(buffer, SHELL_BUFFER);

        shell_execute(buffer);
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