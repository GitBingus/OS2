global loader                   ; loader is the entry point for the bootloader

MAGIC_NUMBER equ 0x1BADB002     ; defines the magic number for the multiboot header
ALIGN_MODULES equ 0x00000003    ; tell GRUB to align modules, Here, 0x03 requests page-aligned boot modules (bit 0) and basic memory information (bit 1)
VIDEO_MODE equ 0x00000004               ; defines video mode 
FLAGS equ ALIGN_MODULES | VIDEO_MODE
CHECKSUM equ -(MAGIC_NUMBER + FLAGS)    ; calculates checksum

KERNEL_STACK_SIZE equ 4096      ; defines the size of the kernel stack in bytes
extern kmain                    ; declares the external C kernel entry point

section .bss                
    align 16                    ; aligns the kernel stack to a 16-byte boundary
    kernel_stack:
        resb KERNEL_STACK_SIZE  ; reserves space for the kernel stack

section .text
    align 4                     ; aligns the multiboot header to a 4-byte boundary
    dd MAGIC_NUMBER             ; defines the magic number for the multiboot header
    dd FLAGS                    ; defines the flags for the multiboot header
    dd CHECKSUM                 ; defines the checksum for the multiboot header

    ; Address fields (not used)
    dd 0                        ; header_addr
    dd 0                        ; load_addr
    dd 0                        ; load_end_addr
    dd 0                        ; bss_end_addr
    dd 0                        ; entry_addr

    dd 0                        ; Linear graphics mode
    dd 800                      ; Width
    dd 600                      ; Height
    dd 32                       ; Bits per pixel

loader:
    mov esp, kernel_stack + KERNEL_STACK_SIZE   ; sets the stack pointer to the top of the kernel stack
    push ebx

    call kmain                                  ; call the C kernel entry point

    add esp, 4

.loop:
    jmp .loop               ; infinite loop to prevent the bootloader from returning