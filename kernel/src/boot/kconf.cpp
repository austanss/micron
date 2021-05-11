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

void sys::config::setup_paging(stivale2_struct_tag_framebuffer *framebuffer)
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
    {
        memory::paging::map_memory((void*)t, (void*)t, true);
        memory::paging::donate_to_userspace((void*)t);
    }

    for (address t = 0x0; t < 0x80000000; t+=0x1000)
	    { memory::paging::map_memory((void *)t + 0xffffffff80000000, (void *)t, false); }

	asm volatile ("mov %0, %%cr3" : : "r" (memory::paging::pml_4));	
}

void sys::config::configure_memory(stivale2_struct_tag_framebuffer *framebuffer, stivale2_struct_tag_memmap *memory_map)
{
	memory::operations::memset((void *)framebuffer->framebuffer_addr, 0xFF, 0xFFFF);

    // Also initializes heap as a dependency
    memory::pmm::initialize(&memory_map->memmap[0], memory_map->entries, sizeof(stivale2_mmap_entry));

    sys::config::setup_paging(framebuffer);

    memory::heap::initialize_heap((void*)0xffff800000000000, 0x100);
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