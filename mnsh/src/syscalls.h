#pragma once

// TEMPORARY: These syscalls will not be removed, but many may be added before v1.0.
enum syscall {
    
    open,
    read,
    write,
    close,

    spmap,
    spexe,
    spunmap,

    sysinfo,
    
    levrd,
    devrd
};

inline void* pmap(unsigned long address)
{
    unsigned long rax = 0;
    asm volatile ("syscall" : "=a" (rax) : "a" (spmap));
    return (void *)rax;
}