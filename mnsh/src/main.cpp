#include "keyboard.h"
#include "ecpuid.h"
#include "tty.h"
#include "evsys.h"
#include "sysinfo.h"
#include "string.h"

int command_buffer_index = 0;
char* $command_buffer;
bool prompt_waiting = false;

void perform_command(char* command);

void prompt()
{
    print("$red!> $light_blue!i-mnsh $red!>> ");
    prompt_waiting = true;

    while (prompt_waiting);

    print("\n");

    perform_command($command_buffer);

    memset((void *)$command_buffer, 0, 0x1000);
    command_buffer_index = 0;
}

void transitional_flash()
{
    memset((void *)get_info()->display_info.fb_ad, 0xFF, get_info()->display_info.fb_size);
    memset((void *)get_info()->display_info.fb_ad, 0xBF, get_info()->display_info.fb_size);
    memset((void *)get_info()->display_info.fb_ad, 0x7F, get_info()->display_info.fb_size);
    memset((void *)get_info()->display_info.fb_ad, 0x3F, get_info()->display_info.fb_size);
    memset((void *)get_info()->display_info.fb_ad, 0x0F, get_info()->display_info.fb_size);
    memset((void *)get_info()->display_info.fb_ad, 0x00, get_info()->display_info.fb_size);
}

extern "C" 
void main()
{
    transitional_flash();

    tty_initialize();

    print("\n\t$cyan!mnsh v2.0$white!\n\n");

    enable_evsys_handling();

    $command_buffer = (char *)pmap(0xcc99000000);
    memset((void *)$command_buffer, 0, 0x1000);

    while (true)
        prompt();
}

const char* cmd_freemem_str = "freemem";
const char* cmd_cpuid_str = "cpuid";
const char* cmd_help_str = "help";

void cmd_freemem();
void cmd_cpuid();
void cmd_help();

void perform_command(char* command)
{
    if (strcmp(command, cmd_cpuid_str))
    { cmd_cpuid(); return; }

    if (strcmp(command, cmd_freemem_str))
    { cmd_freemem(); return; }
    
    if (strcmp(command, cmd_help_str))
    { cmd_help(); return; }

    if (!*$command_buffer) return;

    print("$red!error: $white!unknown command\n");
}