#include "acpi.h"
#include "io.h"

sys::acpi::rsdp2* sys::acpi::rsdp;

sys::acpi::sdt_header* sys::acpi::get_table(char* signature)
{
    io::serial::serial_byte('\n');
    uint xsdt_entries = (rsdp->xsdt->length - sizeof(sys::acpi::sdt_header)) / 8; 
    for (uint i = 0; i < xsdt_entries; i++)
    {
        struct sys::acpi::sdt_header* current_sdt_header = (sys::acpi::sdt_header *)*(address *)((uint64)rsdp->xsdt + sizeof(sys::acpi::sdt_header) + (i * 8));
        for (int j = 0; j < 4; j++) {
            if (signature[j] != current_sdt_header->signature[j])
                break;    
            if (j == 3)
                return current_sdt_header;
        }
    }
    return nullptr;
}