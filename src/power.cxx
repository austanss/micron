//
// Created by rizet on 11/27/20.
//

#include "kernel/power.hxx"
#include "kernel/bootinfo.hxx"

// extern as C so can be called from ASM
extern "C"
{
void shutdown() {
	UEFI->ResetSystem(EfiResetShutdown, 0, 0, nullptr);
}

void restart_warm() {
	UEFI->ResetSystem(EfiResetWarm, 0, 0, nullptr);
}

void restart_cold() {
	UEFI->ResetSystem(EfiResetCold, 0, 0, nullptr);
}
}