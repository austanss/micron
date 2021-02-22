global kernel_entry
global stack_begin
global stack_end

extern kernel_main
extern load_idt
extern gdt_init
extern configure_pic
extern enter_userspace

default rel

kernel_entry:

	mov r15, rdi					;	preserve parameters

	lea rsp, [rel stack_end]		;	reconfigure the stack
	push rbp
	mov rbp, rsp

	; ensure we are in long mode
	; and replace the UEFI-owned
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

	lea r14, [rel kernel_main]
	call r14						;	call kernel to configure

	lea r14, [rel enter_userspace]
	jmp r14

section .bss

stack_begin:
	resb 16384 						; 	16 KiB
stack_end: