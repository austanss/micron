global load_gdt
global load_tss

default rel

load_gdt:
	push rax
	lgdt [rdi]        ; Load the new GDT pointer
	mov ax, 0x10
	mov ds, ax
	mov es, ax
   	mov fs, ax
   	mov gs, ax
   	mov ss, ax
	pop rax
	jmp far [cs_ptr]

	.ret:
		ret

load_tss:
    ltr di
    ret        

cs_ptr:
	dq load_gdt.ret
	dw 0x08