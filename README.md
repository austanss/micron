# µCORE

µCORE (microCORE) is the kernel of µNET (microNET).

# About
## Use Case
µNET is designed to be used in embedded or low power operations, where the maximum performance is obtained via having minimal overhead. However, in comparison to other common embedded operating systems (eg. Linux or Windows Embedded), µNET is designed with a hybrid-kernel. This provides a perfect compromise between kernel modularity and performance, leaving minimal overhead.

## Compatibility
While it may sound enticing for embedded systems to squeeze more performance, in practice it is a challenge. Developers for embedded systems will have to port their existing software, which can either be a simple recompile, all the way up to a complete rewrite. µNET solves this issue by embedding a .NET Framework-compliant runtime (Mono) into the heart of the OS.

# Development
## Requirements
µCORE requires the following software for building:

- GNU Make (build-essential on some systems)
- GCC/G++
- NASM
- GNU Binutils

## Building
```
make kernel
```

Object files will be located in `bin/obj`.\
The binary will be located in `bin` (`microCORE.kernel`).\
