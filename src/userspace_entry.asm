global enter_userspace
global userspace_entry
extern tss_install
extern request_page
extern donate_to_userspace
extern setup_syscalls
extern userspace_debug_catch

enter_userspace:

    push rdi
    push rsi

    mov rdi, 0
    mov rsi, rsp
    call tss_install
    
    call request_page
    mov rbx, rax
    add rbx, 0x1000
    mov rdi, rax
    call donate_to_userspace

    call setup_syscalls
    pop rsi
    pop rdi

    mov rax, 0x1B               ; Selector 0x18 (User Data) + RPL 3
    mov ds, ax
    mov es, ax

    push rax                    ; Selector 0x18 (User Data) + RPL 3
    push rbx                    ; User space stack
    push 0x202                  ; rflags = interrupt enable + reserved bit
    push 0x23                   ; Selector 0x20 (User Code) + RPL 3
    push userspace_entry        ; Entry point in user space

    iretq

align 4096
userspace_entry:
    ; Validate stack: if the stack is invalid and no exception 
    ; is thrown (i.e. #PF), throw one so we know that the stack
    ; is bad. Came in clutch for me so I could fix the stack issues
    push 0x80
    pop rax
    mov rbx, 0x80
    cmp rax, rbx
    jne 0x0

    call userspace_debug_catch

    ; Test syscalls: 
    ; rax=100501h: serial message string
    ; rbx: parameter: pointer to null-terminated string
    mov rax, 0x100501
    mov rbx, userspace_message
    syscall
    jmp $

userspace_message:
    db "Got into userspace, and system calls are working!",0