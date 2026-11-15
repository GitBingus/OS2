#ifndef INPUT_H
#define INPUT_H

#include "types.h"
#include "keyboard.h"
#include "kstring.h"

#define INPUT_KEY_UP    256
#define INPUT_KEY_DOWN  257
#define INPUT_KEY_LEFT  258
#define INPUT_KEY_RIGHT 259

typedef enum {
    INPUT_NONE,              // No input action
    INPUT_SUBMITTED,         // Enter pressed, input completed
    INPUT_HISTORY_PREVIOUS,  // Up arrow pressed
    INPUT_HISTORY_NEXT       // Down arrow pressed
} input_action_t;

int getc(void);
input_action_t readline(char *buffer, u32int size); /* using a buffer size argument in readline() 
                                            is important because it prevents the function
                                            from writing past the end of the supplied buffer
                                        */
keyboard_event_t getkey(void);

input_action_t readline_history (
    string buffer,
    u32int size,
    cstring (*history_get)(u32int index),
    u32int history_count
);

static input_action_t readline_internal (
    string buffer,
    u32int size,
    cstring (*history_get)(u32int),
    u32int history_count
);

static input_action_t readline_internal(
    string buffer,
    u32int size,
    cstring (*history_get)(u32int),
    u32int history_count
)

#endif