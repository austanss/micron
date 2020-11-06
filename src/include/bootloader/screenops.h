//
// Created by rizet on 10/30/20.
//

#ifndef MICROCORE_SCREENOPS_H
#define MICROCORE_SCREENOPS_H

#include <efi/efi.h>
#include <efi/efilib.h>

#define print(message) ST->ConOut->OutputString(ST->ConOut, message);
#define clear_screen() efiCall(ST->ConOut->ClearScreen(ST->ConOut), L"error: screen wouldn't reset");

#endif //MICROCORE_SCREENOPS_H
