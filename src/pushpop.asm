global pusha
global popa

pusha:
	push rax
	push rcx
	push rdx
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push 0xBEEFBEEF

popa:
	.beefbeef:
		pop r9
		cmp r9, 0xBEEFBEEF
		jne .beefbeef
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rdx
	pop rcx
	pop rax
	ret
