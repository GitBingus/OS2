#include "kstring.h"

void uint_to_string(unsigned int value, char *buffer)
{
    char reversed[10];
    unsigned int count = 0;
    unsigned int output = 0;

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0) {
        reversed[count++] = (char)('0' + (value % 10));
        value /= 10;
    }

    while (count > 0) {
        buffer[output++] = reversed[--count];
    }

    buffer[output] = '\0';
}

void byte_to_hex(u8int value, char *buffer)
{
    const char hex[] = "0123456789ABCDEF";

    buffer[0] = hex[(value >> 4) & 0x0F];
    buffer[1] = hex[value & 0x0F];
    buffer[2] = '\0';
}

int kstrcmp(cstring str1, cstring str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return 1;   // If characters are different, return 1
        }
        str1++;
        str2++;
    }
    return 0; // If both strings are equal, return 0
}

int kstrsplit(string input, cstring argv[], int max_args) {
    int argc = 0;
    char *p = input;

    while (*p != '\0' && argc < max_args)
    {
        // skip leading whitespace
        while (*p == ' ' || *p == '\t')
            p++;

        if (*p == '\0')
            break;

        // start of an argument
        argv[argc++] = p;

        // find the end of the argument
        while (*p != '\0' && *p != ' ' && *p != '\t')
            p++;

        // replace separator with string terminator
        if (*p != '\0')
        {
            *p = '\0';
            p++;
        }
    }

    return argc;
}