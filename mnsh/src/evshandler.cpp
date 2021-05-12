#include "evsys.h"
#include "keyboard.h"
#include "tty.h"

extern int command_buffer_index;
extern char* $command_buffer;
extern bool prompt_waiting;

void keyboard_event(evsys_packet* packet)
{
    keyboard_event_args* event_args = (keyboard_event_args *)((unsigned long)packet + sizeof(evsys_packet));

    if (!event_args->release_or_press)
        return;
    
    char character = kcascii(event_args->key_code, event_args->shifted, event_args->caps_lock);
    
    if (character == '\t')
    {
        command_buffer_index = (command_buffer_index / 4 + 1) * 4;
        return;
    }

    if (character == '\b')
    {
        command_buffer_index--;
        $command_buffer[command_buffer_index] = 0;
        print("\b");
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

    tty_putc(character, white);

    return;
}