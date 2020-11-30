global kernel_entry

extern kernel_main
extern ctor_global
extern loadIDT64
extern loadGDT64
extern restart_cold
extern hex_str_serial
extern puts
extern serial_msg

kernel_entry:

	mov r15, rdi					;	preserve parameters]

	mov rdi, kernel_taken_over
	call serial_msg

	mov rcx, [r15+8]
	mov qword rdx, [rcx]
	mov qword [asm_framebuffer_ptr], rdx
	mov dword eax, [rcx+20]
	mov dword [asm_framebuffer_x_res], eax
	mov dword eax, [rcx+24]
	mov dword [asm_framebuffer_y_res], eax
	mov rax, 0
	mov rbx, 0
	mov rcx, 0
	mov rdx, 0

	mov edi, 0xFFFFFF00
	call set_status_color


	mov esp, stack_end				;	reconfigure the stack


	; ensure we are in long mode
	; and replace the UEFI-owned
	; existing GDT and IDT, ofc

	; interrupts

	cli								;	clear the interrupt flag

	; long mode control label
	mov ecx, 0xC0000080          	; 	Set the C-register to 0xC0000080, which is the EFER MSR.
    rdmsr                        	; 	Read from the model-specific register.
    or eax, 1 << 8               	; 	Set the LM-bit which is the 9th bit (bit 8).
    wrmsr                        	; 	Write to the model-specific register.

	mov edi, 0xFF000000
	call set_status_color

	; paging
;    mov rax, cr0                 	; 	Set the A-register to control register 0.
 ;   or rax, 1 << 31              	; 	Set the PG-bit, which is the 32nd bit (bit 31).
  ;  mov cr0, rax                 	;	Set control register 0 to the A-register.

	mov edi, 0x00FF0000
	call set_status_color

	; gdt
;	call loadGDT64

	mov edi, 0x0000FF00
	call set_status_color

	; idt
;	call loadIDT64

	mov edi, 0xFF00FF00
	call set_status_color

	; interrupts
	sti								;	set the interrupt flag

	call ctor_global				;	call global constructors

	mov edi, 0x00FFFF00
	call set_status_color

	mov rdi, r15					;	bring back original rdi

	call kernel_main				;	call kernel main

	.halt:							;	hang
		hlt
		jmp .halt

	call restart_cold				;	if loop breaks save system from fatal errors

set_status_color:
	push r14
	mov r14, [asm_framebuffer_ptr]
%rep 200
	mov [r14], edi
	inc r14
%endrep
	pop r14
	ret

section .bss

stack_begin:
	resb 16384 						; 	16 KiB
stack_end:

section .data

asm_framebuffer_ptr:
	resb 8

asm_framebuffer_x_res:
	resb 8

asm_framebuffer_y_res:
	resb 8

kernel_taken_over:
	db "KERNEL TAKEN OVER",0x0A,0
