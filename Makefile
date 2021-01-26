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
KERNEL_SRC_DIR := $(SRC_DIR)
HEADERS_DIR := include
OBJ_DIR := $(BUILD_DIR)/obj
OBJ_DIR_RES := $(OBJ_DIR)/res
CC := gcc -I$(HEADERS_DIR) -pipe -D_FILE_OFFSET_BITS=64 -Wall -fno-pic -no-pie -Winvalid-pch -Wnon-virtual-dtor -g -m64 -march=x86-64 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
CXX := g++ -I$(HEADERS_DIR) -pipe -D_FILE_OFFSET_BITS=64 -Wall -fno-pic -no-pie --std=gnu++17 -Winvalid-pch -Wnon-virtual-dtor -g -m64 -march=x86-64 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
ASM := nasm -f elf64
STATIC_LINK := llvm-ar
OBJCOPY := objcopy -O elf64-x86-64 -I binary
CXX_LINK := g++ -m64 -Wl,--as-needed -Wl,--no-undefined -no-pie -fno-pic -march=x86-64 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive
IMAGE_GEN := $(RES_DIR)/GenerateImage

export GCC
export BUILD_DIR

kernel:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR_RES)
	$(ASM) $(KERNEL_SRC_DIR)/idt.asm -o $(OBJ_DIR)/idt.o
	$(ASM) $(KERNEL_SRC_DIR)/gdt.asm -o $(OBJ_DIR)/gdt.o
	$(ASM) $(KERNEL_SRC_DIR)/interrupts.asm -o $(OBJ_DIR)/interrupts.o
	$(ASM) $(KERNEL_SRC_DIR)/memory.asm -o $(OBJ_DIR)/memory.o
	$(ASM) $(KERNEL_SRC_DIR)/kernel_entry.asm -o $(OBJ_DIR)/kernel_entry.o
	$(CXX) -MF$(OBJ_DIR)/idt.cxx.o.d -o $(OBJ_DIR)/idt.cxx.o -c $(KERNEL_SRC_DIR)/idt.cxx
	$(CXX) -MF$(OBJ_DIR)/pic.cxx.o.d -o $(OBJ_DIR)/pic.cxx.o -c $(KERNEL_SRC_DIR)/pic.cxx
	$(CXX) -MF$(OBJ_DIR)/memory.cxx.o.d -o $(OBJ_DIR)/memory.cxx.o -c $(KERNEL_SRC_DIR)/memory.cxx
	$(CXX) -MF$(OBJ_DIR)/io.cxx.o.d -o $(OBJ_DIR)/io.cxx.o -c $(KERNEL_SRC_DIR)/io.cxx
	$(CXX) -MF$(OBJ_DIR)/terminal.cxx.o.d -o $(OBJ_DIR)/terminal.cxx.o -c $(KERNEL_SRC_DIR)/terminal.cxx
	$(CXX) -MF$(OBJ_DIR)/debugging.cxx.o.d -o $(OBJ_DIR)/debugging.cxx.o -c $(KERNEL_SRC_DIR)/debugging.cxx
	$(CXX) -MF$(OBJ_DIR)/kutil.cxx.o.d -o $(OBJ_DIR)/kutil.cxx.o -c $(KERNEL_SRC_DIR)/kutil.cxx
	$(CXX) -MF$(OBJ_DIR)/kbd.cxx.o.d -o $(OBJ_DIR)/kbd.cxx.o -c $(KERNEL_SRC_DIR)/kbd.cxx
	$(CXX) -MF$(OBJ_DIR)/gfx.cxx.o.d -o $(OBJ_DIR)/gfx.cxx.o -c $(KERNEL_SRC_DIR)/gfx.cxx
	$(CXX) -MF$(OBJ_DIR)/uart.cxx.o.d -o $(OBJ_DIR)/uart.cxx.o -c $(KERNEL_SRC_DIR)/uart.cxx
	$(CXX) -MF$(OBJ_DIR)/string.cxx.o.d -o $(OBJ_DIR)/string.cxx.o -c $(KERNEL_SRC_DIR)/string.cxx
	$(CXX) -MF$(OBJ_DIR)/kmain.cxx.o.d -o $(OBJ_DIR)/kmain.cxx.o -c $(KERNEL_SRC_DIR)/kmain.cxx -DARCH=\"$(ARCH)\"
	$(CXX) -MF$(OBJ_DIR)/bootinfo.cxx.o.d -o $(OBJ_DIR)/bootinfo.cxx.o -c $(KERNEL_SRC_DIR)/bootinfo.cxx
	$(CXX) -MF$(OBJ_DIR)/power.cxx.o.d -o $(OBJ_DIR)/power.cxx.o -c $(KERNEL_SRC_DIR)/power.cxx
	$(CXX_LINK) -o $(BUILD_DIR)/microCORE.kernel $(OBJ_DIR)/kernel_entry.o $(OBJ_DIR)/kmain.cxx.o $(OBJ_DIR)/bootinfo.cxx.o $(OBJ_DIR)/power.cxx.o $(OBJ_DIR)/gdt.o $(OBJ_DIR)/idt.o $(OBJ_DIR)/interrupts.o $(OBJ_DIR)/memory.o $(OBJ_DIR)/idt.cxx.o $(OBJ_DIR)/io.cxx.o $(OBJ_DIR)/memory.cxx.o $(OBJ_DIR)/terminal.cxx.o $(OBJ_DIR)/pic.cxx.o $(OBJ_DIR)/debugging.cxx.o $(OBJ_DIR)/kutil.cxx.o $(OBJ_DIR)/kbd.cxx.o $(OBJ_DIR_RES)/font.o $(OBJ_DIR)/gfx.cxx.o $(OBJ_DIR)/string.cxx.o $(OBJ_DIR)/uart.cxx.o -T $(RES_DIR)/Linkerscript

clean:
	rm -rfv $(BUILD_DIR) iso

all: kernel
