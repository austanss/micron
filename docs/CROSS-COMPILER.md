# Cross Compiler
microCORE is built with no red zone, using an x86-64 GCC cross compiler. You cannot download a prebuilt package of this cross-compiler due to special compiler options when built.

## Instructions

**NOTE:** you may need root privileges for this process.

### Get source code
You will need to obtain the `binutils` and `gcc` source code.

GCC source code can be found [here](ftp://ftp.gnu.org/gnu/gcc/), and binutils source code can be found [here](ftp://ftp.gnu.org/gnu/binutils/). It is recommended you download the latest release.

Extract these two archives to a folder. You should have a folder for GCC, and a folder for Binutils. The two folders should be in one same folder.

### **Set up build environment**
You will need to run Linux for this compilation process.

First, install these packages from your package manager:
```
gcc
make
bison
flex
libgmp3-dev
libmpc-dev
libmpfr-dev
texinfo
libcloog-isl-dev
libisl-dev
```

Open a terminal, and type these commands:
```bash
export PREFIX="$HOME/opt/cross"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"
```
Do not close the terminal.

### **Building**
#### **`binutils`:**

Type these commands:
```bash
mkdir build-binutils
cd build-binutils
../binutils-x.y.z/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install
```
Compilation may take anywhere from 5-10 minutes.

Do not close the terminal.

#### **`gcc`:**
Before you compile GCC, you will need to edit some configurations.

Create a file with these contents:
```
MULTILIB_OPTIONS += mno-red-zone
MULTILIB_DIRNAMES += no-red-zone
```
Save the file as `t-x86_64-elf` under the `gcc/config/i386/` directory in the GCC source tree.

Edit `gcc/config.gcc` in the GCC source tree, and replace this case block:
```bash
 x86_64-*-elf*)
 	tm_file="${tm_file} i386/unix.h i386/att.h dbxelf.h elfos.h newlib-stdint.h i386/i386elf.h i386/x86-64.h"
 	;;
```
with
```bash
 x86_64-*-elf*)
	tmake_file="${tmake_file} i386/t-x86_64-elf"
 	tm_file="${tm_file} i386/unix.h i386/att.h dbxelf.h elfos.h newlib-stdint.h i386/i386elf.h i386/x86-64.h"
 	;;
```
Now, leave the GCC source tree.
Type these following commands:
```bash
mkdir build-gcc
cd build-gcc
../gcc-x.y.z/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
```
If you did not build Binutils, GCC will throw errors when you try to build libgcc.

Compilation can take anywhere from 30 minutes to 2 hours.

When you are done, you should have a properly configured `x86_64-elf-gcc` cross-compiler.

Now, you are ready to build microCORE.