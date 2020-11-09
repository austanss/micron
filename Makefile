.DEFAULT_GOAL := all

# ifeq (, $(shell which grub-mkrescue 2>/dev/null))
# 	ifeq (, $(shell which grub2-mkrescue 2>/dev/null))
# 		$(error "No grub-mkrescue or grub2-mkrescue in $(PATH)")
# 	else
#		MKRESCUE = grub2-mkrescue
# 	endif
# endif


ARCH := UNSPECIFIED
BUILD_DIR := bin
SRC_DIR := src
RES_DIR := res
FONTS_DIR := res/fonts
KERNEL_SRC_DIR := $(SRC_DIR)/kernel
BOOTLOADER_SRC_DIR := $(SRC_DIR)/bootloader
HEADERS_DIR := $(SRC_DIR)/include
OBJ_DIR := $(BUILD_DIR)/obj
OBJ_DIR_RES := $(OBJ_DIR)/res
LIB := /usr/lib64
EFILIB := /usr/lib64/gnuefi
EFI_CRT_OBJS := $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS := $(EFILIB)/elf_$(ARCH)_efi.lds
CC := gcc -I$(HEADERS_DIR) -pipe -D_FILE_OFFSET_BITS=64 -Wall -fPIE -Winvalid-pch -Wnon-virtual-dtor -g -m64 -march=x86-64 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
CXX := g++ -I$(HEADERS_DIR) -pipe -D_FILE_OFFSET_BITS=64 -Wall -fPIE -Winvalid-pch -Wnon-virtual-dtor -g -m64 -march=x86-64 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
MINGW := x86_64-w64-mingw32-gcc -m64 -march=x86-64 -fpic -ffreestanding -I$(HEADERS_DIR) -I$(HEADERS_DIR)/efi/x86_64 -I$(HEADERS_DIR)/efi/protocol
MINGW_LINK := x86_64-w64-mingw32-gcc
ASM := nasm -f elf64
STATIC_LINK := llvm-ar
OBJCOPY := objcopy -O elf64-x86-64 -B i386 -I binary
CXX_LINK := g++ -m64 -Wl,--as-needed -Wl,--no-undefined -fPIE -march=x86-64 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive
IMAGE_GEN := $(RES_DIR)/GenerateImage

export MKRESCUE
export GCC
export BUILD_DIR

kernel:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR_RES)
	$(ASM) $(KERNEL_SRC_DIR)/idt.asm -o $(OBJ_DIR)/idt.o
	$(ASM) $(KERNEL_SRC_DIR)/gdt.asm -o $(OBJ_DIR)/gdt.o
	$(ASM) $(KERNEL_SRC_DIR)/interrupts.asm -o $(OBJ_DIR)/ISR.o
	$(ASM) $(KERNEL_SRC_DIR)/memset.asm -o $(OBJ_DIR)/MemSet.o
	$(ASM) $(KERNEL_SRC_DIR)/paging.asm -o $(OBJ_DIR)/paging.o
	$(ASM) $(KERNEL_SRC_DIR)/pushpop.asm -o $(OBJ_DIR)/pushpop.o
	$(CXX) -MF$(OBJ_DIR)/gdt.cxx.o.d -o $(OBJ_DIR)/gdt.cxx.o -c $(KERNEL_SRC_DIR)/gdt.cxx
	$(CXX) -MF$(OBJ_DIR)/pic.cxx.o.d -o $(OBJ_DIR)/pic.cxx.o -c $(KERNEL_SRC_DIR)/pic.cxx
	$(CXX) -MF$(OBJ_DIR)/paging.cxx.o.d -o $(OBJ_DIR)/paging.cxx.o -c $(KERNEL_SRC_DIR)/paging.cxx
	$(CXX) -MF$(OBJ_DIR)/io.cxx.o.d -o $(OBJ_DIR)/io.cxx.o -c $(KERNEL_SRC_DIR)/io.cxx
	$(CXX) -MF$(OBJ_DIR)/terminal.cxx.o.d -o $(OBJ_DIR)/terminal.cxx.o -c $(KERNEL_SRC_DIR)/terminal.cxx
	$(CXX) -MF$(OBJ_DIR)/debugging.cxx.o.d -o $(OBJ_DIR)/debugging.cxx.o -c $(KERNEL_SRC_DIR)/debugging.cxx
	$(CXX) -MF$(OBJ_DIR)/kutil.cxx.o.d -o $(OBJ_DIR)/kutil.cxx.o -c $(KERNEL_SRC_DIR)/kutil.cxx
	$(CXX) -MF$(OBJ_DIR)/kbd.cxx.o.d -o $(OBJ_DIR)/kbd.cxx.o -c $(KERNEL_SRC_DIR)/kbd.cxx
	$(CXX) -MF$(OBJ_DIR)/gfx.cxx.o.d -o $(OBJ_DIR)/gfx.cxx.o -c $(KERNEL_SRC_DIR)/gfx.cxx
	$(CXX) -MF$(OBJ_DIR)/uart.cxx.o.d -o $(OBJ_DIR)/uart.cxx.o -c $(KERNEL_SRC_DIR)/uart.cxx
	$(CXX) -MF$(OBJ_DIR)/string.cxx.o.d -o $(OBJ_DIR)/string.cxx.o -c $(KERNEL_SRC_DIR)/string.cxx
	$(CXX) -MF$(OBJ_DIR)/kmain.cxx.o.d -o $(OBJ_DIR)/kmain.cxx.o -c $(KERNEL_SRC_DIR)/kmain.cxx -DARCH=\"$(ARCH)\"
	$(OBJCOPY) $(FONTS_DIR)/font.psf $(OBJ_DIR_RES)/font.o
	$(CXX_LINK) -o $(BUILD_DIR)/microCORE.kernel $(OBJ_DIR)/kmain.cxx.o $(OBJ_DIR)/pushpop.o $(OBJ_DIR)/gdt.o $(OBJ_DIR)/idt.o $(OBJ_DIR)/ISR.o $(OBJ_DIR)/MemSet.o $(OBJ_DIR)/paging.o $(OBJ_DIR)/gdt.cxx.o $(OBJ_DIR)/io.cxx.o $(OBJ_DIR)/paging.cxx.o $(OBJ_DIR)/terminal.cxx.o $(OBJ_DIR)/pic.cxx.o $(OBJ_DIR)/debugging.cxx.o $(OBJ_DIR)/kutil.cxx.o $(OBJ_DIR)/kbd.cxx.o $(OBJ_DIR_RES)/font.o $(OBJ_DIR)/gfx.cxx.o $(OBJ_DIR)/string.cxx.o $(OBJ_DIR)/uart.cxx.o -T $(RES_DIR)/Linkerscript

