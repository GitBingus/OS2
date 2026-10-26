#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

void outb(unsigned short port, unsigned char data);
void fb_move_cursor(unsigned short pos);

unsigned char read_scan_code(void);
char scan_code_to_ascii(unsigned char scan_code);

void fb_clear(unsigned char background);

#endif