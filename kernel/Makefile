SRCDIR 	= src
BINDIR  = bin
OBJDIR  = bin/obj

CPPSRC 	= $(call rwildcard,$(SRCDIR),*.cpp)
ASMSRC 	= $(call rwildcard,$(SRCDIR),*.asm)
LDS		= src/mnk.lds

OBJS 	= $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.cc.o, $(CPPSRC))
OBJS 	+= $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%.as.o, $(ASMSRC))
SYMS	= $(OBJDIR)/syms.o
KERNEL	= $(BINDIR)/mnk.elf

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CC 		= x86_64-elf-gcc
AS 		= nasm
GS		= ./gensyms.sh

# Flags to enable all (reasonable) warnings possible
# and treat them as errors to ensure code quality
WRFLAGS	=\
-Werror \
-Wall \
-Wno-int-to-pointer-cast \
-Wno-pointer-arith

CCFLAGS 	= \
-ffreestanding \
-I$(SRCDIR) \
-gdwarf \
-O0 \
-std=c++20 \
-mno-red-zone \
-msse3 \
-fno-threadsafe-statics \
-fno-omit-frame-pointer \
-mcmodel=kernel \
$(WRFLAGS)

LDFLAGS  = -ffreestanding \
-nostdlib \
-z max-page-size=0x1000 \
-lgcc \
-T $(LDS)

ASFLAGS	= \
-f elf64 \
-g \
-F dwarf 

.DEFAULT-GOAL 	= all
.PHONY			= clean

$(OBJDIR)/%.cc.o: $(SRCDIR)/%.cpp
	@ echo "MNK  >]		CC $^"
	@ mkdir -p $(@D)
	@ $(CC) $(CCFLAGS) -c $^ -o $@

$(OBJDIR)/%.as.o: $(SRCDIR)/%.asm
	@ echo "MNK  >]		AS $^"
	@ mkdir -p $(@D)
	@ $(AS) $(ASFLAGS) $^ -o $@

$(KERNEL): $(OBJS)
	@ echo "MNK  >]		LD $@"
	@ $(CC) $(LDFLAGS) $(OBJS) -o $@

	@ echo "MNK  >]		GS $@"
	@ $(GS) $@

	@ echo "MNK  >]		CC syms.gen"
	@ $(CC) -fno-pic -fpie -xc -c syms.gen -o $(SYMS)

	@ echo "MNK  >]		RM syms.gen"
	@ rm syms.gen

	@ echo "MNK  >]		LD $@"
	@ $(CC) $(LDFLAGS) $(OBJS) $(SYMS) -o $@

all: kernel
kernel: $(KERNEL)

clean:
	@ rm -rf bin