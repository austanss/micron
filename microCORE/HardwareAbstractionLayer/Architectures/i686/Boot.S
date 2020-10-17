# Multiboot header constants
.set ALIGN,    1<<0             # load modules on page boundaries
.set MEMINFO,  1<<1             # memory map
.set FLAGS,    ALIGN | MEMINFO  # multiboot flags
.set MAGIC,    0x1BADB002       # magic number
.set CHECKSUM, -(MAGIC + FLAGS) # header checksum


# Multiboot Header
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Memory for initial thread
.section .bss
.align 16
stack_bottom:
	.skip 16384 # 16 kib
stack_top:

/* the linker script specifies _start as the entry
point for the kernel */
.section .text
.global _start
.type _start, @function
_start:
	/* stack */
	mov $stack_top, %esp

	cli

	call loadGDT

	mov %cr0, %eax
	or 1, %al
	mov %eax, %cr0

	call loadIDT

	call configurePIC

	jmp $0x8, $protectedModeMain

protectedModeMain:

	sti

	/* here we call kernel_main */
	call kernel_main

	/* test for terminal, post-kernel */
	call term_test_post

	/* if the system has nothing else to do, put the pc
	into an infinite loop */

1:	hlt
	jmp 1
;
;/* set thie size of the _start symbol to the current location */
;.size _start, . - _start
;
