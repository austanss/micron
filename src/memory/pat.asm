global configure_pat:function (configure_pat.end - configure_pat)

configure_pat:
    push rbp
    mov rbp, rsp
    xor rax, rax
    mov al, 6
    mov ah, 1
    mov bl, 0
    shl bl, 16
    or eax, ebx
    mov cx, 0x277
    wrmsr
    pop rbp
    ret
    .end: