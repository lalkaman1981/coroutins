/**
 * @file create_stack.S
 * @brief Bootstrap new coroutine on dedicated stack
 *
 * Assembly routine to initialize and execute a coroutine on its new stack.
 * Handles stack alignment and calling convention for both Windows and Linux.
 */
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
    # Windows x64: rcx, rdx, r8, r9
    # rcx = function pointer
    # rdx = argument
    # r8 = stack pointer
    # r9 = context pointer
    mov r10, r9               # Save context pointer
    mov r11, r8               # Save stack pointer

    # Save current context
    mov QWORD PTR [r10 + 0], rsp
    lea r13, [r10 + 64]
    push rax
    push rdx
    mov eax, 7
    xor edx, edx
    xsave [r13]
    pop rdx
    pop rax

    # Switch to new stack
    mov r12, r10
    mov rsp, [r11 + 4168]
    and rsp, -16
    sub rsp, 32               # Windows shadow space

    # Call function
    mov rax, rcx
    mov rcx, rdx
    xor rdx, rdx
    call rax

    # Restore context
    mov r9, r12
    mov rsp, QWORD PTR [r9 + 0]
    lea r13, [r9 + 64]
    mov eax, 7
    xor edx, edx
    xrstor [r13]
#else
    # Linux: rdi, rsi, rdx, rcx
    # rdi = function pointer
    # rsi = argument
    # rdx = stack pointer
    # rcx = context pointer

    mov QWORD PTR [rcx + 0], rsp
    lea r13, [rcx + 64]
    push rax
    push rdx
    mov eax, 7
    xor edx, edx
    xsave [r13]
    pop rdx
    pop rax

    mov r12, rcx
    mov rsp, [rdx + 4168]
    and rsp, -16

    mov rax, rdi
    mov rdi, rsi
    xor rsi, rsi
    call rax

    mov rcx, r12
    mov rsp, QWORD PTR [rcx + 0]
    lea r13, [rcx + 64]
    mov eax, 7
    xor edx, edx
    xrstor [r13]
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
