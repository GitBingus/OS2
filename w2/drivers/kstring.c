#include "kstring.h"

void uint_to_string(unsigned int value, char *buffer)
{
    char reversed[10];
    unsigned int count = 0;
    unsigned int output = 0;

    if (value == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0)
    {
        reversed[count++] = (char)('0' + (value % 10));
        value /= 10;
    }

    while (count > 0)
    {
        buffer[output++] = reversed[--count];
    }

    buffer[output] = '\0';
}
