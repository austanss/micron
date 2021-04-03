#include "ahci.hxx"
#include "output/terminal.hxx"
#include "memory/memory.hxx"

io::pci::device_header* io::disk::ahci::ahci_controller;
io::disk::ahci::hba_memory* io::disk::ahci::abar;
io::disk::ahci::ahci_port* io::disk::ahci::ports[32];
uint08 io::disk::ahci::usable_ports;

void io::disk::ahci::initialize(pci::device_header* ahci_device_header) {
    ahci_controller = ahci_device_header;
    printf("\n\t\t\tahci devices:\n");
    abar = ((io::disk::ahci::hba_memory *)((io::pci::device_header_ex0 *)ahci_device_header)->bar5);
    memory::paging::map_memory((void *)abar, (void *)abar);
    probe_ports();

    for (int i = 0; i < usable_ports; i++)
        configure_port(i);
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
                    printf("\t\t\t\t - Unknown/unplugged drive\n");
                    break;
            }

            if (current_port_type != none)
            {
                ports[usable_ports] = (ahci_port *)memory::allocation::malloc(sizeof(ahci_port));
                ports[usable_ports]->port = &abar->ports[i];
                ports[usable_ports]->ahci_port_number = usable_ports;
                usable_ports++;
            }
        }
    }
}

void io::disk::ahci::configure_port(uint port_number)
{
    ahci_port* port = ports[port_number];

    command_stop(port_number);

    void* new_list_base = memory::paging::allocation::request_page();
    port->port->command_list_base = (uint32)(uint64)new_list_base;
    port->port->command_list_base_upper = (uint32)((uint64)new_list_base >> 32);
    memory::operations::memset((void *)(port->port->command_list_base), 0x00, 0x400);

    void* new_fis_base = memory::paging::allocation::request_page();
    port->port->fis_base = (uint32)(uint64)new_fis_base;
    port->port->fis_base_upper = (uint32)((uint64)new_fis_base >> 32);
    memory::operations::memset((void *)(port->port->command_list_base), 0x00, 0x100);

    hba_command_header* command_header = (hba_command_header *)((port->port->command_list_base) | (uint32)((uint64)port->port->command_list_base_upper << 32));

    for (int i = 0; i < 32; i++) {
        command_header[i].prdt_length = 8;
        void* command_table_address = memory::paging::allocation::request_page();
        uint64 address = (uint64)command_table_address + (i << 8);
        command_header[i].command_table_base = (uint32)address;
        command_header[i].command_table_base_upper = (uint32)(address << 32);
        memory::operations::memset((void *)address, 0, 0x100);
    }


    command_start(port_number);
}

void io::disk::ahci::command_start(uint port_number)
{
    ahci_port* port = ports[port_number];
    
    while (port->port->command_status & command_status_field::pxcmd_cr);

    port->port->command_status |= command_status_field::pxcmd_fre;
    port->port->command_status |= command_status_field::pxcmd_st;
}

void io::disk::ahci::command_stop(uint port_number)
{
    ahci_port* port = ports[port_number];

    port->port->command_status &= ~command_status_field::pxcmd_fre;
    port->port->command_status &= ~command_status_field::pxcmd_st;   

    while (true) {
        if (port->port->command_status & command_status_field::pxcmd_fr) continue;
        if (port->port->command_status & command_status_field::pxcmd_cr) continue;
        break;
    }
}

int io::disk::ahci::command_read(uint port_number, uint64 sector, uint32 sector_count, void* buffer)
{
    ahci_port* port = ports[port_number];

    uint64 spinlock = 0;

    while ((port->port->task_file_data & (tf_data_bits::busy | tf_data_bits::drq)) && spinlock < 1000000)
        spinlock++;

    if (spinlock >= 1000000)
        return ahci_read_status::device_hung;

    uint32 sector_low = (uint32)sector;
    uint32 sector_high = (uint32)(sector >> 32);

    port->port->interrupt_status = (uint32)-1;

    hba_command_header* command_header = (hba_command_header *)port->port->command_list_base;
    command_header = (hba_command_header *)(((uint64)command_header) | (((uint64)port->port->command_list_base_upper) << 32));

    command_header->command_fis_length = sizeof(fis_register_h2d) / sizeof(uint32);
    command_header->write = false;
    command_header->prdt_length = 1;

    hba_command_table* command_table = (hba_command_table *)command_header->command_table_base;
    command_table = (hba_command_table *)(((uint64)command_table) | (((uint64)command_header->command_table_base_upper) << 32));

    memory::operations::memset(command_table, 0, sizeof(hba_command_table) + (command_header->prdt_length - 1) * sizeof(hba_prdt_entry));

    command_table->prdt_entries[0].data_base_address = (uint32)((address)buffer);
    command_table->prdt_entries[0].data_base_address_upper = (uint32)((address)buffer >> 32);
    command_table->prdt_entries[0].byte_count = (sector_count * 512);
    command_table->prdt_entries[0].interrupt_on_completion = 1;

    fis_register_h2d* command_fis = (fis_register_h2d *)&command_table->command_fis;
    command_fis->fis_type = fis_type::reg_h2d;
    command_fis->cc = 1;
    command_fis->command = sata_command::read_dma_ex;

    command_fis->lba0 = (uint08)sector_low;
    command_fis->lba1 = (uint08)sector_low >> 8;
    command_fis->lba2 = (uint08)sector_low >> 16;
    command_fis->lba3 = (uint08)sector_low >> 24;
    command_fis->lba4 = (uint08)sector_high;
    command_fis->lba5 = (uint08)sector_high >> 8;

    command_fis->device = 1 << 6;

    command_fis->count_low = sector_count & 0xFF;
    command_fis->count_high = (sector_count >> 8) & 0xFF;

    port->port->command_issue = 1;

    while (true)
    {
        if (!port->port->command_issue)
            break;

        if (port->port->interrupt_status & hba_pxis::tfes)
            return ahci_read_status::task_file_error;
    }

    return ahci_read_status::success;
}