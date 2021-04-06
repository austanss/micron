ENTRY(kernel_entry)

SECTIONS
{
	. = 2M;

	__kernel_start = .;
	
	.stivalehdr : 
	{
        KEEP(*(.stivalehdr))
    }

	. = ALIGN(4K);

	.text :
	{
		*(.text)
		. = ALIGN(4K);
		*(.userspace)
	}

	. = ALIGN(4K);

	.rodata :
	{
		*(.rodata .rodata.*)
	}

	. = ALIGN(4K);

	.data :
	{
		*(.data.rel.ro.local*) *(.data.rel.ro .data.rel.ro.*) *(.data.*)
	}

	. = ALIGN(4K);

	.bss :
	{
		*(.bss .bss.*)
	}

	. = ALIGN(4K);

	__kernel_end = .;
}