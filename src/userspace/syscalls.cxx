#include "types.hxx"

#include "io/io.hxx"

extern "C" void ex_syscall_handler(uint syscall_number, uint primary_parameter/*, uint secondary_parameter*/)
{
    switch (syscall_number)
    {
        case 1:
            io::serial::serial_msg((char *)primary_parameter);
            break;

        default:
            break;
    }
}