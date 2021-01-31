#include <stdint.h>
#include "kernel/kutil.hxx"
#include "kernel/serialcon.hxx"
#include "kernel/io.hxx"

char serial_buffer[512];
int buffer_index = 0;

void read_command()
{
    if (util::strcomp(serial_buffer, "test-commands"))
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
    outb(0x3FB, inb(0x3FB) | 0b10000000); // set the DLAB bit of the Line Control Register
    
    // now we can access the divisor registers
    // set the divisor to set the baud rate
    outb(0x3F8, 1);
    outb(0x3F9, 0);

    outb(0x3FB, 0b00000011); // set the line control register
    // No DLAB bit
    // 8 character bits
    // 1 stop bit
    // No parity bits

    outb(0x3F9, 0b00000001); // set the interrupt enable register
    // interrupt when data available


    io::pic::irq_unmask(4); // unmask the COM1 port IRQ line, therefore accepting generated interrupts from COM1
    clear_buffer();
    prompt();
}

void io::serial::console::read_character() // interrupt 4 will call this function
{
    serial_buffer[buffer_index] = inb(0x3F8);
    
    if (serial_buffer[buffer_index] == '\n')
    {
        serial_buffer[buffer_index] = '\0';
        io::serial::serial_msg("\r\n");
        read_command();
        clear_buffer();
        buffer_index = 0;
        prompt();
        io::serial::serial_byte(serial_buffer[buffer_index]);
    }
    else 
    {
        io::serial::serial_byte(serial_buffer[buffer_index]);
        buffer_index++;
    }

}


