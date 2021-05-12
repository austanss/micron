#include "evsys.h"
#include "syscalls.h"

unsigned short id_list[] {
    0x5286
};

void keyboard_event(evsys_packet* packet);

evsys_recipient_descriptor evrd {
    .handler = keyboard_event,
    .id_list_entries = 1,
    .reserved = 0,
    .id_list = &id_list[0]
};

void _levrd(evsys_recipient_descriptor* tblevrd)
{
    asm volatile ("syscall" : : "a" (levrd));
}