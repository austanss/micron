#include <cstdint>
#include "kernel/kutil.hxx"
#include "kernel/terminal.hxx"
#include "kernel/memory.hxx"
#include "kernel/kbd.hxx"
#include "kernel/io.hxx"
#include "kernel/boot.hxx"
#include "kernel/idt.hxx"
#include "kernel/gfx.hxx"
#include "kernel/power.hxx"

#define __hang__ while (true);

#ifndef ARCH
    #define ARCH "$RED!UNKNOWN"
#endif


extern "C" {

// global constructors
void ctor_global
// required params
(/* none atm */)
// constructors called
{
	io::serial::serial_msg("GLOBAL CONSTRUCTORS CALLED\n");
}

void kernel_main(boot::boot_info *bootloader_info)
{
	boot::uefi = bootloader_info->runtime_services;

	io::serial::serial_msg("\nBOOTINFO ADDRESS: ");

	io::serial::serial_msg(util::itoa((uintptr_t)bootloader_info, 16));

	io::serial::serial_msg("\nBOOTINFO CHECKSUM: ");

	io::serial::serial_msg(util::itoa(bootloader_info->verification, 16));

	io::serial::serial_msg("\nFRAMEBUFFER ADDRESS: ");

	io::serial::serial_msg(util::itoa((uint64_t)bootloader_info->vbe_framebuffer->framebuffer_base, 16));
	io::serial::serial_msg("\n\n");

//	memory::paging::begin_paging();

	memory::allocation::map_memory(bootloader_info->memory_map, bootloader_info->mmap_size, bootloader_info->mmap_descriptor_size);

	memory::allocation::start_malloc();

	gfx::gop = *(bootloader_info->vbe_framebuffer);

	gfx::buffer = (uint32_t *)memory::allocation::kmalloc(gfx::gop.framebuffer_size + 3) + 3;

	*((uint8_t *)gfx::buffer - 3) = 'D';
	*((uint8_t *)gfx::buffer - 2) = 'F';
	*((uint8_t *)gfx::buffer - 1) = 'B';

	terminal::instance();
	
	printf("%s", "Booting...\n");

	printf("[$GREEN!kernel-startup$WHITE!] configuring paging\n");

	printf("[$GREEN!kernel-startup$WHITE!] mapping memory\n");

	printf("[$GREEN!kernel-startup$WHITE!] starting malloc\n");

	printf("[$GREEN!kernel-startup$WHITE!] configuring terminal\n");

	printf("[$GREEN!kernel-startup$WHITE!] starting keyboard\n");
	io::keyboard::init();

	io::serial::serial_msg("\n\nKernel configured.");

	gfx::shapes::dimensions term_size = terminal::get_optimal_size(gfx::shapes::dims(gfx::gop.x_resolution, gfx::gop.y_resolution));

	printf("[$GREEN!kernel-startup$WHITE!] res=%dx%d term=%dx%d\n", gfx::gop.x_resolution, gfx::gop.y_resolution, term_size.w, term_size.h);

	printf("[$GREEN!kernel-startup$WHITE!] boot success\n");

	printf("[$GREEN!kernel-checkup$WHITE!] testing memset: ");

	printf("in: %d", 25);

	int* tf25 = (int *)memory::allocation::kmalloc(4);

	memory::operations::memset(tf25, 25, 1);

	printf("out: %d\n", *tf25);
}

}
