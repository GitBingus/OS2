#include "input.h"
#include "keyboard.h"
#include "tui.h"

char getc(void) {
    keyboard_event_t event;
    
    for (;;) {
        if (keyboard_dequeue(&event)) {
            if (event.key == KEY_CHARACTER) {
                return event.character;
                
            } else if (event.key == KEY_ENTER) {
                return '\n';
                
            } else if (event.key == KEY_BACKSPACE) {
                // Handle backspace by returning a special character or
                // implementing a backspace behavior in the calling function.
                return '\b';
            }
        }
    }
}

void readline(char *buffer, u32int size) {
    if (buffer == 0 || size == 0) {
        return;
    }
    
    u32int index = 0;
    
    // Stop when Enter is pressed.
    for (;;) {
        char c = getc();
        if (c == '\n') {
            break;
        }
        
        if (index < size - 1) {
            if (c == '\b') {
                if (index > 0) {
                    index--;
                    tui_backspace();
                }
            } // move back one position in the buffer and erase the character from the terminal
            else {
                buffer[index++] = c;
                tui_putchar(c); // Echo the character to the terminal
            }
        }
    }
    // make sure the buffer is null-terminated.
    buffer[index] = '\0';
    tui_newline();
}