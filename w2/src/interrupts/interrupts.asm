global interrupt_handler_0
global interrupt_handler_6
global interrupt_handler_33

extern keyboard_irq_handler
extern exception_handler

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
    push dword 0                ; Dummy error code
    push dword 0                ; Exception number
    jmp exception_common


interrupt_handler_6:
    push dword 0                ; Dummy error code
    push dword 6                ; Exception number
    jmp exception_common


exception_common:
    pushad

    push ds
    push es
    push fs
    push gs

    cld

    ; Address	    Contents
    ; [esp]	        Exception number
    ; [esp + 4]	    Error code (dummy zero)
    ; [esp + 8]	    Saved EIP
    ; [esp + 12]	Saved CS
    ; [esp + 16]	Saved EFLAGS

    mov ebx, esp
    mov eax, [esp + 48]

    ; align the stack for C

    mov ebx, esp
    and esp, 0xFFFFFFF0     ; align stack to 16 bytes

    ; push args

    sub esp, 16
    mov [esp], eax
    call exception_handler

    hlt