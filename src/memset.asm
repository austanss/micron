global memset

memset:
	push rbp
	mov rbp,rsp
	mov rdi,[rbp+8]                     ; dest
	movzx rax,byte [rbp+12]             ; ch
	mov rcx,[rbp+16]                    ; count
	rep stosb
	pop rbp
	ret
