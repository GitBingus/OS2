#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KBD_DATA_PORT       0x60
#define KBD_STATUS_PORT     0x64

#include "kbool.h"
#include "types.h"

/* Keyboard only supports US style QWERTY -- May add UK style, Devorak, Colemak etc in future */

#include "types.h"

static const char keymap_normal[128] = {
    // Number row
    [0x02] = '1',
    [0x03] = '2',
    [0x04] = '3',
    [0x05] = '4',
    [0x06] = '5',
    [0x07] = '6',
    [0x08] = '7',
    [0x09] = '8',
    [0x0A] = '9',
    [0x0B] = '0',
    [0x0C] = '-',
    [0x0D] = '=',

    // QWERTY row
    [0x10] = 'q',
    [0x11] = 'w',
    [0x12] = 'e',
    [0x13] = 'r',
    [0x14] = 't',
    [0x15] = 'y',
    [0x16] = 'u',
    [0x17] = 'i',
    [0x18] = 'o',
    [0x19] = 'p',
    [0x1A] = '[',
    [0x1B] = ']',

    // ASDF row
    [0x1E] = 'a',
    [0x1F] = 's',
    [0x20] = 'd',
    [0x21] = 'f',
    [0x22] = 'g',
    [0x23] = 'h',
    [0x24] = 'j',
    [0x25] = 'k',
    [0x26] = 'l',
    [0x27] = ';',
    [0x28] = '\'',
    [0x29] = '`',
    [0x2B] = '\\',

    // ZXCV row
    [0x2C] = 'z',
    [0x2D] = 'x',
    [0x2E] = 'c',
    [0x2F] = 'v',
    [0x30] = 'b',
    [0x31] = 'n',
    [0x32] = 'm',
    [0x33] = ',',
    [0x34] = '.',
    [0x35] = '/',

    // Space
    [0x39] = ' ',

    // Numpad (assuming Num Lock is enabled)
    [0x37] = '*',
    [0x47] = '7',
    [0x48] = '8',
    [0x49] = '9',
    [0x4A] = '-',
    [0x4B] = '4',
    [0x4C] = '5',
    [0x4D] = '6',
    [0x4E] = '+',
    [0x4F] = '1',
    [0x50] = '2',
    [0x51] = '3',
    [0x52] = '0',
    [0x53] = '.'
};

static const char keymap_shift[128] = {
    // Number row symbols
    [0x02] = '!',
    [0x03] = '@',
    [0x04] = '#',
    [0x05] = '$',
    [0x06] = '%',
    [0x07] = '^',
    [0x08] = '&',
    [0x09] = '*',
    [0x0A] = '(',
    [0x0B] = ')',
    [0x0C] = '_',
    [0x0D] = '+',

    // QWERTY row
    [0x10] = 'Q',
    [0x11] = 'W',
    [0x12] = 'E',
    [0x13] = 'R',
    [0x14] = 'T',
    [0x15] = 'Y',
    [0x16] = 'U',
    [0x17] = 'I',
    [0x18] = 'O',
    [0x19] = 'P',
    [0x1A] = '{',
    [0x1B] = '}',

    // ASDF row
    [0x1E] = 'A',
    [0x1F] = 'S',
    [0x20] = 'D',
    [0x21] = 'F',
    [0x22] = 'G',
    [0x23] = 'H',
    [0x24] = 'J',
    [0x25] = 'K',
    [0x26] = 'L',
    [0x27] = ':',
    [0x28] = '"',
    [0x29] = '~',
    [0x2B] = '|',

    // ZXCV row
    [0x2C] = 'Z',
    [0x2D] = 'X',
    [0x2E] = 'C',
    [0x2F] = 'V',
    [0x30] = 'B',
    [0x31] = 'N',
    [0x32] = 'M',
    [0x33] = '<',
    [0x34] = '>',
    [0x35] = '?',

    // Space
    [0x39] = ' ',

    // Numpad operators (unaffected by Shift)
    [0x37] = '*',
    [0x4A] = '-',
    [0x4E] = '+'
};

typedef enum {
    KEY_NONE,
    KEY_CHARACTER,
    KEY_ENTER,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT
} keyboard_key_t;   // custom enum to simplify input types

typedef struct {
    keyboard_key_t key;
    char character;
} keyboard_event_t;

extern unsigned int cursor;

// Returns true when the keyboard controller has data
bool keyboard_has_data(void);

// Processes a single raw scancode
keyboard_event_t keyboard_process_scancode(unsigned char scan_code);

// Polls the keyboard and returns an event, if one is available
keyboard_event_t keyboard_poll(void);

u32int keyboard_get_scancode_log(u8int *buffer, u32int max_count);

// Constantly monitors kb
void monitor_kb(void);

bool keyboard_dequeue(keyboard_event_t *event);
void keyboard_irq_handler(void);

#endif