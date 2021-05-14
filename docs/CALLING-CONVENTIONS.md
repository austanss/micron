# Calling Conventions

micron's kernel follows the System V x86-64 calling conventions. You must adhere to the calling conventions, in order for micron's kernel to keep compatibillity with C/C++ code.

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