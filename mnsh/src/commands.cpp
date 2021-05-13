#include "sysinfo.h"
#include "ecpuid.h"
#include "tty.h"
#include "string.h"

void cmd_help()
{
    print("\n\tfreemem:         $light_green!get info about memory\n");
    print("\tcpuid:             $light_green!get info about the cpu\n");
    print("\thelp:              $light_green!display this current menu\n\n");
}

void cmd_cpuid()
{
    print("\n\tsupports x64: $green!yes\n");
    print("\tsupports acpi: $green!yes\n");
    print("\tsupports apic: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_EDX_APIC) ? "$green!yes\n" : "$red!no\n");
    print("\tsupports pae: $green!yes\n");
    print("\tsupports mmx: $green!yes\n");
    print("\tsupports sse: $green!yes\n");
    print("\tsupports sse2: $green!yes\n");
    print("\tsupports sse3: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSE3) ? "$green!yes\n" : "$red!no\n");
    print("\tsupports ssse3: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSSE3) ? "$green!yes\n" : "$red!no\n");
    print("\tsupports sse4.1: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSE4_1) ? "$green!yes\n" : "$red!no\n");
    print("\tsupports sse4.2: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSE4_2) ? "$green!yes\n" : "$red!no\n");
    print("\tsupports avx: ");
    print(cpuid_get_field(ECX, CPUID_FEAT_ECX_AVX) ? "$green!yes\n\n" : "$red!no\n\n");
}

void cmd_freemem()
{
    unsigned long total_ram, free_ram, used_ram;

    system_info* sysinfo = get_info();

    total_ram = sysinfo->memory_info.total_ram;
    free_ram = sysinfo->memory_info.free_ram;
    used_ram = sysinfo->memory_info.used_ram;

    print("\n\t$light_grey!total ram: ");
    print(itoa(total_ram / 1024 / 1024, 10));
    print("M\n\t$light_grey!free ram: ");
    print(itoa(free_ram / 1024 / 1024, 10));
    print("M\n\t$light_grey!used ram: ");
    print(itoa(used_ram / 1024 / 1024, 10));
    print("M\n\n");
}