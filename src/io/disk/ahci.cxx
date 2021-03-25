#include "ahci.hxx"
#include "output/terminal.hxx"
#include "memory/memory.hxx"

io::pci::device_header* io::disk::ahci::ahci_controller;
io::disk::ahci::hba_memory* io::disk::ahci::abar;
io::disk::ahci::ahci_port* io::disk::ahci::ports[32];
uint08 io::disk::ahci::port_count;

void io::disk::ahci::initialize(pci::device_header* ahci_device_header) {
    ahci_controller = ahci_device_header;
    printf("\n\t\t\tahci devices:\n");
    abar = ((io::disk::ahci::hba_memory *)((io::pci::device_header_ex0 *)ahci_device_header)->bar5);
    memory::paging::map_memory((void *)abar, (void *)abar);
    probe_ports();
}

io::disk::ahci::port_type io::disk::ahci::get_port_type(hba_port* port) {
    uint32 sata_status = port->sata_status;
    uint08 interface_power_management = (sata_status >> 8) & 0x7;
    uint08 device_detection = sata_status & 0x7;

    if (device_detection != device_detection_code::present)
        return none;
    if (interface_power_management != ipm_code::active)
        return none;

    switch (port->signature)
    {
        case port_signature::sata_s:
            return port_type::sata;
        case port_signature::semb_s:
            return port_type::semb;
        case port_signature::sata_pi_s:
            return port_type::sata_pi;
        case port_signature::pm_s:
            return port_type::pm;
        default:
            return port_type::none;
    }
}

void io::disk::ahci::probe_ports() {
    uint32 implemented_ports = abar->ports_implemented;
    for (int i = 0; i < 32; i++) {
        if (implemented_ports & (1 << i)) {
            port_type current_port_type = get_port_type(&abar->ports[i]);

            switch (current_port_type)
            {
                case sata:
                    printf("\t\t\t\t - SATA drive\n");
                    break;
                case sata_pi:
                    printf("\t\t\t\t - SATA optical drive\n");
                    break;
                case pm:
                    printf("\t\t\t\t - PM drive\n");
                    break;
                case semb:
                    printf("\t\t\t\t - SEMB drive\n");
                    break;
                default:
                    printf("\t\t\t\t - Unknown drive\n");
                    break;
            }
        }
    }
}