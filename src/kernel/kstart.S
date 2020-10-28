# Multiboot header constants
    .set MAGIC, 0x1badb002
    .set FLAGS,    3  # multiboot flags
    .set CHECKSUM, -(MAGIC + FLAGS)

// set up graphics mode

    .set MODE_TYPE, 0   //  request graphics mode
    .set WIDTH, 640     //  resolution requested
    .set HEIGHT, 480    //  for screen from GRUB
    .set DEPTH, 32      //  requested BPP


// set up framebuffer

    .set HEADER_ADDR, 0
    .set LOAD_ADDR, 0
    .set LOAD_END_ADDR, 0
    .set BSS_END_ADDR, 0
    .set ENTRY_ADDR, 0


# Multiboot Header
    .section .multiboot
    .align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

    .long HEADER_ADDR
    .long LOAD_ADDR
    .long LOAD_END_ADDR
    .long BSS_END_ADDR
    .long ENTRY_ADDR
    .long MODE_TYPE
    .long WIDTH
    .long HEIGHT
    .long DEPTH


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
    cli
	/* stack */
	mov $stack_top, %esp

    /* global constructors */
    call ctor_global

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
	   into an infinite loop while waiting for interrupts */

1:	hlt
	jmp 1b
;
;/* set thie size of the _start symbol to the current location */
;.size _start, . - _start
;
