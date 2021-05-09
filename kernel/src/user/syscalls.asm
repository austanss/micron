extern tss_get

extern sys_keyboard_event_subscribe
extern sys_copy_framebuffer
extern sys_tty_print
extern sys_allocate_page
extern sys_get_info

default rel

tss_rsp0:
    dq 0

setup_syscalls:
    push rbp
    mov rbp, rsp

    lea rax, [rel syscall_handler]
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
    mov edx, 0x00100008
    wrmsr

    mov rdi, 0
    lea r10, [rel tss_get]
    call r10
    lea r11, [rel tss_rsp0]
    mov r10, [rax + 4]
    mov [r11], r10

    pop rbp
    ret
global setup_syscalls:function ($ - setup_syscalls)

syscall_handler:
    cli

    push rbp
    mov rbp, rsp
    
    mov r11, rsp
    lea r10, [rel tss_rsp0]
    mov rsp, [r10]
    push r11

    push rcx

    lea r10, [rel syscall_table]
    lea rax, [r10 + rax * 8]
    call [rax]

    pop rcx
    pop rsp
    pop rbp
    o64 sysret
global syscall_handler:function ($ - syscall_handler)

syscall_table:
    dq sys_copy_framebuffer
    dq sys_allocate_page
    dq sys_keyboard_event_subscribe
    dq sys_get_info