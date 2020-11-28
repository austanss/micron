global pusha
global popa

pusha:
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi


popa:
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
	ret
