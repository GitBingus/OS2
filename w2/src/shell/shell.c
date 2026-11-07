#include "shell.h"
#include "tui.h" 
#include "kstring.h"
#include "input.h"

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
    string buffer = "";
    
    tui_write("Welcome to OS2!\n");
    tui_write("Keyboard initialised.\n\n");

    for (;;) {
        tui_write("> ");
        readline(buffer, sizeof(buffer));

        shell_execute(buffer);
    }
}