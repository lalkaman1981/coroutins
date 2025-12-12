/**
 * @file switch_context.S
 * @brief Low-level x86-64 context switching implementation
 *
 * Assembly implementation supporting both Windows and System V calling conventions.
 * Saves and restores all general-purpose registers, RFLAGS, and extended CPU state.
 */


/** @addtogroup context
 *  @{
 */

.intel_syntax noprefix

#ifdef _WIN32
.section .text
#else
.section .note.GNU-stack,"",@progbits
.section .text
#endif

.global switch_context
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

#ifdef _WIN32
    mov QWORD PTR [rcx + 0], rsp
    lea r13, [rcx + 64]
    mov eax, 7
    xor edx, edx
    xsave [r13]

    lea r13, [rdx + 64]
    mov eax, 7
    xor edx, edx
    xrstor [r13]
    mov rsp, QWORD PTR [rdx + 0]
#else
    mov QWORD PTR [rdi + 0], rsp
    lea r13, [rdi + 64]
    mov eax, 7
    xor edx, edx
    xsave [r13]

    lea r13, [rsi + 64]
    mov eax, 7
    xor edx, edx
    xrstor [r13]
    mov rsp, QWORD PTR [rsi + 0]
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

/** @} */