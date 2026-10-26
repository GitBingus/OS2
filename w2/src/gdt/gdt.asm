global lgdt_init            ; Makes the function callable from C

lgdt_init:                  ; start of function
    mov     eax,[esp + 4]   ; loads address of base and limit of GDT
    lgdt    [eax]           ; loads the CPU's GDTR

    mov ax, 0x10            ; moves 0x10 into ax, saves putting 0x10 into every register
    mov ds, ax              ; reloads the data segment
    mov es, ax              ; reloads the extra segment
    mov fs, ax              ; reloads FS
    mov gs, ax              ; reloads GS
    mov ss, ax              ; reloads the stack segment

    jmp 0x08:.flush_cs      ; 'far jump' to flush_cs 

.flush_cs:
    ret                     ; returns to calling function