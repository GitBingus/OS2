global loader                   ; loader is the entry point for the bootloader

%ifndef FB_MODE
%define FB_MODE 1
%endif

MAGIC_NUMBER equ 0x1BADB002
ALIGN_MODULES equ 0x00000003

%if FB_MODE
FLAGS equ 0x00000003
%else
FLAGS equ ALIGN_MODULES
%endif

CHECKSUM equ -(MAGIC_NUMBER + FLAGS)

KERNEL_STACK_SIZE equ 4096      ; defines the size of the kernel stack in bytes

extern kmain                    ; declares the external C kernel entry point

section .bss                
    align 16                    ; aligns the kernel stack to a 16-byte boundary
    kernel_stack:
        resb KERNEL_STACK_SIZE  ; reserves space for the kernel stack

section .text
    align 4                     ; aligns the multiboot header to a 4-byte boundary
    dd MAGIC_NUMBER             ; defines the magic number for the multiboot header
    ;dd FLAGS                    ; defines the flags for the multiboot header
    ;dd CHECKSUM                 ; defines the checksum for the multiboot header

%if FB_MODE
    dd 0                        ; mode 0 = graphics
    dd 1024                     ; requested width
    dd 768                      ; requested height
    dd 32                       ; requested depth
%endif

loader:
    mov esp, kernel_stack + KERNEL_STACK_SIZE   ; sets the stack pointer to the top of the kernel stack
    push ebx

    call kmain                                  ; call the C kernel entry point

    add esp, 4

.loop:
    jmp .loop               ; infinite loop to prevent the bootloader from returning