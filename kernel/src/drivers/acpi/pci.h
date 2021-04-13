#pragma once

#include "types.h"
#include "acpi.h"

namespace io {
    namespace pci {
        struct device_configuration {
            address base;
            uint16 pci_segment_group;
            uint08 start_bus;
            uint08 end_bus;
            uint08 pad[4];
        } __attribute__((packed));

        struct device_header {
            uint16 vendor_id;
            uint16 device_id;
            uint16 command;
            uint16 status;
            uint08 revision_id;
            uint08 program_interface;
            uint08 device_subclass;
            uint08 device_class;
            uint08 cache_line_size;
            uint08 latency_timer;
            uint08 header_type;
            uint08 bist;
        };

        struct device_header_ex0 {
            device_header common;
            uint32 bar0;
            uint32 bar1;
            uint32 bar2;
            uint32 bar3;
            uint32 bar4;
            uint32 bar5;
            uint32 cardbus_cis_pointer;
            uint16 subsystem_vendor_id;
            uint16 subsystem_id;
            uint32 expansion_rom_base;
            uint08 capabillities_ptr;
            uint08 pad[7];
            uint08 interrupt_line;
            uint08 interrupt_pin;
            uint08 minimum_grant;
            uint08 maximum_latency;
        };

        extern sys::acpi::mcfg_header* mcfg;

        void enumerate_pci();
    }
}