#include "io.h"
#include "tui.h"
#include "kbool.h"
#include "keyboard.h"

// Persistent keyboard state
static bool extended = false;
static bool left_shift_pressed = false;
static bool right_shift_pressed = false;
static bool caps_lock = false;

// Check whether the PS/2 controller has data available
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


// Create an empty keyboard event
static keyboard_event_t keyboard_no_event(void)
{
    keyboard_event_t event;

    event.key = KEY_NONE;
    event.character = 0;

    return event;
}


// Process a single scancode
keyboard_event_t keyboard_process_scancode(unsigned char scan_code)
{
    keyboard_event_t event = keyboard_no_event();

    // Extended key prefix
    if (scan_code == 0xE0)
    {
        extended = true;
        return event;
    }

    // Handle extended keys, including arrow keys
    if (extended)
    {
        extended = false;

        // Ignore key releases
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

    // Left Shift pressed/released
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

    // Right Shift pressed/released
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

    // Toggle Caps Lock when pressed
    if (scan_code == 0x3A)
    {
        caps_lock = !caps_lock;
        return event;
    }

    // Ignore other key-release events
    if (scan_code & 0x80)
        return event;

    // Handle special keys
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

    // Convert a normal key press into ASCII
    char character = scan_code_to_ascii(scan_code);

    if (character == 0)
        return event;

    bool shift_pressed = left_shift_pressed || right_shift_pressed;

    // Apply Shift and Caps Lock to letters
    if (character >= 'a' && character <= 'z')
    {
        if (shift_pressed != caps_lock)
            character -= ('a' - 'A');
    }

    event.key = KEY_CHARACTER;
    event.character = character;

    return event;
}


// Poll the controller and return any newly received keyboard event
keyboard_event_t keyboard_poll(void)
{
    if (!keyboard_has_data())
        return keyboard_no_event();

    unsigned char scan_code = read_scan_code();

    return keyboard_process_scancode(scan_code);
}