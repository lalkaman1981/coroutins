.intel_syntax noprefix

.section .note.GNU-stack,"",@progbits
.section .text
.global create_coro_stack
.type create_coro_stack, @function

create_coro_stack:
    # rdx = new_context, rcx = old_context
    # rdi = fn, rsi = args
    # offsets: old_rsp:0, fxsave_area:16, stack_base:528, stack_top:536

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

    mov QWORD PTR [rcx + 0], rsp
    lea rax, [rcx + 16]
    fxsave [rax]
    mov r12, rcx

    lea rsp, [rdx + 536]
    and rsp, -16

    mov rax, rdi            # rax = fn
    mov rdi, rsi            # args -> rdi
    xor rsi, rsi

    call rax

    mov rcx, r12
    mov rsp, QWORD PTR [rcx + 0]
    lea rax, [rcx + 16]
    fxrstor [rax]

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
