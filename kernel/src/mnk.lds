ENTRY(mnkentry)

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
		uentry = .;
		*(.userspace)
		*(.data.userspace)
		*(.rodata.userspace)
	}

	. = ALIGN(4k);

	.data :
	{
		*(.rodata .rodata.*)
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