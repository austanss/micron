extern mnkmain
extern load_idt
extern gdt_init
extern configure_pic
extern enter_userspace

default rel

mnkentry:

	mov r15, rdi					;	preserve parameters

	lea rsp, [rel stack.end]		;	reconfigure the stack
	
	xor rbp, rbp
	push rbp
	mov rbp, rsp

	; replace the UEFI-owned
	; existing GDT and IDT, ofc

	cli								;	clear the interrupt flag

	; gdt
	lea r14, [rel gdt_init]
	call r14

	; pic
	lea r14, [rel configure_pic]
	call r14

	mov rdi, r15					;	bring back original rdi
	mov rsi, rsp					;	pass rsp to kernel for tss

	lea r14, [rel mnkmain]
	call r14						;	call kernel to configure

	mov rax, rdi
	lea r14, [rel enter_userspace]
	call r14
global mnkentry:function ($ - mnkentry)

section .bss

stack:
	resb 4096						; 	one page
	.end: