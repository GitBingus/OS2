#include "input.h"
#include "keyboard.h"
#include "tui.h"
#include "display.h"
#include "io.h"


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

static void input_redraw_line(
    cstring buffer,
    u32int input_start,
    u32int previous_length,
    u32int length,
    u32int cursor
) {
    // Move to the beginning of the editable input
    display_set_cursor(input_start);

    // Redraw the current text
    for (u32int i = 0; i < length; i++) {
        display_putchar(buffer[i]);
    }

    // Erase any characters left over from the old text
    for (u32int i = length; i < previous_length; i++) {
        display_putchar(' ');
    }

    // Restore the editing cursor
    display_set_cursor(input_start + cursor);
}

int getc(void) {
    keyboard_event_t event;

    static u32int last_irq_count = 0;
    
    for (;;) {
        if (keyboard_irq_count != last_irq_count) {
            last_irq_count = keyboard_irq_count;
        }
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

    display_set_cursor(input_start);

    for (u32int i = 0; i < *length; i++) {
        display_putchar(' ');
    }

    display_set_cursor(input_start);
    
    u32int i = 0;

    while (i < size - 1 && replacement[i] != '\0') {
        buffer[i] = replacement[i];
        i++;
    }

    buffer[i] = '\0';

    *length = i;
    *cursor = i;

    display_set_cursor(input_start);

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
    
    u32int input_start = display_get_cursor();
    u32int index = 0;
    u32int cursor = 0;
    u32int history_index = history_count;

    char draft[128];
    u32int draft_length = 0;
    bool draft_saved = false;

    (void)draft_length; // It is being used in the code, but the compiler may not recognise it.
    
    // Stop when Enter is pressed.
    for (;;) {
        int c = getc();
        
        if (c == INPUT_KEY_LEFT) {
            if (cursor > 0) {
                cursor--;
                display_set_cursor(input_start + cursor);
            }
            continue;
        }
        
        if (c == INPUT_KEY_RIGHT) {
            if (cursor < index) {
                cursor++;
                display_set_cursor(input_start + cursor);
            }
            continue;
        }
        
        if (c == INPUT_KEY_UP) {
            if (history_get == 0 || history_count == 0)
                continue;

            // Save whatever the user had typed before entering history
            if (history_index == history_count && !draft_saved) {
                u32int i = 0;

                while (i < size - 1 && i < index && buffer[i] != '\0') {
                    draft[i] = buffer[i];
                    i++;
                }

                draft[i] = '\0';
                draft_length = i;
                draft_saved = true;
            }

            if (history_index > 0) {
                history_index--;

                cstring previous = history_get(history_index);
                if (previous != 0) {
                    input_replace_line(
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

        if (c == INPUT_KEY_DOWN) {
            if (history_get == 0 || history_index >= history_count)
                continue;

            history_index++;

            if (history_index == history_count) {
                // Restore the line that existed before history navigation
                input_replace_line(
                    buffer,
                    size,
                    draft_saved ? draft : "",
                    input_start,
                    &index,
                    &cursor
                );
            } else {
                cstring next = history_get(history_index);

                if (next != 0) {
                    input_replace_line(
                        buffer,
                        size,
                        next,
                        input_start,
                        &index,
                        &cursor
                    );
                }
            }

            continue;
        }
                
        if (c == '\n') {
            buffer[index] = '\0';
            display_newline();
            return INPUT_SUBMITTED;
        }
        
        else if (c == '\b') {
            if (cursor > 0) {
                (void)draft_length; // It is being used in the code, but the compiler may not recognise it.

                u32int previous_length = index;

                // Shift characters left, including the null terminator
                for (u32int i = cursor - 1; i < index; i++) {
                    buffer[i] = buffer[i + 1];
                }

                index--;
                cursor--;

                input_redraw_line(
                    buffer,
                    input_start,
                    previous_length,
                    index,
                    cursor
                );
            }
        }
        else if (c >= 32 && c <= 126 && index < size - 1) {
            u32int previous_length = index;

            // Shift characters right
            for (u32int i = index; i > cursor; i--) {
                buffer[i] = buffer[i - 1];
            }

            // Insert the character
            buffer[cursor] = (char)c;

            index++;
            cursor++;

            buffer[index] = '\0';

            // Redraw the modified line
            input_redraw_line(
                buffer,
                input_start,
                previous_length,
                index,
                cursor
            );
        }
    }
    // make sure the buffer is null-terminated.
    buffer[index] = '\0';
    display_newline();
    
    return INPUT_SUBMITTED;
}