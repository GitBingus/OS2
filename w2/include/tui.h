#ifndef TUI_H
#define MACRO

void tui_init(void);
void tui_putchar(char c);
void tui_write(const char *str);
void tui_newline(void);
void tui_backspace(void);
void tui_handle_key(keyboard_event_t event);

#endif // MACRO