#include "types.hxx"

#include "io/io.hxx"

extern "C" void syscall_handler_ex(uint syscall_number, uint primary_parameter, uint secondary_parameter)
{
    (void)secondary_parameter;
    
    switch (syscall_number)
    {
        case 1:
            io::serial::serial_msg((char *)primary_parameter);
            break;

        default:
            break;
    }
}