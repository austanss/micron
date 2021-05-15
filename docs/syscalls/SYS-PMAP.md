# `syscall: ax=0x0004 // pmap`

### Procedure
```x86asm
mov ax, 0x4
mov rdi, 0xc800000 ; address to map page at
syscall ; rax holds address of new page
```

### Parameters
**`rdi`**

Holds the address to map a new page at.

### Returns
This system call returns the value of rdi.

May return `nullptr` if an error arises.