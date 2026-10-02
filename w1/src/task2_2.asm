%include "asm_io.inc"

segment .data

segment .bss
    numbers resd 100; reserve space for 100 integers


segment .text
    global asm_main
    asm_main:
        pusha

        ;; Fill the array with numbers from 1 to 100

        mov esi, numbers
        mov ecx, 1

        fill_array:
            mov [esi], ecx ;; store the current value of ecx in the array
            add esi, 4 ;; does not add '4', it adds 4 bytes to the address in esi, which is the size of a dword (32 bits)
            inc ecx ;; increment ecx to get the next number

            cmp ecx, 101 ;; compare ecx with 101 to check if we have filled the array with numbers from 1 to 100
            jl fill_array ;; if ecx is less than 101, jump back to fill_array to continue filling the array

        ;; Print the numbers in the array

        mov esi, numbers
        mov ecx, 0

        print_array:
            mov eax, [esi]
            call print_int
            call print_nl

            add esi, 4
            inc ecx

            cmp ecx, 100
            jl print_array


        mov eax, 0 ;; returns 0

        ret

    