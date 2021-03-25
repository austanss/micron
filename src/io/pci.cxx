#include "pci.hxx"
#include "disk/ahci.hxx"
#include "memory/memory.hxx"
#include "output/terminal.hxx"

sys::acpi::mcfg_header* io::pci::mcfg;

void enumerate_function(address device_address, uint function)
{
    uint64 offset = function << 12;
    address function_address = device_address + offset;
    memory::paging::map_memory((void *)function_address, (void *)function_address);

    io::pci::device_header* device_header = (io::pci::device_header *)function_address;

    if (!device_header->device_id)
        return;
    if (device_header->device_id == 0xFFFF)
        return;

    printf("\t\t%hx %hx", device_header->vendor_id, device_header->device_id);

    switch (device_header->device_class) {
        case 0x01: // mass storage controller
            switch (device_header->device_subclass) {
                case 0x06: //Serial ATA 
                    switch (device_header->program_interface) {
                        case 0x01: //AHCI 1.0 device
                            printf(" - ahci sata controller\n");
                            io::disk::ahci::initialize(device_header);
                            break;
                    }
            }
    }
    printf("\n");
}

void enumerate_device(address bus_address, uint device)
{
    uint64 offset = device << 15;
    address device_address = bus_address + offset;
    memory::paging::map_memory((void *)device_address, (void *)device_address);

    io::pci::device_header* device_header = (io::pci::device_header *)device_address;

    if (!device_header->device_id)
        return;
    if (device_header->device_id == 0xFFFF)
        return;

    for (int function = 0; function < 8; function++)
        enumerate_function(device_address, function);
}

void enumerate_bus(address base, uint bus)
{
    uint64 offset = bus << 20;
    address bus_address = base + offset;
    memory::paging::map_memory((void *)bus_address, (void *)bus_address);

    io::pci::device_header* device_header = (io::pci::device_header *)bus_address;

    if (!device_header->device_id)
        return;
    if (device_header->device_id == 0xFFFF)
        return;

    for (int device = 0; device < 32; device++)
        enumerate_device(bus_address, device);
}

void io::pci::enumerate_pci()
{
    printf("\n\n\n\n\n\n\n\n\tpci functions:\n\n");
    int mcfg_entries = ((mcfg->header.length) - sizeof(sys::acpi::mcfg_header)) / sizeof(io::pci::device_configuration);
    for (int i = 0; i < mcfg_entries; i++)
    {
        io::pci::device_configuration* current_device = (io::pci::device_configuration *)((uint64)mcfg + sizeof(sys::acpi::mcfg_header) + (sizeof(io::pci::device_configuration) * i));
        for (uint bus = current_device->start_bus; bus < current_device->end_bus; bus++)
            enumerate_bus(current_device->base, bus);
    }
}