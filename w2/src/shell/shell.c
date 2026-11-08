#include "shell.h"
#include "tui.h" 
#include "kstring.h"
#include "input.h"

#define SHELL_BUFFER 128

void shell_execute(const char *input) {
    if (kstrcmp(input, "help") == 0) {
        tui_write("Available commands:\n");
        tui_write("help - Show this help message\n");
        tui_write("clear - Clear the screen\n");
        tui_write("exit - Exit the shell\n");
    }
    else if (kstrcmp(input, "clear") == 0) {
        tui_clear();
    }

    else if (kstrcmp(input, "exit") == 0) {
        tui_write("Exiting shell...\n");
    }

    else if (kstrcmp(input, "keytest") == 0) {
        tui_write("Keyboard test started. Press keys to see their scancodes. Press ESC to exit.\n");
        keyboard_test();
    }

    else if (kstrcmp(input, "divide by 0 test") == 0) {
        tui_write("Testing division-by-zero exception...\n");
        test_div0();
    }

    else if (input[0] == '\0') {
        return;
    }

    else {
        tui_write("Unknown command: ");
        tui_write(input);
        tui_write("\n");
    }
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