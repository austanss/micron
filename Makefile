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
CC := gcc -I$(HEADERS_DIR) -I$(OBJ_DIR)/pch -H -pipe -ftree-vectorize -D_FILE_OFFSET_BITS=64 -Wall -fno-pic -no-pie -Winvalid-pch -Wnon-virtual-dtor -g3 -ggdb -O0 -m64 -march=nocona -nostdlib -ffreestanding -Wall -Wextra -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
CXX := g++ -I$(HEADERS_DIR) -I$(OBJ_DIR)/pch -H -pipe -ftree-vectorize -D_FILE_OFFSET_BITS=64 -Wall -fno-pic -no-pie --std=gnu++17 -Winvalid-pch -Wnon-virtual-dtor -g3 -ggdb -O0 -m64 -march=nocona -nostdlib -ffreestanding -Wall -Wextra -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive -MD
ASM := nasm -f elf64
STATIC_LINK := llvm-ar
OBJCOPY := objcopy -O elf64-x86-64 -I binary
CXX_LINK := g++ -m64 -Wl,--as-needed -Wl,--no-undefined -H -no-pie -fno-pic -march=x86-64 -nostdlib -g3 -ggdb -O0 -ffreestanding -Wall -Wextra -fno-threadsafe-statics -Wl,--build-id=none -Wreturn-type -fpermissive
IMAGE_GEN := $(RES_DIR)/GenerateImage

export GCC
export BUILD_DIR

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/pch
	mkdir -p $(OBJ_DIR)/pch/kernel
	mkdir -p $(OBJ_DIR_RES)

headers:
	$(CXX) -o $(OBJ_DIR)/pch/kernel/idt.hxx.gch -c $(HEADERS_DIR)/kernel/idt.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/io.hxx.gch -c $(HEADERS_DIR)/kernel/io.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/terminal.hxx.gch -c $(HEADERS_DIR)/kernel/terminal.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/bitmap.hxx.gch -c $(HEADERS_DIR)/kernel/bitmap.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/kutil.hxx.gch -c $(HEADERS_DIR)/kernel/kutil.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/kconfigf.hxx.gch -c $(HEADERS_DIR)/kernel/kconfigf.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/kbd.hxx.gch -c $(HEADERS_DIR)/kernel/kbd.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/gdt.hxx.gch -c $(HEADERS_DIR)/kernel/gdt.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/gfx.hxx.gch -c $(HEADERS_DIR)/kernel/gfx.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/boot.hxx.gch -c $(HEADERS_DIR)/kernel/boot.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/power.hxx.gch -c $(HEADERS_DIR)/kernel/power.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/serialcon.hxx.gch -c $(HEADERS_DIR)/kernel/serialcon.hxx
	$(CXX) -o $(OBJ_DIR)/pch/kernel/memory.hxx.gch -c $(HEADERS_DIR)/kernel/memory.hxx

