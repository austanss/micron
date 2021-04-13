# Calling Conventions

microCORE follows the System V x86-64 calling conventions. You must adhere to the calling conventions, in order for microCORE to keep compatibillity with C++ code, and stay relocatable.

## Passing Parameters

When passing parameters to **any** function, C++ or not, you must pass parameters in a specific order.

Take, for example, the following function's prototype:
```c++
extern "C" int foo(uint8_t bar, uint64_t baz, uint64_t quux);
```

When passing the parameters, you would pass the parameters in this order:

```nasm
mov rdi, 0xFF           ; bar
mov rsi, 0xDEADBEEF     ; baz
mov rdx, 0xBEEFCAFE     ; quux
```

See [this webpage](https://wiki.osdev.org/Calling_Conventions) for more info.

## Calling Functions/Accessing Data

microCORE is **position-independent**, meaning that no matter what address the kernel is loaded to, it will always function properly.

When calling functions, there is a specific method to keep the code position-independent.

This also applies to pointers, or accessing any kind of field defined in the kernel's executable.

Say we want to call this function here:
```c++
extern "C" int foo(char* bar, uint32_t baz);
```
Say that `bar` is a pointer to a string.

In assembly, this is how you would call the function:
```nasm
extern foo

fee:                    ; executing here
    lea rdi, [rel bar]  ; load address of bar into rdi
    lea r14, [rel baz]  ; load address of baz into r14
    mov esi, [r14]      ; access the data of baz
    lea r14, [rel foo]  ; load the address of foo into r14
    call r14            ; call the address of foo, stored in r14

bar:
    db "quux",0         ; string defined here

baz:
    dd 4                ; length of quux
```
The process is a little tedious, but it is required for functionality.    