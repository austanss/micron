global setup_syscalls
global syscall_handler
extern serial_msg
extern tss_get

%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

setup_syscalls:
    mov rax, syscall_handler
    mov r8, rax
    shr r8, 0x20
    mov rdx, r8

    ; Truncate to 32-bits
    mov eax, eax
    mov edx, edx

    ; Load handler RIP into LSTAR MSR
    mov rcx, 0xc0000082
    wrmsr

    ; Enable syscall / sysret instruction
    mov rcx, 0xc0000080
    rdmsr
    or eax, 1
    wrmsr
    mov rcx, 0xc0000081
    rdmsr
    mov edx, 0x00180008
    wrmsr

    ret

syscall_handler:
    cli

    pushaq

    mov r15, rax
    mov rdi, 0
    mov r14, rcx
    call tss_get
    mov rcx, r14
    mov rdi, rsp
    mov rsp, qword [rax + 0x04]
    push rdi

    mov r13, 0x100501
    cmp r15, r13
    jne .syscall_end
    mov rdi, rbx
    call serial_msg

    .syscall_end:
    pop rsp
    popaq 

    o64 sysret    
    