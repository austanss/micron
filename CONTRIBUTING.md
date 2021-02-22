# Contributing

## Cross Compiler

microCORE builds using the `x86_64-elf-gcc` cross-compiler. You cannot install this cross compiler through prebuilt packages, as the compiler used specifically for microCORE is built with special options.

See [this document](docs/CROSS-COMPILER.md) for more info.

## Styling

The maintainer of this project strictly requires all code follow a styleguide. Any contributions made that do not follow the styleguide, no matter how helpful they may be, will be rejected.

See [the styleguide](docs/STYLEGUIDE.md) for more info.

## Calling Conventions

microCORE uses very specific calling conventions. When writing C++ code, the following information is rather useless to you.

However, when writing assembly, it is critical that you follow this guide.

See [the calling conventions](docs/CALLING-CONVENTIONS.md) for more info.