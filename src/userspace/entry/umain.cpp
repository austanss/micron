#include <stdint.h>
#include "keyboard.h"
#include "ecpuid.h"

// TEMPORARY: These syscalls only exist for v0.6, and will be changed many times before v1.0
enum syscall {
    copy_framebuffer,
    allocate_page,
    write_tty,
    subscribe_kbd_event,
    get_info
};

enum info_field {
    total_ram,
    free_ram
};

__attribute__((section(".rodata.userspace")))
const char itoa_characters[] = "0123456789abcdef";

__attribute__((section(".data.userspace")))
char itoa_buf[64] = { 0 };

__attribute__((section(".userspace")))
char* itoa(long int val, int base)
{
	int i = 60;

	for(; val && i ; --i, val /= base)

		itoa_buf[i] = itoa_characters[val % base];

	return &itoa_buf[i+1];
}

__attribute__((section(".userspace")))
bool $strcmp(const char* lhs, const char *rhs)
{
    const char* slhs = lhs;

	for (; *lhs; lhs++)
	{
		if (*lhs != *rhs)
			return false;

		rhs++;
	}

    if (!*slhs) return false;

	return true;
}

__attribute__((section(".userspace")))
void $print(const char * text)
{
    asm volatile ("syscall" : : "a" (write_tty));
}

__attribute__((section(".userspace")))
uint64_t $get_info(uint8_t field)
{
    uint64_t rax;
    asm volatile ("syscall" : "=a" (rax) : "a" (get_info));
    return rax;
}

__attribute__((section(".userspace")))
void $subscribe_keyboard_event(void (*handler)(keyboard_event_args))
{
    asm volatile ("syscall" : : "a" (subscribe_kbd_event));
}

__attribute__((section(".data.userspace")))
int command_buffer_index = 0;
__attribute__((section(".data.userspace")))
char* $command_buffer;
__attribute__((section(".data.userspace")))
bool prompt_waiting = false;
__attribute__((section(".data.userspace")))
bool event_occured = false;
__attribute__((section(".data.userspace")))
bool unhandled_backspace = false;

__attribute__((section(".userspace")))
void* $allocate_page()
{
    uint64_t rax = 0;
    asm volatile ("syscall" : "=a" (rax) : "a" (allocate_page));
    return (void *)rax;
}

__attribute__((section(".userspace")))
void* $memset(void* buffer, unsigned char value, unsigned long count)
{
	unsigned char* cbuffer = (unsigned char *)buffer;
	for (unsigned long i = 0; i < count; i++)
		cbuffer[i] = value;
	return buffer;
}

__attribute__((section(".userspace")))
void keyboard_event(keyboard_event_args event_args)
{
    if (!event_args.release_or_press)
        return;
    
    char character = kcascii(event_args.key_code, event_args.shifted, event_args.caps_lock);
    
    if (character == '\t')
    {
        command_buffer_index = (command_buffer_index / 4 + 1) * 4;
        return;
    }

    if (character == '\b')
    {
        command_buffer_index--;
        $command_buffer[command_buffer_index] = 0;
        unhandled_backspace = true;
        return;
    }

    if (character == '\n')
    {
        prompt_waiting = false;
        return;
    }

    if (!character)
        return;

    $command_buffer[command_buffer_index] = character;
    command_buffer_index++;

    event_occured = true;

    return;
}

void $perform_command(char* command);

__attribute__((section(".userspace")))
void $prompt()
{
    $print("$RED!> $LIGHT_BLUE!imnus-cmd $RED!>> ");
    prompt_waiting = true;
    while (prompt_waiting)
    { 
        if (event_occured)
        {
            $print(&($command_buffer[command_buffer_index - 1]));
            event_occured = false;
        }

        if (unhandled_backspace)
        {
            $print("\b");
            unhandled_backspace = false;
        }
    }

    $print("\n");

    $perform_command($command_buffer);

    $memset((void *)$command_buffer, 0, 0x1000);
    command_buffer_index = 0;
}

extern "C" 
__attribute__((section(".userspace")))
void umain()
{
    $print("\n\t$CYAN!imnus v1.0$WHITE!\n\n");

    $subscribe_keyboard_event(keyboard_event);

    $command_buffer = (char *)$allocate_page();
    $memset((void *)$command_buffer, 0, 0x1000);

    while (true)
        $prompt();
}

__attribute__((section(".rodata.userspace")))
const char $meminfo[] = "meminfo";

__attribute__((section(".rodata.userspace")))
const char $cpuinfo[] = "cpuinfo";

__attribute__((section(".rodata.userspace")))
const char $help[] = "help";

__attribute__((section(".userspace")))
void $$meminfo()
{
    uint64_t total_ram, free_ram, used_ram;

    total_ram = $get_info(info_field::total_ram);
    free_ram = $get_info(info_field::free_ram);
    used_ram = total_ram - free_ram;

    $print("\n\t$LIGHT_GREY!total ram: ");
    $print(itoa(total_ram / 1024 / 1024, 10));
    $print("M\n\t$LIGHT_GREY!free ram: ");
    $print(itoa(free_ram / 1024 / 1024, 10));
    $print("M\n\t$LIGHT_GREY!used ram: ");
    $print(itoa(used_ram / 1024 / 1024, 10));
    $print("M\n\n");
}

__attribute__((section(".userspace"))) 
void $$cpu_info()
{
    $print("\n\tsupports x64: $GREEN!yes\n");
    $print("\tsupports acpi: $GREEN!yes\n");
    $print("\tsupports apic: ");
    $print(cpuid_get_field(ECX, CPUID_FEAT_EDX_APIC) ? "$GREEN!yes\n" : "$RED!no\n");
    $print("\tsupports pae: $GREEN!yes\n");
    $print("\tsupports mmx: $GREEN!yes\n");
    $print("\tsupports sse: $GREEN!yes\n");
    $print("\tsupports sse2: $GREEN!yes\n");
    $print("\tsupports sse3: ");
    $print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSE3) ? "$GREEN!yes\n" : "$RED!no\n");
    $print("\tsupports ssse3: ");
    $print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSSE3) ? "$GREEN!yes\n" : "$RED!no\n");
    $print("\tsupports sse4.1: ");
    $print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSE4_1) ? "$GREEN!yes\n" : "$RED!no\n");
    $print("\tsupports sse4.2: ");
    $print(cpuid_get_field(ECX, CPUID_FEAT_ECX_SSE4_2) ? "$GREEN!yes\n" : "$RED!no\n");
    $print("\tsupports avx: ");
    $print(cpuid_get_field(ECX, CPUID_FEAT_ECX_AVX) ? "$GREEN!yes\n\n" : "$RED!no\n\n");
}

__attribute__((section(".userspace"))) 
void $$help()
{
    $print("\n\tmeminfo:        $LIGHT_GREEN!get info about memory\n");
    $print("\tcpuinfo:          $LIGHT_GREEN!get info about the cpu\n");
    $print("\thelp:             $LIGHT_GREEN!display this current menu\n\n");
}

__attribute__((section(".userspace"))) 
void $perform_command(char* command)
{
    if ($strcmp(command, $cpuinfo))
    {
        $$cpu_info();
        return;
    }

    if ($strcmp(command, $meminfo))
    {
        $$meminfo();
        return;
    }
    
    if ($strcmp(command, $help))
    {
        $$help();
        return;
    }

    if (!*$command_buffer) return;

    $print("$RED!error: $WHITE!unknown command\n");
}