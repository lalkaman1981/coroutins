.intel_syntax noprefix

; .section .note.GNU-stack,"",@progbits

.section .bss
.balign 16
stack_space:
    .zero 16384

.balign 16
fxsave_area:
    .zero 512

.section .data
.global old_rsp
old_rsp: .quad 0


.section .text
.global change_stack
.type change_stack, @function

change_stack:
    jmp save_data

continue_after_save:
    lea rsp, [rip + stack_space + 16384]

    mov rax, rdi    # rax = fn
    mov rdi, rsi    # args -> rdi
    xor rsi, rsi

    call rax

    jmp restore_data

end_after_restore:
    ret

save_data:

    push rax
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov QWORD PTR [rip + old_rsp], rsp

    lea rax, [rip + fxsave_area]
    fxsave [rax]

    jmp continue_after_save

restore_data:

    lea rax, [rip + fxsave_area]
    fxrstor [rax]

    mov rsp, QWORD PTR [rip + old_rsp]
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    pop rax

    jmp end_after_restore
