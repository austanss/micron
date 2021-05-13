global ring3_call
global ring0_return
global ring3_call_rsp_save

align 4096

ring3_call_rsp_save:
    dq 0
ring3_call_rbp_save:
    dq 0

; extern "C" void ring3_call(void (*function)(evsys_packet*), evsys_packet* packet, void* stack);
ring3_call:
    mov rax, 0x1B  
    mov ds, ax
    mov es, ax

    mov [ring3_call_rsp_save], rsp
    mov [ring3_call_rbp_save], rbp

    push rax
    push rdx
    push 0x00
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
    mov rsp, [ring3_call_rsp_save]
    mov rbp, [ring3_call_rbp_save]
    ret

align 4096
db 0