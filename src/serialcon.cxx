#include <stdint.h>
#include "kernel/kutil.hxx"
#include "kernel/serialcon.hxx"
#include "kernel/io.hxx"

char serial_buffer[512];
int buffer_index = 0;

void read_command()
{
    if (!util::strcomp(serial_buffer, "test-commands"))
    {
        io::serial::serial_msg("We read you, loud and clear, sergeant!\n");
    }
}

void prompt()
{
    io::serial::serial_msg("\nkernel-console> ");
}

void clear_buffer()
{
    for (int i = 0; i < 512; i++)
    {
        serial_buffer[i] = '\0';
    }
}

void io::serial::console::init()
{
    io::pic::irq_unmask(4); // unmask the serial port IRQ line, therefore accepting interrupts when COM1 receives data
    clear_buffer();
    prompt();
}

void io::serial::console::read_character() // interrupt 4 will call this function
{
    serial_buffer[buffer_index] = inb(0x3F8);
    
    if (serial_buffer[buffer_index] == '\n')
    {
        serial_buffer[buffer_index] = '\0';
        read_command();
        clear_buffer();
        buffer_index = 0;
        prompt();
    }

    io::serial::serial_byte(serial_buffer[buffer_index]);

    buffer_index++;
}


