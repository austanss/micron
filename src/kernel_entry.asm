global kernel_entry

extern kernel_main
extern ctor_global
extern loadIDT
extern loadGDT64
extern restart_cold
extern hex_str_serial
extern puts
extern serial_msg
extern configurePIC
extern setupPaging

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

	; gdt
	call loadGDT64

	mov edi, 0x0000FF00
	call set_status_color

	; idt
	call loadIDT

	; paging
	call setupPaging

	mov edi, 0xFF00FF00
	call set_status_color

	call configurePIC

	; interrupts
	sti								;	set the interrupt flag

	call ctor_global				;	call global constructors

	mov edi, 0x00FFFF00
	call set_status_color

	mov rdi, r15					;	bring back original rdi

	call kernel_main				;	call kernel

	.halt:							;	hang
		hlt
		jmp .halt

	call restart_cold				;	if loop breaks save system from fatal errors

set_status_color:
	push r14
	mov r14, [asm_framebuffer_ptr]
%rep 200
	mov [r14], edi
	add r14, 4
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
