global setup_syscalls
global syscall_handler
extern serial_msg
extern tss_get

%macro pushaq 0
    mov [rsp-0x8],  rdi
    mov [rsp-0x10], rsi
    mov [rsp-0x18], rdx
    mov [rsp-0x20], r8
    mov [rsp-0x28], r11
    mov [rsp-0x30], r10
    mov [rsp-0x38], r9
    mov [rsp-0x40], r8
    mov [rsp-0x48], rdi
    mov [rsp-0x50], rsi
    mov [rsp-0x58], rbp
    mov [rsp-0x60], rsp
    mov [rsp-0x68], rbx
    mov [rsp-0x70], rdx
    mov [rsp-0x78], rcx
    mov [rsp-0x80], rax
    sub rsp, 0x80
%endmacro

%macro popaq 0
    add rsp, 0x80
    mov r15, [rsp-0x8]
    mov r14, [rsp-0x10]
    mov r13, [rsp-0x18]
    mov r12, [rsp-0x20]
    mov r11, [rsp-0x28]
    mov r10, [rsp-0x30]
    mov r9,  [rsp-0x38]
    mov r8,  [rsp-0x40]
    mov rdi, [rsp-0x48]
    mov rsi, [rsp-0x50]
    mov rbp, [rsp-0x58]
    mov rsp, [rsp-0x60]
    mov rbx, [rsp-0x68]
    mov rdx, [rsp-0x70]
    mov rcx, [rsp-0x78]
    mov rax, [rsp-0x80]
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
    call tss_get
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

    sti
    o64 sysret    
    