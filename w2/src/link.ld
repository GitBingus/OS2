ENTRY(loader)

SECTIONS {
    . = 0x00100000; /* Kernel starts at 1 MiB */

    /* Mark the beginning of kernel memory */
    kernel_start = .;

    .text ALIGN(0x1000) : {
        *(.text)
        *(.text*)
    }

    .rodata ALIGN(0x1000) : {
        *(.rodata*)
    }

    .data ALIGN(0x1000) : {
        *(.data)
        *(.data*)
    }

    .bss ALIGN(0x1000) : {
        *(COMMON)
        *(.bss)
        *(.bss*)
    }

    /* Align the end to a 4 KiB frame boundary */
    . = ALIGN(0x1000);

    /* Mark the end of kernel memory */
    kernel_end = .;
}