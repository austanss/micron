global memset

memset:
	push ebp
	mov ebp,esp
	mov edi,[ebp+8]                     ; dest
	movzx eax,byte [ebp+12]             ; ch
	mov ecx,[ebp+16]                    ; count
	rep stosb
	pop ebp
	ret
