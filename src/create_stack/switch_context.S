.intel_syntax noprefix

.section .note.GNU-stack,"",@progbits
.section .text
.global switch_context
.type switch_context, @function

switch_context:

    pushfq
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15


#    lea rax, [rdi + 16]
#    fxsave [rax]
    mov QWORD PTR [rdi + 0], rsp

# restore:
    
    mov rsp, QWORD PTR [rsi + 0]
#    lea rax, [rsi + 16]
#    fxrstor [rax]

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq

    ret
