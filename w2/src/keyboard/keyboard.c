#include "io.h"
#include "fb.h"
#include "tui.h"
#include "kbool.h"
#include "gdt.h"
#include "keyboard.h"

// Persistent keyboard state
static bool extended = false;
static bool left_shift_pressed = false;
static bool right_shift_pressed = false;
static bool caps_lock = false;

unsigned int cursor = 0;

// Check whether the PS/2 controller has data available.
bool keyboard_has_data(void)
{
    unsigned char status;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(status)
        : "Nd"(KBD_STATUS_PORT)
    );

    return (status & 0x01) != 0;
}


// Create an empty keyboard event.
static keyboard_event_t keyboard_no_event(void)
{
    keyboard_event_t event;

    event.key = KEY_NONE;
    event.character = 0;

    return event;
}


// Process a single scancode.
keyboard_event_t keyboard_process_scancode(unsigned char scan_code)
{
    keyboard_event_t event = keyboard_no_event();

    // Extended key prefix.
    if (scan_code == 0xE0)
    {
        extended = true;
        return event;
    }

    // Handle extended keys, including arrow keys.
    if (extended)
    {
        extended = false;

        // Ignore key releases.
        if (scan_code & 0x80)
            return event;

        switch (scan_code)
        {
            case 0x48:
                event.key = KEY_UP;
                break;

            case 0x50:
                event.key = KEY_DOWN;
                break;

            case 0x4B:
                event.key = KEY_LEFT;
                break;

            case 0x4D:
                event.key = KEY_RIGHT;
                break;

            default:
                break;
        }

        return event;
    }

    // Left Shift pressed/released.
    if (scan_code == 0x2A)
    {
        left_shift_pressed = true;
        return event;
    }

    if (scan_code == 0xAA)
    {
        left_shift_pressed = false;
        return event;
    }

    // Right Shift pressed/released.
    if (scan_code == 0x36)
    {
        right_shift_pressed = true;
        return event;
    }

    if (scan_code == 0xB6)
    {
        right_shift_pressed = false;
        return event;
    }

    // Toggle Caps Lock when pressed.
    if (scan_code == 0x3A)
    {
        caps_lock = !caps_lock;
        return event;
    }

    // Ignore other key-release events.
    if (scan_code & 0x80)
        return event;

    // Handle special keys.
    switch (scan_code)
    {
        case 0x1C:
            event.key = KEY_ENTER;
            return event;

        case 0x0E:
            event.key = KEY_BACKSPACE;
            return event;

        case 0x0F:
            event.key = KEY_TAB;
            return event;

        default:
            break;
    }

    // Convert a normal key press into ASCII.
    char character = scan_code_to_ascii(scan_code);

    if (character == 0)
        return event;

    bool shift_pressed = left_shift_pressed || right_shift_pressed;

    // Apply Shift and Caps Lock to letters.
    if (character >= 'a' && character <= 'z')
    {
        if (shift_pressed != caps_lock)
            character -= ('a' - 'A');
    }

    event.key = KEY_CHARACTER;
    event.character = character;

    return event;
}


// Poll the controller and return any newly received keyboard event.
keyboard_event_t keyboard_poll(void)
{
    if (!keyboard_has_data())
        return keyboard_no_event();

    unsigned char scan_code = read_scan_code();

    return keyboard_process_scancode(scan_code);
}


void monitor_kb(void) {
    while (1)
    {
        // Read and interpret any available keyboard input
        keyboard_event_t event = keyboard_poll();

        if (event.key == KEY_NONE)
            continue;

        switch (event.key)
        {
            // Move cursor upwards
            case KEY_UP:
                if (cursor >= SCREEN_WIDTH)
                    cursor -= SCREEN_WIDTH;
                break;

            // Move cursor downwards
            case KEY_DOWN:
                if (cursor < SCREEN_WIDTH * (SCREEN_HEIGHT - 1))
                    cursor += SCREEN_WIDTH;
                break;

            // Move cursor left
            case KEY_LEFT:
                if (cursor > 0)
                    cursor--;
                break;

            // Move cursor right
            case KEY_RIGHT:
                if (cursor < (unsigned int)screen_size - 1)
                    cursor++;
                break;

            // Move to the beginning of the next line
            case KEY_ENTER:
                cursor = ((cursor / SCREEN_WIDTH) + 1) * SCREEN_WIDTH;

                if (cursor >= screen_size)
                    cursor = 0;
                break;

            // Remove the character before the cursor
            case KEY_BACKSPACE:
                if (cursor > 0)
                {
                    cursor--;

                    VGA_MEMORY[cursor] =
                        ((unsigned short)attribute << 8) | ' ';
                }
                break;

            // Advance to the next tab stop (every 4 columns)
            case KEY_TAB:
            {
                unsigned short column = cursor % SCREEN_WIDTH;
                unsigned short spaces = 4 - (column % 4);

                cursor += spaces;

                if (cursor >= screen_size)
                    cursor = 0;

                break;
            }

            // Display a regular character
            case KEY_CHARACTER:
                VGA_MEMORY[cursor] =
                    ((unsigned short)attribute << 8)
                    | (unsigned char)event.character;

                cursor++;

                if (cursor >= screen_size)
                    cursor = 0;

                break;

            case KEY_NONE:
            default:
                break;
        }

        // Update the hardware cursor after processing the event
        fb_move_cursor(cursor);
    }
}