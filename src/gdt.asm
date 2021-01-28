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
    .null: equ $ - gdt         ; The null descriptor.
    dw 0xFFFF                    ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 1                         ; Granularity.
    db 0                         ; Base (high).
    .code: equ $ - gdt         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access (exec/read).
    db 10101111b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
    .data: equ $ - gdt         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
    .ptr:                      ; The GDT-pointer.
    dw $ - gdt - 1               ; Limit.
    dq gdt                       ; Base.

cs_ptr:
	dq load_gdt.ret
	dw 0x08