extern request_page
extern donate_to_userspace
extern setup_syscalls
extern userspace_debug_catch
extern evsys_initialize

global enter_userspace

default rel

enter_userspace:

    mov r15, rdi

    push rbp
    mov rbp, rsp

    call request_page
    mov rbx, rax
    add rbx, 0x1000
    mov rdi, rax
    call donate_to_userspace

    call setup_syscalls

;   call evsys_initialize

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