bootloader:
	$(MINGW) -c $(BOOTLOADER_SRC_DIR)/loader.c -o $(OBJ_DIR)/loader.o
	$(MINGW) -c $(BOOTLOADER_SRC_DIR)/data.c -o $(OBJ_DIR)/data.o
	$(MINGW) -c $(BOOTLOADER_SRC_DIR)/elf.c -o $(OBJ_DIR)/elf.o
	$(MINGW) -c $(BOOTLOADER_SRC_DIR)/error.c -o $(OBJ_DIR)/error.o
	$(MINGW) -c $(BOOTLOADER_SRC_DIR)/fs.c -o $(OBJ_DIR)/fs.o
	$(MINGW) -c $(BOOTLOADER_SRC_DIR)/graphics.c -o $(OBJ_DIR)/graphics.o
	$(MINGW) -c $(BOOTLOADER_SRC_DIR)/main.c -o $(OBJ_DIR)/main.o
	$(MINGW) -c $(BOOTLOADER_SRC_DIR)/serial.c -o $(OBJ_DIR)/serial.o
	ld $(OBJ_DIR)/main.o $(OBJ_DIR)/loader.o $(OBJ_DIR)/data.o $(OBJ_DIR)/elf.o $(OBJ_DIR)/error.o $(OBJ_DIR)/fs.o $(OBJ_DIR)/graphics.o /usr/lib/crt0-efi-x86_64.o $(OBJ_DIR)/serial.o -nostdlib -melf_x86_64 -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib -o $(BUILD_DIR)/boot.so -fpic -lefi -lgnuefi
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .reloc --target=efi-app-x86_64 $(BUILD_DIR)/boot.so $(BUILD_DIR)/bootloader.efi
	# x86_64-w64-mingw32-gcc -nostdlib -shared -Wl,--subsystem,10 -e efi_main -o bootloader.efi $(OBJ_DIR)/main.o $(OBJ_DIR)/loader.o $(OBJ_DIR)/data.o $(OBJ_DIR)/elf.o $(OBJ_DIR)/error.o $(OBJ_DIR)/fs.o $(OBJ_DIR)/graphics.o /usr/lib/crt0-efi-x86_64.o $(OBJ_DIR)/serial.o -lgcc

image: clean kernel bootloader
	$(IMAGE_GEN) $(BUILD_DIR)/microCORE.kernel $(BUILD_DIR)/bootloader.efi

qemu:
	$(MAKE) image ARCH=i686
	qemu-system-x86_64 -bios res/OVMF.fd -cdrom microNET.iso -m 512M
	
clean:
	rm -rfv $(BUILD_DIR) iso

all: image
