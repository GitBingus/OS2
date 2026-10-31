global interrupt_handler_33
extern keyboard_irq_handler

global interrupt_handler_0
extern exception_handler_0

interrupt_handler_33:
    pushad                      ; saves all GP registers

    push ds
    push es
    push fs
    push gs

    cld                         ; clears direction flag before calling C

    mov ebx, esp                ; Saves original stack pointer in ebx.
    and esp, 0xFFFFFFF0         ; Rounds ESP down to the nearest 16-byte boundary.

    call keyboard_irq_handler   ; executes C kb handler

    mov esp, ebx                ; restores the original stack pointer

    pop gs
    pop fs
    pop es
    pop ds

    popad                       ; restores registers

    iretd                       ; returns to interrupted code


interrupt_handler_0:
    pushad

    push ds
    push es
    push fs
    push gs

    ; ensure kernel data segments are loaded if necessary

    cld

    ; call C

    call exception_handler_0

    ; restore registers

    pop gs
    pop fs
    pop es
    pop ds

    popad
    iretd