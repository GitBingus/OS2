#ifndef SHELL_H
#define SHELL_H

#define VERSION (cstring)"0.1"

#include "kstring.h"

typedef void (*shell_command_fn)(int argc, cstring argv[]);

typedef struct {
    cstring name;
    cstring description;
    shell_command_fn handler;
} shell_command_t;

void shell_init(void);
void shell_execute(string input);

void keyboard_test(int argc, cstring argv[]);
void test_div0(int argc, cstring argv[]);

int kstrsplit(string input, cstring argv[], int max_args);

void shell_history_add(cstring command);
cstring shell_history_get(u32int index);
u32int shell_history_count(void);
#endif