# `syscall: ax=0x0007 // sysinfo`

### Procedure
```x86asm
mov ax, 0x7
syscall ; return value is in rax
```

### Parameters
This system call takes no parameters.

### Returns
This system call returns a pointer to a system info structure, defined below:
```cpp
struct system_info {

    struct {
        unsigned long   fb_ad;
        unsigned short  fb_resx;
        unsigned short  fb_resy;
        unsigned short  fb_pitch;
        unsigned short  fb_bpp;
        unsigned long   fb_size;
    } display_info;

    struct {
        unsigned long   total_ram;
        unsigned long   used_ram;
        unsigned long   free_ram;
    } memory_info;
};
```