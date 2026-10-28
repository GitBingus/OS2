#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

void outb(unsigned short port, unsigned char data);

unsigned char read_scan_code(void);
char scan_code_to_ascii(unsigned char scan_code);

#endif