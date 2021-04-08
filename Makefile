SRCDIR 	= src
BINDIR  = bin
OBJDIR  = bin/obj

CXXSRC 	= $(call rwildcard,$(SRCDIR),*.cxx)
ASMSRC 	= $(call rwildcard,$(SRCDIR),*.asm)
LDS		= microCORE.lds

OBJS 	= $(patsubst $(SRCDIR)/%.cxx, $(OBJDIR)/%.cxx.o, $(CXXSRC))
OBJS 	+= $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%.asm.o, $(ASMSRC))

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CXXC 	= x86_64-elf-gcc
ASMC 	= nasm

# Flags to enable all (reasonable) warnings possible
# and treat them as errors to ensure code quality
WFLAGS	=\
-Werror \
-Wall \
-Wno-int-to-pointer-cast

CFLAGS 	= -ffreestanding -I$(SRCDIR) -fno-pic -fpie -std=c++2a -gdwarf -O0 -mno-red-zone -msse3 -fno-threadsafe-statics $(WFLAGS)
LFLAGS  = -ffreestanding -nostdlib -fno-pic -fpie -static-pie -pie -z max-page-size=0x1000 -lgcc -T $(LDS)
AFLAGS	= -f elf64 -g -F dwarf 

.DEFAULT-GOAL = all

$(OBJDIR)/%.cxx.o: $(SRCDIR)/%.cxx
	@ echo "=>==>>===>>> COMPILING $^"
	@ mkdir -p $(@D)
	@ $(CXXC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%.asm.o: $(SRCDIR)/%.asm
	@ echo "=>==>>===>>> ASSEMBLING $^"
	@ mkdir -p $(@D)
	@ $(ASMC) $(AFLAGS) $^ -o $@

kernel: $(OBJS)
	@ echo "=>==>>===>>> LINKING bin/microCORE.kernel"
	@ $(CXXC) $(LFLAGS) $(OBJS) -o $(BINDIR)/microCORE.kernel

all: kernel

clean:
	@ rm -rf bin