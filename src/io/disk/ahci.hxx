#pragma once
#include "types.hxx"
#include "../pci.hxx"

namespace io {
    namespace disk {
        namespace ahci {
            enum port_type {
                none = 0,
                sata = 1,
                semb = 2,
                pm = 3,
                sata_pi = 4
            };

            enum device_detection_code {
                present = 0x03
            };

            enum ipm_code {
                active = 0x01
            };

            enum port_signature {
                sata_pi_s = 0xEB140101,
                sata_s = 0x00000101,
                semb_s = 0xC33C0101,
                pm_s = 0x96690101
            };

            struct hba_port {
                uint64 command_list_base;
                uint64 fis_base;
                uint32 interrupt_status;
                uint32 interrupt_enable;
                uint32 command_status;
                uint08 pad[4];
                uint32 task_file_data;
                uint32 signature;
                uint32 sata_status;
                uint32 sata_control;
                uint32 sata_error;
                uint32 sata_active;
                uint32 command_issue;
                uint32 sata_notification;
                uint32 fis_switch_control;
                uint08 pad_ex[44];
                uint32 vendor[4];
            };

            struct hba_memory {
                uint32 host_capability;
                uint32 global_host_control;
                uint32 interrupt_status;
                uint32 ports_implemented;
                uint32 version;
                uint32 c3_control;
                uint32 c3_ports;
                uint32 enclosure_management_location;
                uint32 enclosure_management_control;
                uint32 host_capability_ex;
                uint32 bios_handoff_control_status;
                uint08 pad[116];
                uint08 vendor[96];
                hba_port ports[1];
            };

            struct ahci_port {
                hba_port* port;
                port_type type;
                uint08* dma_buffer;
                uint08* ahci_port_number;
            };

            void initialize(pci::device_header* ahci_device_header);
            void probe_ports();
            port_type get_port_type(hba_port* port);
            extern pci::device_header* ahci_controller;
            extern hba_memory* abar;
            extern ahci_port* ports[];
            extern uint08 usable_ports;
        }
    }
}