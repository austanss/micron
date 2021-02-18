global load_gdt
global gdt

load_gdt:
	push rax
	lgdt [gdt.ptr]        ; Load the new GDT pointer
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


gdt:                           ; Global Descriptor Table (64-bit).
   ; Invalid segment
    .null:
    DQ 0x0000000000000000 ; 0x0
   ; Kernel code: RW, executable, code/data segment, present, 64-bit, ring 0
    .kcode
    DQ 0x00209A0000000000 ; 0x8
   ; Kernel data: RW, data, code/data segment, present, ring 0
    .kdata
    DQ 0x0000920000000000 ; 0x10
   ; User data: RW, data, code/data segment, present, ring 3
    .udata
    DQ 0x0020F20000000000 ; 0x18
   ; User code: RW, executable, code/data segment, present, 64-bit, ring 3
    .ucode
    dq 0x0020FA0000000000 ; 0x20
    .ptr:                      ; The GDT-pointer.
    dw $ - gdt - 1               ; Limit.
    dq gdt                       ; Base.

cs_ptr:
	dq load_gdt.ret
	dw 0x08