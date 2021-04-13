.PHONY: kernel image all run clean

kernel:
	@ mkdir -p bin
	@ make -C kernel all
	@ mv kernel/bin bin/kernel

all: kernel

image: all
	@ mkdir -p img
	@ res/GenerateImage

run:
	@ qemu-system-x86_64 -bios res/OVMF.fd -drive file=img/microNET.fat -serial stdio -net none -m 512M -machine q35 -soundhw pcspk

clean:
	@ rm -rf bin img
	@ make -C kernel clean