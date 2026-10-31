global interrupt_handler_0
global interrupt_handler_6
global interrupt_handler_33

extern keyboard_irq_handler
extern exception_handler

;       Vector	    Hex	    Exception	                            Error code?
;       0	        0x00	Divide Error (#DE)	                    No
;       1	        0x01	Debug (#DB)	                            No
;       2	        0x02	Non-Maskable Interrupt (NMI)	        No
;       3	        0x03	Breakpoint (#BP)	                    No
;       4	        0x04	Overflow (#OF)	                        No
;       5	        0x05	Bound Range Exceeded (#BR)	            No
;       6	        0x06	Invalid Opcode (#UD)	                No
;       7	        0x07	Device Not Available (#NM)	            No
;       8	        0x08	Double Fault (#DF)	                    Yes
;       9	        0x09	Coprocessor Segment Overrun (legacy)	No
;       10	        0x0A	Invalid TSS (#TS)	                    Yes
;       11	        0x0B	Segment Not Present (#NP)	            Yes
;       12	        0x0C	Stack-Segment Fault (#SS)	            Yes
;       13	        0x0D	General Protection Fault (#GP)	        Yes
;       14	        0x0E	Page Fault (#PF)	                    Yes
;       15	        0x0F	Reserved	                            —
;       16	        0x10	x87 Floating-Point Exception (#MF)	    No
;       17	        0x11	Alignment Check (#AC)	                Yes
;       18	        0x12	Machine Check (#MC)	                    No
;       19	        0x13	SIMD Floating-Point Exception (#XM/#XF)	No
;       20	        0x14	Virtualization Exception (#VE)	        No
;       21	        0x15	Control Protection Exception (#CP)	    Yes
;       22	        0x16	Reserved	                            —
;       23	        0x17	Reserved	                            —
;       24	        0x18	Reserved	                            —
;       25	        0x19	Reserved	                            —
;       26	        0x1A	Reserved	                            —
;       27	        0x1B	Reserved	                            —
;       28	        0x1C	Hypervisor Injection Exception (#HV)	No
;       29	        0x1D	VMM Communication Exception (#VC)	    Yes
;       30	        0x1E	Security Exception (#SX)	            Yes
;       31	        0x1F	Reserved	                            —

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


%macro ISR_NOERR 1
    global interrupt_handler_%1

interrupt_handler_%1:
    push dword 0           ; Dummy error code
    push dword %1          ; Exception number
    jmp exception_common
%endmacro

%macro ISR_ERR 1
    global interrupt_handler_%1

interrupt_handler_%1:
    ; CPU has already pushed an error code
    push dword %1          ; Exception number
    jmp exception_common
%endmacro

; CPU exceptions
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR   21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_ERR   29
ISR_ERR   30
ISR_NOERR 31


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

    and esp, 0xFFFFFFF0     ; align stack to 16 bytes

    ; push args

    sub esp, 16
    mov [esp], eax
    call exception_handler

    .halt:
        cli
        hlt
        jmp .halt