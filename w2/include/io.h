#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

#include "types.h"

void outb(unsigned short port, unsigned char data);

unsigned char read_scan_code(void);
char scan_code_to_ascii(unsigned char scan_code);

u8int inb(u16int port);

#endif