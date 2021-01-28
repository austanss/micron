//
// Created by rizet on 11/27/20.
//

#include "kernel/power.hxx"
#include "kernel/boot.hxx"

// extern as C so can be called from ASM
extern "C"
{
void cpu::power::shutdown() {
	boot::uefi->reset_system(boot::efi_reset_shutdown, 0, 0, nullptr);
}

void cpu::power::restart_warm() {
	boot::uefi->reset_system(boot::efi_reset_warm, 0, 0, nullptr);
}

void cpu::power::restart_cold() {
	boot::uefi->reset_system(boot::efi_reset_cold, 0, 0, nullptr);
}
}