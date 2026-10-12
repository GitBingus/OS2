%include "asm_io.inc"

segment .data
    ; data init

    username    db      "Enter your name: ", 0
    repeat      db      "Enter number of times to loop your name: ", 0

    error       db      "Error, repeat number not in range or 50 - 100 (50 < repeat < 100)", 0

segment .bss
    ; data that is not init

    counter resd 1
    name resb 100


segment .text
    global asm_main

    asm_main:   
        pusha

        ;;;

        mov eax, username   ; print out username
        call print_string

        mov edi, name

        read_name:
            call read_char

            cmp eax, 10          ; Enter / newline
            je  end_name

            mov [edi], al        ; store character
            inc edi              ; move to next byte
            jmp read_name

        end_name:
            mov byte [edi], 0    ; null terminate the string


        mov eax, repeat     ; prints out second prompt
        call print_string

        call read_int       ; reads int input
        mov [counter], eax

        ;; test if 50 < repeat < 100

        mov ebx, [counter]  ; moves the counter into ebx for comp
        cmp ebx, 50         ; comp the counter to 50

        jle errorblock      ; errors the program if less than 50

        cmp ebx, 100        ; comp the counter to 100

        jge errorblock       ; errors the program if larger than 100

        jmp done

        errorblock:
            mov eax, error
            call print_string

        ;;

        done:
            ;; loop (i++ <= counter -> terminate);;

            mov ebx, 0          ; counter

            loop_start:
                mov eax, name
                call print_string
                call print_nl

                inc ebx

                cmp ebx, [counter]
                jl loop_start

                jmp exit

        ;;;

        exit:
            popa
            mov eax,0  ; confirm program ran w/o error

            ret