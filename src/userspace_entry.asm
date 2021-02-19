global enter_userspace
global userspace_entry
extern tss.kernel_rsp
extern irq_mask
extern request_page

enter_userspace:

    push rdi
    mov rdi, 0x0
    call irq_mask
    pop rdi
    
    call request_page
    add rax, 0x1000
    mov [tss.kernel_rsp], rax

    mov rax, 0x1B               ; Selector 0x18 (User Data) + RPL 3
    mov ds, ax
    mov es, ax

    push rax                    ; Selector 0x18 (User Data) + RPL 3
    push userspace_stack_end    ; User space stack
    push 0x202                  ; rflags = interrupt enable + reserved bit
    push 0x23                   ; Selector 0x20 (User Code) + RPL 3
    push userspace_entry        ; Entry point in user space

    iretq

align 4096
userspace_entry:
    mov r15, 0xD00D1ED1D17
    jmp $

section .bss

userspace_stack_begin:
    resb 4096 ; 4 KiB
userspace_stack_end:         