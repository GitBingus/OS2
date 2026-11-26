#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H
#include "types.h"

void serial_init(void);
void serial_write_char(char c);
void serial_write(const char *str);
void serial_write_u32(u32int value);

#include "types.h"

void outb(unsigned short port, unsigned char data);

unsigned char read_scan_code(void);
char scan_code_to_ascii(unsigned char scan_code);

u8int inb(u16int port);
void outw(u16int port, u16int value);
u16int inw(u16int port);

#endif