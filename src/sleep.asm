global sleep
extern ms_clock

sleep:
    push rbp
    mov rbp, rsp
    push rax
    mov rax, 0
    mov qword [ms_clock], rax
    xor rax, rax
    mov eax, edi
    .loop:
        cli
        mov rbx, qword [ms_clock]
        cmp rbx, rax
        jg .done
        sti
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        jmp .loop
    .done:
        sti
        pop rax
        pop rbp
        ret    
            