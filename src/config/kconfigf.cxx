#include "config/kconfigf.hxx"
#include "start/boot.hxx"
#include "io/io.hxx"
#include "io/pci.hxx"
#include "util/kutil.hxx"
#include "memory/memory.hxx"
#include "output/terminal.hxx"
#include "output/gfx.hxx"
#include "util/bitmap.hxx"


uint64 sys::config::_kernel_pages;
uint64 sys::config::_kernel_size;

extern util::bitmap page_bitmap_map;

void sys::config::setup_paging(stivale_framebuffer *framebuffer)
{
	memory::paging::pml_4 = (memory::paging::page_table *)memory::paging::allocation::request_page();
	
	memory::operations::memset(memory::paging::pml_4, 0, 0x1000);

    memory::paging::map_memory((void*)memory::paging::pml_4, (void*)memory::paging::pml_4);
    
	for (address t = 0x0; t < memory::allocation::get_total_memory_size(nullptr, 0, 0); t+=0x1000)
	    { memory::paging::map_memory((void *)t, (void *)t); }

	address fb_base = (address)framebuffer->framebuffer_addr;
    uint64 fb_size = (((uint64)framebuffer->framebuffer_width * framebuffer->framebuffer_height * (framebuffer->framebuffer_bpp / 8) + 1024));

    for (uint64 t = fb_base; t < fb_base + fb_size; t+=0x1000)
        { memory::paging::map_memory((void*)t, (void*)t); }

	asm volatile ("mov %0, %%cr3" : : "r" (memory::paging::pml_4));	
}

void sys::config::configure_memory(stivale_framebuffer *framebuffer, stivale_memory_map *memory_map)
{
    memory::allocation::map_memory(memory_map, memory_map->memory_map_entries * sizeof(stivale_mmap_entry), sizeof(stivale_mmap_entry));
    sys::config::setup_paging(framebuffer);
    
    for (int t = 0; t < 0x100; t++) { 
        void* pos = (void*)0xffff800000000000;
        memory::paging::map_memory(pos, memory::paging::allocation::request_page());
        pos = (void*)((uint64)pos + 0x1000);
    }

    memory::allocation::initialize_heap((void*)0xffff800000000000, 0x100000);
}

void sys::config::configure_graphics(stivale_framebuffer *framebuffer)
{
    gfx::gop = *(framebuffer);

	gfx::buffer = (uint32 *)memory::paging::allocation::request_pages(gfx::gop.framebuffer_width * gfx::gop.framebuffer_height * (gfx::gop.framebuffer_bpp / 8) / 0x1000 + 1);

	terminal::instance();
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
    _kernel_size = (address)&_kernel_end - (address)&_kernel_start;
    _kernel_pages = (uint64)_kernel_size / 4096 + 1;
}

extern "C" void userspace_entry();
void sys::config::configure_userspace()
{
    memory::paging::donate_to_userspace((void *)&userspace_entry);
}