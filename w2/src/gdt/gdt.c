#include "io.h"
#include "gdt.h"


struct gdt_entry    gdt_init[3];
struct gdtr_entry   gdtr_init;


static inline void setGDTEntry(
    int i,              // Index of the GDT descriptor to configure
    unsigned int base,  // 32-bit base address of the segment
    unsigned int limit, // 20-bit segment limit
    char access,        // 8-bit access byte (permissions and privilege)
    char flags          // 4-bit flags (G, D/B, L, AVL)
) {
    // BASE ADDRESS (32 bits)

    // 0xFFFF = 16 bits set to 1
    // Bitwise AND (&) keeps only bits 0–15 of the base.
    gdt_init[i].base_low = base & 0xFFFF;

    // Right shift (>>) by 16 moves bits 16–23 into positions 0–7.
    // 0xFF = 8 bits set to 1.
    // Bitwise AND removes every bit except those lowest 8 bits.
    gdt_init[i].base_mid = (base >> 16) & 0xFF;

    // Right shift by 24 moves bits 24–31 into positions 0–7.
    // AND with 0xFF keeps only those 8 bits.
    gdt_init[i].base_high = (base >> 24) & 0xFF;


    // SEGMENT LIMIT (20 bits)

    // AND with 0xFFFF extracts the lowest 16 bits (bits 0–15).
    gdt_init[i].limit_low = limit & 0xFFFF;

    // The granularity byte contains two separate 4-bit values:
    //
    // Bits 0–3:  Upper 4 bits of the segment limit (bits 16–19).
    // Bits 4–7:  Descriptor flags (AVL, L, D/B, G).
    //
    // (limit >> 16):
    // Moves limit bits 16–19 into positions 0–3.
    //
    // & 0x0F:
    // Keeps only the lowest 4 bits, discarding everything else.
    //
    // (flags & 0x0F):
    // Ensures only the lowest 4 flag bits are used.
    //
    // << 4:
    // Moves the four flag bits into positions 4–7.
    //
    // | (bitwise OR):
    // Combines both 4-bit values into one 8-bit byte.
    gdt_init[i].granularity =
        ((limit >> 16) & 0x0F) |
        ((flags & 0x0F) << 4);


    // ACCESS BYTE (8 bits)

    // Copy the access byte into the descriptor.
    // Examples:
    // 0x9A = Ring 0 kernel code (readable + executable)
    // 0x92 = Ring 0 kernel data (readable + writable)
    gdt_init[i].access = access;
}

void setGDT(void) {
    setGDTEntry(0, 0, 0, 0x00, 0x00);       // initialise null descriptor
    setGDTEntry(1, 0, 0xFFFFF, 0x9A, 0xC);  // init kernel code  
    setGDTEntry(2, 0, 0xFFFFF, 0x92, 0xC);  // init kernel data

    gdtr_init.limit     = sizeof(gdt_init) - 1;     // sets GDTR limit
    gdtr_init.base      = (unsigned int)&gdt_init;  // sets base address

    lgdt_init(&gdtr_init);      // loads the GDT
}