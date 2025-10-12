.intel_syntax noprefix

; .section .note.GNU-stack,"",@progbits

.section .text
.global switch_context
.type switch_context, @function

// void switch_context(old_context, new_context):
//                     RDI,         RSI
// since uint64_t = 8 bytes, move each register by 8
switch_context:

// save old

    pushf

    push r15
    push r14
    push r13
    push r12
    push rbx
    push rbp

    fxsave [rdi + 16]

// save stack pointer
    mov [rdi], rsp

// load new context
    mov rsp, [rsi]

    fxrstor [rsi + 16]

    pop rbp
    pop rbx
    pop r12
    pop r13
    pop r14
    pop r15

    popf

    ret