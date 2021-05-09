SRCDIR 	= src
BINDIR  = bin
OBJDIR  = bin/obj

CPPSRC 	= $(call rwildcard,$(SRCDIR),*.cpp)

OBJS 	= $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.cc.o, $(CPPSRC))
MNSH	= $(BINDIR)/mnsh.elf

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CC 		= x86_64-elf-gcc
LD		= x86_64-elf-gcc

.DEFAULT-GOAL 	= all
.PHONY			= clean

CCFLAGS 	= \
-ffreestanding \
-I$(SRCDIR) \
-fno-pic \
-fpie \
-std=c++20 \
$(WRFLAGS)

LDFLAGS  = \
-ffreestanding \
-nostdlib \
-fno-pic \
-fpie \
-pie \
-static-pie \
-e main

$(OBJDIR)/%.cc.o: $(SRCDIR)/%.cpp
	@ echo "MNSH >]		CC $^"
	@ mkdir -p $(@D)
	@ $(CC) $(CCFLAGS) -c $^ -o $@

$(MNSH): $(OBJS)
	@ $(LD) $(LDFLAGS) $(OBJS) -o $@

all: $(MNSH)

clean:
	@ rm -rf $(BINDIR)
