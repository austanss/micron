.DEFAULT_GOAL := all

MKRESCUE = grub-mkrescue

ifeq (, $(shell which grub-mkrescue 2>/dev/null))
	ifeq (, $(shell which grub2-mkrescue 2>/dev/null))
		$(error "No grub-mkrescue or grub2-mkrescue in $(PATH)")
	else
		MKRESCUE = grub2-mkrescue
	endif
endif


ARCH := UNSPECIFIED
ROOT_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_DIR := $(ROOT_DIR)/bin
SRC_DIR := $(ROOT_DIR)/microCORE
HAL_DIR := $(SRC_DIR)/HardwareAbstractionLayer
HAL_SRC_DIR := $(HAL_DIR)/Architectures/i686
KERNEL_SRC_DIR := $(SRC_DIR)/Kernel
OBJ_DIR := $(BUILD_DIR)/obj
OBJ_DIR_HAL := $(OBJ_DIR)/HAL
CC := clang -I$(HAL_SRC_DIR) -I$(HAL_SRC_DIR) -I$(HAL_DIR)/Headers -I$(SRC_DIR)/Utilities -Xclang -fcolor-diagnostics -pipe -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wnon-virtual-dtor -g -fPIC --target=i686-pc-none-elf -march=i686 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-pic -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
CXX := clang++ -I$(HAL_SRC_DIR) -I$(HAL_SRC_DIR) -I$(KERNEL_SRC_DIR) -I$(HAL_DIR)/Headers -I$(SRC_DIR)/Utilities -Xclang -fcolor-diagnostics -pipe -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wnon-virtual-dtor -g -fPIC --target=i686-pc-none-elf -march=i686 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-pic -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
ASM := nasm -f elf
STATIC_LINK := llvm-ar
CXX_LINK := clang++ -o -Wl,--as-needed -Wl,--no-undefined --target=i686-pc-none-elf -march=i686 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-pic -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive
IMAGE_GEN := $(KERNEL_SRC_DIR)/gen_image.sh
GRUB_CFG := $(KERNEL_SRC_DIR)/grub.i686.cfg

export MKRESCUE
export GCC
export BUILD_DIR

bin: clean
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR_HAL)
	$(ASM) $(HAL_SRC_DIR)/IDT.asm -o $(OBJ_DIR_HAL)/IDT.o
	$(ASM) $(HAL_SRC_DIR)/GDT.asm -o $(OBJ_DIR_HAL)/GDT.o
	$(ASM) $(HAL_SRC_DIR)/ISR.asm -o $(OBJ_DIR_HAL)/ISR.o
	$(ASM) $(HAL_SRC_DIR)/MemSet.asm -o $(OBJ_DIR_HAL)/MemSet.o
	$(ASM) $(HAL_SRC_DIR)/Paging.asm -o $(OBJ_DIR_HAL)/Paging.o
	$(CC)  -MF$(OBJ_DIR_HAL)/Boot.S.o.d -o $(OBJ_DIR_HAL)/Boot.S.o -c $(HAL_SRC_DIR)/Boot.S
	$(CXX) -MF$(OBJ_DIR_HAL)/GDT.cpp.o.d -o $(OBJ_DIR_HAL)/GDT.cpp.o -c $(HAL_SRC_DIR)/GDT.cpp
	$(CXX) -MF$(OBJ_DIR_HAL)/PIC.cpp.o.d -o $(OBJ_DIR_HAL)/PIC.cpp.o -c $(HAL_SRC_DIR)/PIC.cpp
	$(CXX) -MF$(OBJ_DIR_HAL)/Paging.cpp.o.d -o $(OBJ_DIR_HAL)/Paging.cpp.o -c $(HAL_SRC_DIR)/Paging.cpp
	$(CXX) -MF$(OBJ_DIR_HAL)/IO.cpp.o.d -o $(OBJ_DIR_HAL)/IO.cpp.o -c $(HAL_SRC_DIR)/IO.cpp
	$(CXX) -MF$(OBJ_DIR_HAL)/Terminal.cpp.o.d -o $(OBJ_DIR_HAL)/Terminal.cpp.o -c $(HAL_SRC_DIR)/HALFunctions/Terminal.cpp
	$(CXX) -MF$(OBJ_DIR_HAL)/DebugFunctions.cpp.o.d -o $(OBJ_DIR_HAL)/DebugFunctions.cpp.o -c $(HAL_SRC_DIR)/DebugFunctions.cpp
	$(CXX) -MF$(OBJ_DIR_HAL)/KernelUtil.cpp.o.d -o $(OBJ_DIR_HAL)/KernelUtil.cpp.o -c $(HAL_SRC_DIR)/KernelUtil.cpp
	$(CXX) -MF$(OBJ_DIR_HAL)/Kernel.cpp.o.d -o $(OBJ_DIR)/Kernel.cpp.o -c $(KERNEL_SRC_DIR)/Kernel.cpp -DARCH=\"$(ARCH)\"
	$(CXX_LINK) -o $(BUILD_DIR)/microCORE.kernel $(OBJ_DIR)/Kernel.cpp.o $(OBJ_DIR_HAL)/GDT.o $(OBJ_DIR_HAL)/IDT.o $(OBJ_DIR_HAL)/ISR.o $(OBJ_DIR_HAL)/MemSet.o $(OBJ_DIR_HAL)/Paging.o $(OBJ_DIR_HAL)/Boot.S.o $(OBJ_DIR_HAL)/GDT.cpp.o $(OBJ_DIR_HAL)/IO.cpp.o $(OBJ_DIR_HAL)/Paging.cpp.o $(OBJ_DIR_HAL)/Terminal.cpp.o $(OBJ_DIR_HAL)/PIC.cpp.o $(OBJ_DIR_HAL)/DebugFunctions.cpp.o $(OBJ_DIR_HAL)/KernelUtil.cpp.o -T $(HAL_SRC_DIR)/Linker.ld

image: bin
	$(IMAGE_GEN) $(GRUB_CFG) $(BUILD_DIR)/microCORE.kernel $(MKRESCUE)

qemu:
	$(MAKE) image ARCH=i686
	qemu-system-i386 -cdrom microNET.iso -m 512M
	
clean:
	rm -rfv $(BUILD_DIR) iso

all: image
