#include "input.h"
#include "keyboard.h"
#include "tui.h"

input_action_t readline(string buffer, u32int size);

input_action_t readline_history(
    string buffer,
    u32int size,
    cstring (*history_get)(u32int index),
    u32int history_count
);

static input_action_t readline_internal(
    string buffer,
    u32int size,
    cstring (*history_get)(u32int),
    u32int history_count
);

static void input_replace_line(
    string buffer,
    u32int size,
    cstring replacement,
    u32int input_start,
    u32int *length,
    u32int *cursor
);

int getc(void) {
    keyboard_event_t event;
    
    for (;;) {
        if (keyboard_dequeue(&event)) {
            switch (event.key) {
                case KEY_CHARACTER:
                return (unsigned char)event.character;
                
                case KEY_ENTER:
                return '\n';
                
                case KEY_BACKSPACE:
                return '\b';
                
                case KEY_UP:
                return INPUT_KEY_UP;
                
                case KEY_DOWN:
                return INPUT_KEY_DOWN;
                
                case KEY_LEFT:
                return INPUT_KEY_LEFT;
                
                case KEY_RIGHT:
                return INPUT_KEY_RIGHT;
                
                default:
                break;
            }
        }
    }
}

keyboard_event_t getkey(void) {
    keyboard_event_t event;
    
    for (;;) {
        if (keyboard_dequeue(&event)) {
            return event;
        }
    }
}

input_action_t readline_history(
    string buffer,
    u32int size,
    cstring (*history_get)(u32int),
    u32int history_count
)
{
    return readline_internal(buffer, size, history_get, history_count);
}

input_action_t readline(string buffer, u32int size) {
    return readline_internal(buffer, size, 0, 0);
}

void input_replace_line (
    string buffer,
    u32int size,
    cstring replacement,
    u32int input_start,
    u32int *length,
    u32int *cursor
) {
    if (!buffer || size == 0 || !replacement || !length || !cursor) return;

    tui_set_cursor(input_start);
    
    for (unsigned int i = 0; i < size - 1; i++) {
        buffer[i] = replacement[i];
    }

    cursor = length;
    tui_set_cursor(input_start);

    tui_write(buffer);
}

static input_action_t readline_internal(
    string buffer,
    u32int size,
    cstring (*history_get)(u32int),
    u32int history_count
) {
    if (buffer == 0 || size == 0) {
        return INPUT_NONE;
    }
    
    u32int input_start = tui_get_cursor();
    u32int index = 0;
    u32int cursor = 0;
    
    u32int history_index = history_count;
    
    // Stop when Enter is pressed.
    for (;;) {
        int c = getc();
        
        if (c == INPUT_KEY_LEFT) {
            if (cursor > 0) {
                cursor--;
                tui_set_cursor(input_start + cursor);
            }
            continue;
        }
        
        if (c == INPUT_KEY_RIGHT) {
            if (cursor < index) {
                cursor++;
                tui_set_cursor(input_start + cursor);
            }
            continue;
        }
        
        if (c == INPUT_KEY_UP) {
            if (history_get == 0) continue;
            if (history_index > 0) {
                history_index--;
                
                cstring previous = history_get(history_index);
                
                if (previous != 0) {
                    if (previous != 0) input_replace_line (
                        buffer,
                        size,
                        previous,
                        input_start,
                        &index,
                        &cursor
                    );
                }
            }
            
            continue;
        }
        
        if (c == INPUT_KEY_DOWN)
        {
            if (history_index < history_count)
            {
                history_index++;
                
                if (history_index == history_count)
                {
                    // Restore a blank line or saved draft.
                }
                else
                {
                    cstring next = history_get(history_index);
                    
                    (void)next; // TODO: Retrieve and display the next history entry
                }
            }
            
            continue;
        }
        
        if (c == '\n') {
            buffer[index] = '\0';
            tui_newline();
            return INPUT_SUBMITTED;
        }
        
        if (c == '\b') {
            if (index > 0) {
                index--;
                tui_backspace();
            }
        }
        else if (index < size - 1) {
            buffer[index++] = (char)c;
            cursor = index;
            tui_putchar((char)c);
        }
    }
    // make sure the buffer is null-terminated.
    buffer[index] = '\0';
    tui_newline();
    
    return INPUT_SUBMITTED;
}