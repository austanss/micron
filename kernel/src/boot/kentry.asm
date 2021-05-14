extern mnkmain
extern load_idt
extern gdt_init
extern configure_pic
extern enter_userspace

default rel

global mnkentry

mnkentry:
	mov r15, rdi					;	preserve parameters

	mov rsp, stack.end
	
	xor rbp, rbp
	push rbp
	mov rbp, rsp
	
	cli								;	clear the interrupt flag

	; gdt
	call gdt_init

	; pic
	call configure_pic

	mov rdi, r15					;	bring back original rdi
	mov rsi, rsp					;	pass rsp to kernel for tss

	call mnkmain					;	call kernel to configure

	mov rdi, rax
	call enter_userspace

section .bss

stack:
	resb 4096						; 	one page
	.end: