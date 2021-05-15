# System Calling

micron supports x86-64-style system calls.

## Parameters
Parameters for system calls follow the same method as the System V ABI.

Parameters go in `rdi`, `rsi`, `rdx`, `rcx`, `r8`, and `r9`.

## Return value
System calls return values same way as normal C functions.

Return value is stored in `rax`, `rdx`.

## System call ID
The system call ID goes in `rax`.

Here is an enum defining system call IDs.

```cpp
enum syscall {
    
    open, // 0x0
    read, // 0x1
    write, // 0x2
    close, // 0x3

    pmap, // 0x4
    pexe, // 0x5
    punmap, // 0x6

    sysinfo, // 0x7
    
    levrd, // 0x8
    devrd // 0x9
};
```