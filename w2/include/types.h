#ifndef TYPES_H
#define TYPES_H

#define USIZE u32int

typedef unsigned char      u8int;
typedef signed char        s8int;

typedef unsigned short     u16int;
typedef signed short       s16int;

typedef unsigned int       u32int;
typedef signed int         s32int;

typedef unsigned long long u64int;
typedef signed long long   s64int;

// compile time size checks

_Static_assert(sizeof(u8int) == 1, "u8int must be 1 byte");
_Static_assert(sizeof(u16int) == 2, "u16int must be 2 bytes");
_Static_assert(sizeof(u32int) == 4, "u32int must be 4 bytes");
_Static_assert(sizeof(u64int) == 8, "u64int must be 8 bytes");

#endif