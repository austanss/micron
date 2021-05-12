#include "sysinfo.h"
#include "ecpuid.h"
#include "tty.h"
#include "string.h"

void cmd_help()
{
    print("\n\tmeminfo:        $LIGHT_GREEN!get info about memory\n");
    print("\tcpuinfo:          $LIGHT_GREEN!get info about the cpu\n");
    print("\thelp:             $LIGHT_GREEN!display this current menu\n\n");
}

void cmd_cpuid()
{
    print("\n\tsupports x64: $GREEN!yes\n");
    print("\tsupports acpi: $GREEN!yes\n");
    print("\tsupports apic: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_EDX_APIC) ? "$GREEN!yes\n" : "$RED!no\n");
    print("\tsupports pae: $GREEN!yes\n");
    print("\tsupports mmx: $GREEN!yes\n");
    print("\tsupports sse: $GREEN!yes\n");
    print("\tsupports sse2: $GREEN!yes\n");
    print("\tsupports sse3: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSE3) ? "$GREEN!yes\n" : "$RED!no\n");
    print("\tsupports ssse3: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSSE3) ? "$GREEN!yes\n" : "$RED!no\n");
    print("\tsupports sse4.1: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSE4_1) ? "$GREEN!yes\n" : "$RED!no\n");
    print("\tsupports sse4.2: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSE4_2) ? "$GREEN!yes\n" : "$RED!no\n");
    print("\tsupports avx: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_ECX_AVX) ? "$GREEN!yes\n\n" : "$RED!no\n\n");
}

void cmd_freemem()
{
    unsigned long total_ram, free_ram, used_ram;

    system_info* sysinfo = get_info();

    total_ram = sysinfo->memory_info.total_ram;
    free_ram = sysinfo->memory_info.free_ram;
    used_ram = sysinfo->memory_info.used_ram;

    print("\n\t$LIGHT_GREY!total ram: ");
    print(itoa(total_ram / 1024 / 1024, 10));
    print("M\n\t$LIGHT_GREY!free ram: ");
    print(itoa(free_ram / 1024 / 1024, 10));
    print("M\n\t$LIGHT_GREY!used ram: ");
    print(itoa(used_ram / 1024 / 1024, 10));
    print("M\n\n");
}