.intel_syntax noprefix

#ifdef _WIN32
.section .text
#else
.section .note.GNU-stack,"",@progbits
.section .text
#endif

.global create_coro_stack
create_coro_stack:
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

#ifdef _WIN32
    lea rax, [r9 + 16]
    fxsave [rax]
    mov QWORD PTR [r9 + 0], rsp

    mov r12, r9

    mov rsp, [r8 + 536]
    and rsp, -16
    sub rsp, 32

    mov rax, rcx
    mov rcx, rdx
    xor rdx, rdx

    call rax

    mov r9, r12

    mov rsp, QWORD PTR [r9 + 0]
    lea rax, [r9 + 16]
    fxrstor [rax]
#else
    lea rax, [rcx + 16]
    fxsave [rax]
    mov QWORD PTR [rcx + 0], rsp

    mov r12, rcx

    mov rsp, [rdx + 536]
    and rsp, -16

    mov rax, rdi            # rax = fn
    mov rdi, rsi            # args -> rdi
    xor rsi, rsi

    call rax

    mov rcx, r12

    mov rsp, QWORD PTR [rcx + 0]
    lea rax, [rcx + 16]
    fxrstor [rax]
#endif

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