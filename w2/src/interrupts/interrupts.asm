global interrupt_handler_33
extern keyboard_irq_handler

interrupt_handler_33:
    pushad                      ; clears all GP registers
    cld                         ; clears direction flag before calling C

    call keyboard_irq_handler   ; executes C kb handler

    popad                       ; restores registers
    iretd                       ; returns to interrupted code