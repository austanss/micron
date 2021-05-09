#include "boot/kconf.h"
#include "boot/boot.h"
#include "drivers/io.h"
#include "drivers/acpi/pci.h"
#include "util/kutil.h"
#include "memory/paging.h"
#include "memory/operations.h"
#include "memory/pmm.h"
#include "memory/heap.h"
#include "drivers/tty/tty.h"
#include "drivers/gfx/gop/gop.h"
#include "util/bitmap.h"

uint64 sys::config::__kernel_pages;
uint64 sys::config::__kernel_size;

extern util::bitmap page_bitmap_map;

extern "C" void configure_pat();

void sys::config::setup_paging(stivale_framebuffer *framebuffer)
{
    configure_pat();

	memory::paging::pml_4 = (memory::paging::page_table *)memory::pmm::request_page();
	
	memory::operations::memset(memory::paging::pml_4, 0, 0x1000);

    memory::paging::map_memory((void*)memory::paging::pml_4, (void*)memory::paging::pml_4, false);
    
	for (address t = 0x0; t < memory::pmm::get_total_memory_size(nullptr, 0, 0); t+=0x1000)
	    { memory::paging::map_memory((void *)t, (void *)t, false); }

	address fb_base = (address)framebuffer->framebuffer_addr;
    uint64 fb_size = (((uint64)framebuffer->framebuffer_width * framebuffer->framebuffer_height * (framebuffer->framebuffer_bpp / 8) + 1024));

    for (uint64 t = fb_base; t < fb_base + fb_size; t+=0x1000)
        { memory::paging::map_memory((void*)t, (void*)t, true); }

	asm volatile ("mov %0, %%cr3" : : "r" (memory::paging::pml_4));	
}

void sys::config::configure_memory(stivale_framebuffer *framebuffer, stivale_memory_map *memory_map)
{
	memory::operations::memset((void *)framebuffer->framebuffer_addr, 0xFF, 0xFFFF);
    memory::pmm::initialize(memory_map, memory_map->memory_map_entries * sizeof(stivale_mmap_entry), sizeof(stivale_mmap_entry));

    sys::config::setup_paging(framebuffer);

    memory::heap::initialize_heap((void*)0xffff800000000000, 0x100);
}

void sys::config::configure_graphics(stivale_framebuffer *framebuffer)
{
    gfx::gop = *(framebuffer);

	gfx::buffer = (uint32 *)memory::pmm::request_pages(gfx::gop.framebuffer_width * gfx::gop.framebuffer_height * (gfx::gop.framebuffer_bpp / 8) / 0x1000 + 1);

	io::tty::initialize();
}

void sys::config::configure_pci(sys::acpi::rsdp2* rsdp)
{
    sys::acpi::rsdp = rsdp;
    sys::acpi::mcfg_header* mcfg = (sys::acpi::mcfg_header *)sys::acpi::get_table((char *)"MCFG");
    io::pci::mcfg = mcfg;
    io::pci::enumerate_pci();
}

void sys::config::calculate_kernel_size()
{
    __kernel_size = (address)&__kernel_end - (address)&__kernel_start;
    __kernel_pages = (uint64)__kernel_size / 4096 + 1;
}