kernel: dirs headers
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR_RES)
	$(ASM) $(KERNEL_SRC_DIR)/idt.asm -o $(OBJ_DIR)/idt.o
	$(ASM) $(KERNEL_SRC_DIR)/gdt.asm -o $(OBJ_DIR)/gdt.o
	$(ASM) $(KERNEL_SRC_DIR)/interrupts.asm -o $(OBJ_DIR)/interrupts.o
	$(ASM) $(KERNEL_SRC_DIR)/kernel_entry.asm -o $(OBJ_DIR)/kernel_entry.o
	$(CXX) -MF$(OBJ_DIR)/idt.cxx.o.d -o $(OBJ_DIR)/idt.cxx.o -c $(KERNEL_SRC_DIR)/idt.cxx
	$(CXX) -MF$(OBJ_DIR)/pic.cxx.o.d -o $(OBJ_DIR)/pic.cxx.o -c $(KERNEL_SRC_DIR)/pic.cxx
	$(CXX) -MF$(OBJ_DIR)/io.cxx.o.d -o $(OBJ_DIR)/io.cxx.o -c $(KERNEL_SRC_DIR)/io.cxx
	$(CXX) -MF$(OBJ_DIR)/terminal.cxx.o.d -o $(OBJ_DIR)/terminal.cxx.o -c $(KERNEL_SRC_DIR)/terminal.cxx
	$(CXX) -MF$(OBJ_DIR)/bitmap.cxx.o.d -o $(OBJ_DIR)/bitmap.cxx.o -c $(KERNEL_SRC_DIR)/bitmap.cxx
	$(CXX) -MF$(OBJ_DIR)/printf.cxx.o.d -o $(OBJ_DIR)/printf.cxx.o -c $(KERNEL_SRC_DIR)/printf.cxx
	$(CXX) -MF$(OBJ_DIR)/kutil.cxx.o.d -o $(OBJ_DIR)/kutil.cxx.o -c $(KERNEL_SRC_DIR)/kutil.cxx
	$(CXX) -MF$(OBJ_DIR)/kconfigf.cxx.o.d -o $(OBJ_DIR)/kconfigf.cxx.o -c $(KERNEL_SRC_DIR)/kconfigf.cxx
	$(CXX) -MF$(OBJ_DIR)/kbd.cxx.o.d -o $(OBJ_DIR)/kbd.cxx.o -c $(KERNEL_SRC_DIR)/kbd.cxx
	$(CXX) -MF$(OBJ_DIR)/gfx.cxx.o.d -o $(OBJ_DIR)/gfx.cxx.o -c $(KERNEL_SRC_DIR)/gfx.cxx
	$(CXX) -MF$(OBJ_DIR)/kmain.cxx.o.d -o $(OBJ_DIR)/kmain.cxx.o -c $(KERNEL_SRC_DIR)/kmain.cxx -DARCH=\"$(ARCH)\"
	$(CXX) -MF$(OBJ_DIR)/boot.cxx.o.d -o $(OBJ_DIR)/boot.cxx.o -c $(KERNEL_SRC_DIR)/boot.cxx
	$(CXX) -MF$(OBJ_DIR)/power.cxx.o.d -o $(OBJ_DIR)/power.cxx.o -c $(KERNEL_SRC_DIR)/power.cxx
	$(CXX) -MF$(OBJ_DIR)/serialcon.cxx.o.d -o $(OBJ_DIR)/serialcon.cxx.o -c $(KERNEL_SRC_DIR)/serialcon.cxx
	$(CXX) -MF$(OBJ_DIR)/memory.cxx.o.d -o $(OBJ_DIR)/memory.cxx.o -c $(KERNEL_SRC_DIR)/memory.cxx
	$(CXX_LINK) -o $(BUILD_DIR)/microCORE.kernel $(OBJ_DIR)/kernel_entry.o $(OBJ_DIR)/kmain.cxx.o $(OBJ_DIR)/boot.cxx.o $(OBJ_DIR)/bitmap.cxx.o $(OBJ_DIR)/serialcon.cxx.o $(OBJ_DIR)/printf.cxx.o $(OBJ_DIR)/power.cxx.o $(OBJ_DIR)/gdt.o $(OBJ_DIR)/idt.o $(OBJ_DIR)/interrupts.o $(OBJ_DIR)/idt.cxx.o $(OBJ_DIR)/io.cxx.o $(OBJ_DIR)/kconfigf.cxx.o $(OBJ_DIR)/memory.cxx.o $(OBJ_DIR)/terminal.cxx.o $(OBJ_DIR)/pic.cxx.o $(OBJ_DIR)/kutil.cxx.o $(OBJ_DIR)/kbd.cxx.o $(OBJ_DIR)/gfx.cxx.o -T $(RES_DIR)/Linkerscript

clean:
	rm -rfv $(BUILD_DIR)/*.* $(OBJ_DIR)/*.* iso

all: kernel
