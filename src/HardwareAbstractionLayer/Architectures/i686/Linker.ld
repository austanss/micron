ENTRY(_start)

SECTIONS
{
	VGA_POINTER = 0xb8000;
	. = 0xb8000;
	. += 80 * 25 * 2;

	. = 1M;

	.multiboot :
	{
		KEEP(*(.multiboot))
		. = ALIGN(4K);
	}

	.text :
	{
		*(.text .text.*)
		. = ALIGN(4K);
	}

	.rodata :
	{
		*(.rodata .rodata.*)
		. = ALIGN(4K);
	}

	.data :
	{
		*(.data.rel.ro.local*) *(.data.rel.ro .data.rel.ro.*) *(.data.*)
		. = ALIGN(4K);
	}

	.gdt :
	{
		*(.gdt)
		. = ALIGN(4K);
	}

	.bss :
	{
		*(.bss .bss.*);
		. = ALIGN(4K);
	}
}
