global kernel_entry

extern kernel_main
extern ctor_global
extern loadIDT64
extern loadGDT64

kernel_entry:

	mov esp, stack_end				;	reconfigure the stack

	; ensure we are in long mode
	; and replace the UEFI-owned
	; existing GDT and IDT, ofc

	; interrupts
	cli								;	clear the interrupt flag

	push rax

	; long mode control label
	mov ecx, 0xC0000080          	; 	Set the C-register to 0xC0000080, which is the EFER MSR.
    rdmsr                        	; 	Read from the model-specific register.
    or eax, 1 << 8               	; 	Set the LM-bit which is the 9th bit (bit 8).
    wrmsr                        	; 	Write to the model-specific register.

	; paging
    mov rax, cr0                 	; 	Set the A-register to control register 0.
    or rax, 1 << 31              	; 	Set the PG-bit, which is the 32nd bit (bit 31).
    mov cr0, rax                 	;	Set control register 0 to the A-register.

	pop rax

	; gdt
;	call loadGDT64

	; idt
	; call loadIDT64

	; interrupts
	; sti							;	set the interrupt flag

	push rax

	call ctor_global				;	call global constructors

	pop rax							;	push the arguments back

	call kernel_main				;	call kernel main

	.halt:							;	hang
		hlt
		jmp .halt


section .bss

stack_begin:
	resb 16384 						; 	16 KiB
stack_end: