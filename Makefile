.PHONY: run kernel mnsh all clean

kernel:
	@ mkdir -p bin
	@ mkdir -p bin/kernel
	@ make -C kernel all
	@ mv --force kernel/bin/* bin/kernel/

mnsh:
	@ mkdir -p bin
	@ mkdir -p bin/mnsh
	@ make -C mnsh all
	@ mv --force mnsh/bin/* bin/mnsh/

all: kernel mnsh

image: all
	@ mkdir -p img
	@ res/GenerateImage

run:
	@ qemu-system-x86_64 -bios res/OVMF.fd -drive file=img/micron.fat -serial stdio -net none -m 512M -machine q35 -soundhw pcspk

clean:
	@ rm -rf bin img
	@ make -C kernel clean
	@ make -C mnsh clean