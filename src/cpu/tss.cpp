#include "cpu/tss.h"
#include "cpu/gdt.h"
#include "memory/memory.h"

static cpu::tss::tss_t s_tss_descriptors[CONFIG_MAX_CPUS];

static void* isr_stack;
static void* irq_stack;
static void* sgi_stack;

void cpu::tss::tss_init() {
    memory::operations::memset(s_tss_descriptors, 0, sizeof(cpu::tss::tss_t) * CONFIG_MAX_CPUS);
}

void cpu::tss::tss_install(int num_cpu, address stack) {
    isr_stack = (void *)((address)memory::paging::allocation::request_page() + 0x1000);
    irq_stack = (void *)((address)memory::paging::allocation::request_page() + 0x1000);
    sgi_stack = (void *)((address)memory::paging::allocation::request_page() + 0x1000);

    memory::paging::map_memory((void *)((address)isr_stack - 0x1000), (void *)((address)isr_stack - 0x1000), false);
    memory::paging::map_memory((void *)((address)irq_stack - 0x1000), (void *)((address)irq_stack - 0x1000), false);
    memory::paging::map_memory((void *)((address)sgi_stack - 0x1000), (void *)((address)sgi_stack - 0x1000), false);

    address tss_base = (address)&s_tss_descriptors[num_cpu];
    memory::operations::memset((void *)tss_base, 0, sizeof(cpu::tss::tss_t));

    cpu::gdt::gdt_install_tss(tss_base, sizeof(cpu::tss::tss_t));

    s_tss_descriptors[num_cpu].rsp[0] = stack;
    s_tss_descriptors[num_cpu].io_map = sizeof(cpu::tss::tss_t);
    s_tss_descriptors[num_cpu].ist[0] = (address)isr_stack;
    s_tss_descriptors[num_cpu].ist[1] = (address)irq_stack;
    s_tss_descriptors[num_cpu].ist[2] = (address)sgi_stack;

    cpu::tss::load_tss(0x28);
}

void cpu::tss::tss_setstack(int num_cpu, address stack) {
    s_tss_descriptors[num_cpu].rsp[0] = stack;
}

extern "C" cpu::tss::tss_t* tss_get(int num_cpu) {
    return &s_tss_descriptors[num_cpu];
}

extern "C" void userspace_debug_catch()
{
    return;
}