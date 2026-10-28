#ifndef TUI_H
#define MACRO

#include "keyboard.h"

void tui_init(void);
void tui_putchar(char c);
void tui_write(const char *str);
void tui_handle_key(keyboard_event_t event);

void tui_move_up(void);
void tui_move_down(void);
void tui_move_left(void);
void tui_move_right(void);

void tui_newline(void);
void tui_backspace(void);
void tui_tab(void);

void tui_scroll(void);

#endif // TUI_H