%include "asm_io.inc"

segment .data
    integer1 dd 15 ;int1
    integer2 dd 6 ;int2

segment .bss
    result resd 1 ;result

segment .text
    global asm_main
    asm_main:
        pusha

        mov eax, [integer1]  ; eax = int1
        add eax, [integer2]  ; eax = int+int2

        mov [result], eax    ; result = int1 + int2
        call print_int

        popa
        mov eax, 0  ; ret 0

        leave
        ret