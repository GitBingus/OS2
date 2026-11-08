#ifndef SHELL_H
#define SHELL_H

#define VERSION (cstring)"0.1"

#include "kstring.h"

typedef void (*shell_command_fn)(void);

typedef struct {
    cstring name;
    cstring description;
    shell_command_fn handler;
} shell_command_t;

void shell_init(void);
void shell_execute(cstring input);

void keyboard_test(void);
void test_div0(void);

#endif