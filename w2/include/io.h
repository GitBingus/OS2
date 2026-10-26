#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

#define KBD_STATUS_PORT         0x64
#define KBD_DATA_PORT           0x60

void outb(unsigned short port, unsigned char data);

unsigned char read_scan_code(void);
char scan_code_to_ascii(unsigned char scan_code);


#endif