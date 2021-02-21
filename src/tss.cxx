#include "kernel/tss.hxx"
#include "kernel/gdt.hxx"
#include "kernel/memory.hxx"

extern "C" void __load_tss(uint16_t tss);

static cpu::tss::tss_t s_tss_descriptors[CONFIG_MAX_CPUS];

static void* isr_stack;
static void* irq_stack;
static void* sgi_stack;

void cpu::tss::tss_init() {
    memory::operations::memset(s_tss_descriptors, 0, sizeof(cpu::tss::tss_t) * CONFIG_MAX_CPUS);
}

void cpu::tss::tss_install(int num_cpu, uint64_t stack) {
    isr_stack = (memory::paging::allocation::request_page() + 0x1000);
    irq_stack = (memory::paging::allocation::request_page() + 0x1000);
    sgi_stack = (memory::paging::allocation::request_page() + 0x1000);


    uint64_t tss_base = (uint64_t)&s_tss_descriptors[num_cpu];
    memory::operations::memset((void *)tss_base, 0, sizeof(cpu::tss::tss_t));

    cpu::gdt::gdt_install_tss(tss_base, sizeof(cpu::tss::tss_t));

    s_tss_descriptors[num_cpu].rsp[0] = stack;
    s_tss_descriptors[num_cpu].io_map = sizeof(cpu::tss::tss_t);
    s_tss_descriptors[num_cpu].ist[0] = (uint64_t)isr_stack;
    s_tss_descriptors[num_cpu].ist[1] = (uint64_t)irq_stack;
    s_tss_descriptors[num_cpu].ist[2] = (uint64_t)sgi_stack;

    cpu::tss::load_tss(0x28);
}

void cpu::tss::tss_setstack(int num_cpu, uint64_t stack) {
    s_tss_descriptors[num_cpu].rsp[0] = stack;
}

extern "C" cpu::tss::tss_t* tss_get(int num_cpu) {
    return &s_tss_descriptors[num_cpu];
}

extern "C" void userspace_debug_catch()
{
    return;
}