extern request_page
extern donate_to_userspace
extern setup_syscalls
extern userspace_debug_catch
extern evsys_initialize

default rel

enter_userspace:

    mov r15, rdi

    push rbp
    mov rbp, rsp

    lea r14, [rel request_page]
    call r14
    mov rbx, rax
    add rbx, 0x1000
    mov rdi, rax
    lea r14, [rel donate_to_userspace]
    call r14

    lea r14, [rel setup_syscalls]
    call r14

    lea r14, [rel evsys_initialize]
    call r14

    mov rax, 0x1B               ; Selector 0x18 (User Data) + RPL 3
    mov ds, ax
    mov es, ax

    pop rbp
    
    push rax                    ; Selector 0x18 (User Data) + RPL 3
    push rbx                    ; User space stack
    push 0x202                  ; rflags = interrupt enable + reserved bit
    push 0x23                   ; Selector 0x20 (User Code) + RPL 3
    push r15                    ; Entry point in user space

    iretq
global enter_userspace:function ($ - enter_userspace)