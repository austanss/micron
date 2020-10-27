# µCORE

µCORE (microCORE) is the microkernel for µNET (microNET).

# About
## Use Case
µNET is designed to be used in embedded or low power operations, where the maximum performance is obtained via having minimal overhead. However, in comparison to other common embedded operating systems (eg. Linux or Windows Embedded), µNET is designed with a microkernel. This provides **even less** overhead.

## Compatibility
While it may sound enticing for embedded systems to squeeze more performance, in practice it is a challenge. Developers for embedded systems will have to port their existing software, which can either be a simple recompile, all the way up to a complete rewrite. µNET solves this issue by embedding a .NET Framework-compliant runtime (Mono) into the heart of the OS.

# Development
## Requirements
µCORE requires the following software for building:

- GNU Make (build-essential on some systems)
- Clang with cross compilation targets supported
- grub-mkrescue or grub2-mkrescue
- Xorriso
- NASM
- GNU Binutils

## Building
### Binary only
```
make bin ARCH=[arch]
```
### Package as ISO
```
make image ARCH=[arch]
```
You must specify the architecture to build for.\
Currently, we have support for these architectures:
 - i386
 - x86
 - x86_64

Object files will be located in `bin/obj`.\
The binary will be located in `bin` (`microCORE.kernel`).\
The ISO will generate in the repo root (`microNET.iso`).
## Testing
### In QEMU (recommended)
```
make qemu
```
**NOTE:** When testing with QEMU you do not need to specify an architecture, as the image is automatically generated for the QEMU target.
### Real hardware
```
sudo dd if=microNET.iso of=[device] bs=4M
```

**DISCLAIMER:** As of 10/15/2020, µNET has been tested to be unstable on real hardware. Proceed at your own risk.
