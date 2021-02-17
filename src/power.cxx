//
// Created by rizet on 11/27/20.
//

#include "kernel/power.hxx"
#include "kernel/boot.hxx"
#include "kernel/terminal.hxx"

// extern as C so can be called from ASM
extern "C"
{
void cpu::power::shutdown() {
	terminal::instance().clear();
	asm volatile ("cli; hlt");
}

void cpu::power::restart_warm() {
	asm volatile ("jmp 0x0");
}

void cpu::power::restart_cold() {
	asm volatile ("jmp 0x0");
}
}