global ring3_call
global ring0_return

align 4096

; extern "C" void ring3_call(void (*function)(evsys_packet*), evsys_packet* packet, void* stack);
ring3_call:
    mov rax, 0x1B  
    mov ds, ax
    mov es, ax

    push rax
    push rdx
    push 0x202
    push 0x23
    push .ring3

    iretq
    
    .ring3:
    mov r15, rdi
    mov rdi, rsi
    xor rdx, rdx

    call r15

    mov rax, 0xfffccfffdeadbeef
    int 0x80

ring0_return:
    ret

align 4096
db 0