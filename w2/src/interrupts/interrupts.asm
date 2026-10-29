global idt_load

idt_load:
    mov eax, [esp + 4]  ; copy the address of idtr structure into eax
    lidt [eax]          ; tells the CPU to read the IDTR information from memory at the address in eax
    ret