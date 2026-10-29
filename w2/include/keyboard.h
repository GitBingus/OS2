#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KBD_DATA_PORT       0x60
#define KBD_STATUS_PORT     0x64

#include "kbool.h"

/* Keyboard only supports US style QWERTY -- May add UK style, Devorak, Colemak etc in future */

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

// Constantly monitors kb
void monitor_kb(void);

bool keyboard_dequeue(keyboard_event_t *event);

#endif