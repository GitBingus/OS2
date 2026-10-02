%include "asm_io.inc"

segment .data

    prompt_start    db "Enter the start of the range (1-100): ", 0
    prompt_end      db "Enter the end of the range (1-100): ", 0
    error_range     db "Error: range must be between 1 and 100, and start must be <= end.", 0
    result_msg      db "The sum is: ", 0

segment .bss

    numbers resd 100       ; reserve space for 100 integers
    start   resd 1         ; user's starting number
    finish  resd 1         ; user's ending number
    total   resd 1         ; sum of the range


segment .text

    global asm_main

    asm_main:
        pusha

        ; Fill the array with numbers from 1 to 100

        mov esi, numbers
        mov ecx, 1

    fill_array:
        mov [esi], ecx
        add esi, 4
        inc ecx

        cmp ecx, 101
        jl fill_array

        ; Ask user for starting number

        mov eax, prompt_start
        call print_string

        call read_int
        mov [start], eax

        ; Ask user for ending number

        mov eax, prompt_end
        call print_string

        call read_int
        mov [finish], eax

        ; Check that start >= 1

        mov eax, [start]

        cmp eax, 1
        jl error

        ; Check that finish <= 100

        mov eax, [finish]

        cmp eax, 100
        jg error

        ; Check that start <= finish

        mov eax, [start]
        cmp eax, [finish]
        jg error

        ; Sum the range

        mov eax, [start]       ; EAX = current number
        mov ebx, [finish]      ; EBX = ending number
        mov edx, 0             ; EDX = total

    sum_loop:

        ; Add current number to total
        add edx, [numbers + eax*4 - 4]

        inc eax

        cmp eax, ebx
        jle sum_loop

        ; Print result

        mov [total], edx

        mov eax, result_msg
        call print_string

        mov eax, [total]
        call print_int
        call print_nl

        jmp done

        ; Error

    error:
        mov eax, error_range
        call print_string
        call print_nl

        ; Exit

    done:
        popa
        mov eax, 0
        ret
