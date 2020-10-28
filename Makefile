.DEFAULT_GOAL := all

MKRESCUE = grub-mkrescue

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
HEADERS_DIR := $(SRC_DIR)/include
OBJ_DIR := $(BUILD_DIR)/obj
OBJ_DIR_RES := $(OBJ_DIR)/res
CC := gcc -I$(HEADERS_DIR) -pipe -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wnon-virtual-dtor -g -fPIC -m32 -march=i686 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-pic -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
CXX := g++ -I$(HEADERS_DIR) -pipe -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wnon-virtual-dtor -g -fPIC -m32 -march=i686 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-pic -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
ASM := nasm -f elf
STATIC_LINK := llvm-ar
OBJCOPY := objcopy -O elf32-i386 -B i386 -I binary
CXX_LINK := g++ -m32 -Wl,--as-needed -Wl,--no-undefined -m32 -march=i686 -nostdlib -ffreestanding -O2 -Wall -Wextra -fno-pic -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive
IMAGE_GEN := $(RES_DIR)/GenerateImage

export MKRESCUE
export GCC
export BUILD_DIR

bin: clean
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR_RES)
	$(ASM) $(KERNEL_SRC_DIR)/IDT.asm -o $(OBJ_DIR)/IDT.o
	$(ASM) $(KERNEL_SRC_DIR)/GDT.asm -o $(OBJ_DIR)/GDT.o
	$(ASM) $(KERNEL_SRC_DIR)/ISR.asm -o $(OBJ_DIR)/ISR.o
	$(ASM) $(KERNEL_SRC_DIR)/MemSet.asm -o $(OBJ_DIR)/MemSet.o
	$(ASM) $(KERNEL_SRC_DIR)/Paging.asm -o $(OBJ_DIR)/Paging.o
	$(CC)  -MF$(OBJ_DIR)/Boot.S.o.d -o $(OBJ_DIR)/Boot.S.o -c $(KERNEL_SRC_DIR)/Boot.S
	$(CXX) -MF$(OBJ_DIR)/GDT.cxx.o.d -o $(OBJ_DIR)/GDT.cxx.o -c $(KERNEL_SRC_DIR)/GDT.cxx
	$(CXX) -MF$(OBJ_DIR)/PIC.cxx.o.d -o $(OBJ_DIR)/PIC.cxx.o -c $(KERNEL_SRC_DIR)/PIC.cxx
	$(CXX) -MF$(OBJ_DIR)/Paging.cxx.o.d -o $(OBJ_DIR)/Paging.cxx.o -c $(KERNEL_SRC_DIR)/Paging.cxx
	$(CXX) -MF$(OBJ_DIR)/IO.cxx.o.d -o $(OBJ_DIR)/IO.cxx.o -c $(KERNEL_SRC_DIR)/IO.cxx
	$(CXX) -MF$(OBJ_DIR)/Terminal.cxx.o.d -o $(OBJ_DIR)/Terminal.cxx.o -c $(KERNEL_SRC_DIR)/Terminal.cxx
	$(CXX) -MF$(OBJ_DIR)/DebugFunctions.cxx.o.d -o $(OBJ_DIR)/DebugFunctions.cxx.o -c $(KERNEL_SRC_DIR)/DebugFunctions.cxx
	$(CXX) -MF$(OBJ_DIR)/KernelUtil.cxx.o.d -o $(OBJ_DIR)/KernelUtil.cxx.o -c $(KERNEL_SRC_DIR)/KernelUtil.cxx
	$(CXX) -MF$(OBJ_DIR)/Keyboard.cxx.o.d -o $(OBJ_DIR)/Keyboard.cxx.o -c $(KERNEL_SRC_DIR)/Keyboard.cxx
	$(CXX) -MF$(OBJ_DIR)/Graphics.cxx.o.d -o $(OBJ_DIR)/Graphics.cxx.o -c $(KERNEL_SRC_DIR)/Graphics.cxx
	$(CXX) -MF$(OBJ_DIR)/Strings.cxx.o.d -o $(OBJ_DIR)/Strings.cxx.o -c $(KERNEL_SRC_DIR)/Strings.cxx
	$(CXX) -MF$(OBJ_DIR)/Kernel.cxx.o.d -o $(OBJ_DIR)/Kernel.cxx.o -c $(KERNEL_SRC_DIR)/Kernel.cxx -DARCH=\"$(ARCH)\"
	$(OBJCOPY) $(FONTS_DIR)/font.psf $(OBJ_DIR_RES)/font.o
	$(CXX_LINK) -o $(BUILD_DIR)/microCORE.kernel $(OBJ_DIR)/Kernel.cxx.o $(OBJ_DIR)/GDT.o $(OBJ_DIR)/IDT.o $(OBJ_DIR)/ISR.o $(OBJ_DIR)/MemSet.o $(OBJ_DIR)/Paging.o $(OBJ_DIR)/Boot.S.o $(OBJ_DIR)/GDT.cxx.o $(OBJ_DIR)/IO.cxx.o $(OBJ_DIR)/Paging.cxx.o $(OBJ_DIR)/Terminal.cxx.o $(OBJ_DIR)/PIC.cxx.o $(OBJ_DIR)/DebugFunctions.cxx.o $(OBJ_DIR)/KernelUtil.cxx.o $(OBJ_DIR)/Keyboard.cxx.o $(OBJ_DIR_RES)/font.o $(OBJ_DIR)/Graphics.cxx.o $(OBJ_DIR)/Strings.cxx.o -T $(RES_DIR)/Linkerscript

image: bin
	$(IMAGE_GEN) $(GRUB_CFG) $(BUILD_DIR)/microCORE.kernel $(MKRESCUE)

qemu:
	$(MAKE) image ARCH=i686
	qemu-system-x86_64 -cdrom microNET.iso -m 512M
	
clean:
	rm -rfv $(BUILD_DIR) iso

all: image
