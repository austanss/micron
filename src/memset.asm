global memset

memset:
	push rbp
	mov rbp,rsp
	mov rdi, [rdi]                    ; dest
	movzx rax,byte [rsi]              ; ch
	mov rcx, [rdx]                    ; count
	rep stosb
	pop rbp
	ret
