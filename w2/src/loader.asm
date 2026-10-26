global loader               ; loader is the entry point for the bootloader

MAGIC_NUMBER equ 0x1BADB002 ; defines the magic number for the multiboot header
FLAGS equ 0x0               ; defines the flags for the multiboot header
CHECKSUM equ -MAGIC_NUMBER  ; defines the checksum for the multiboot header

KERNEL_STACK_SIZE equ 4096  ; defines the size of the kernel stack in bytes

extern kmain                ; declares the external C kernel entry point

section .bss                
    align 16                ; aligns the kernel stack to a 16-byte boundary
    kernel_stack:
        resb KERNEL_STACK_SIZE  ; reserves space for the kernel stack

section .text
    align 4                 ; aligns the multiboot header to a 4-byte boundary
    dd MAGIC_NUMBER         ; defines the magic number for the multiboot header
    dd FLAGS                ; defines the flags for the multiboot header
    dd CHECKSUM             ; defines the checksum for the multiboot header 

loader:
    mov esp, kernel_stack + KERNEL_STACK_SIZE   ; sets the stack pointer to the top of the kernel stack

    ; Call the C kernel entry point.
    call kmain                                  ; call the C kernel entry point

.loop:
    jmp .loop               ; infinite loop to prevent the bootloader from returning