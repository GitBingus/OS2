#ifndef KSTRING_H
#define KSTRING_H

#include "types.h"

typedef char* string;
typedef const char* cstring;

void uint_to_string(unsigned int value, char *buffer);
void byte_to_hex(u8int value, char *buffer);

#endif