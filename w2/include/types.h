#ifndef TYPES_H
#define TYPES_H

// unsigned

typedef unsigned char  u8int;
typedef unsigned short u16int;
typedef unsigned int   u32int;

// signed

typedef signed char    s8int;
typedef signed short   s16int;
typedef signed int     s32int;

// compile time size checks

_Static_assert(sizeof(u8int) == 1, "u8int must be 1 byte");
_Static_assert(sizeof(u16int) == 2, "u16int must be 2 bytes");
_Static_assert(sizeof(u32int) == 4, "u32int must be 4 bytes");

#endif