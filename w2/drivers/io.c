#include "io.h"
#include "keyboard.h"

unsigned char read_scan_code(void) {
    unsigned char scan_code;
    __asm__ volatile (
        "inb %1, %0"
        : "=a"(scan_code)
        : "Nd"(KBD_DATA_PORT)
    );
    return scan_code;
}


void outb(unsigned short port, unsigned char data) {
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(data), "Nd"(port)
    );
}

char scan_code_to_ascii(unsigned char scan_code)
{
    static const char keyboard_map[128] = {
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

        [0x1E] = 'a',
        [0x1F] = 's',
        [0x20] = 'd',
        [0x21] = 'f',
        [0x22] = 'g',
        [0x23] = 'h',
        [0x24] = 'j',
        [0x25] = 'k',
        [0x26] = 'l',

        [0x2C] = 'z',
        [0x2D] = 'x',
        [0x2E] = 'c',
        [0x2F] = 'v',
        [0x30] = 'b',
        [0x31] = 'n',
        [0x32] = 'm',

        [0x39] = ' '
    };

    if (scan_code >= 128)
        return 0;

    return keyboard_map[scan_code];
}