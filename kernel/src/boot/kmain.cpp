#include "types.h"
#include "drivers/kbd/kbd.h"
#include "boot/boot.h"
#include "cpu/interrupts.h"
#include "scheduling/timer.h"
#include "drivers/audio/beeper/beeper.h"
#include "drivers/gfx/gop/gop.h"
#include "boot/kconf.h"
#include "drivers/tty/tty.h"
#include "cpu/tss.h"
#include "fs/gpt/gpt.h"
#include "scheduling/elf.h"
#include "memory/pmm.h"
#include "memory/paging.h"

#ifndef ARCH
    #define ARCH "$RED!UNKNOWN"
#endif

using namespace memory::pmm;

extern "C" void ring3_call();

extern "C" address mnkmain(stivale2_struct *bootloader_info, uint stack)
{	
	stivale2_struct_tag_framebuffer* framebuffer_info = (stivale2_struct_tag_framebuffer *)find_tag(bootloader_info->tags, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	stivale2_struct_tag_memmap* memory_map = (stivale2_struct_tag_memmap *)find_tag(bootloader_info->tags, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	stivale2_struct_tag_rsdp* rsdp_info = (stivale2_struct_tag_rsdp *)find_tag(bootloader_info->tags, STIVALE2_STRUCT_TAG_RSDP_ID);
	stivale2_struct_tag_modules* modules = (stivale2_struct_tag_modules *)find_tag(bootloader_info->tags, STIVALE2_STRUCT_TAG_MODULES_ID);

	// do some startup configurations
	sys::config::calculate_kernel_size();
	sys::config::configure_memory(framebuffer_info, memory_map);
	cpu::tss::tss_install(0, stack);
	cpu::idt::load_idt();
	asm volatile ("sti");
	sys::config::configure_pci((sys::acpi::rsdp2 *)rsdp_info->rsdp);

	io::pit::set_c0_frequency(1000);
	io::pit::pit_init();

	sys::audio::pcspk::init();

	gfx::gop = *framebuffer_info;

	io::keyboard::init();

	fs::gpt::read_gpt_info(0);

	memory::paging::donate_to_userspace((void *)ring3_call);

	return sys::prog::load_elf((sys::prog::elf_common_header *)modules->modules[0].begin);
}