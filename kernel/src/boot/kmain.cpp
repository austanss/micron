#include "types.h"
#include "drivers/kbd/kbd.h"
#include "boot/boot.h"
#include "cpu/interrupts.h"
#include "scheduling/timer.h"
#include "drivers/audio/beeper/beeper.h"
#include "boot/kconf.h"
#include "drivers/tty/tty.h"
#include "cpu/tss.h"
#include "fs/vfs/vfs.h"
#include "scheduling/elf.h"

#ifndef ARCH
    #define ARCH "$RED!UNKNOWN"
#endif

extern "C" address mnkmain(stivale_struct *bootloader_info, uint stack)
{	
	// do some startup configurations
	sys::config::calculate_kernel_size();
	sys::config::configure_memory(&(bootloader_info->framebuffer), &(bootloader_info->memory_map));
	cpu::tss::tss_install(0, stack);
	cpu::idt::load_idt();
	asm volatile ("sti");
	sys::config::configure_graphics(&(bootloader_info->framebuffer));
	sys::config::configure_pci(bootloader_info->rsdp);

	io::pit::set_c0_frequency(1000);
	io::pit::pit_init();

	sys::audio::pcspk::init();

	io::keyboard::init();

	return sys::prog::load_elf((sys::prog::elf_common_header *)bootloader_info->modules->begin);
}