global load_gdt
global load_tss

default rel

section .text

load_gdt:
	lgdt [rdi]        ; Load the new GDT pointer
	mov ax, 0x10
	mov ds, ax
	mov es, ax
   	mov fs, ax
   	mov gs, ax
   	mov ss, ax
	pop rax
	push 0x08
	push rax
	retfq

load_tss:
    ltr di
    ret        
