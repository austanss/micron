#pragma once
#include "types.h"
#include "drivers/acpi/pci.h"

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

            enum command_status_field {
                pxcmd_st = 0x0001,
                pxcmd_fre = 0x0010,
                pxcmd_fr = 0x4000,
                pxcmd_cr = 0x8000
            };

            enum fis_type {
                reg_h2d	= 0x27,	// Register FIS - host to device
                reg_d2h	= 0x34,	// Register FIS - device to host
                dma_act	= 0x39,	// DMA activate FIS - device to host
                dma_setup = 0x41,	// DMA setup FIS - bidirectional
                data = 0x46,	// Data FIS - bidirectional
                bist = 0x58,	// BIST activate FIS - bidirectional
                pio_setup = 0x5F,	// PIO setup FIS - device to host
                dev_bits = 0xA1,	// Set device bits FIS - device to host
            };

            enum sata_command {
                read_pio = 0x20,
                read_dma_ex = 0x25,
                write_pio = 0x30,
                write_dma_ex = 0x35,
                packet = 0xA1,
                flush = 0xE7,
                identify = 0xEC
            };

            enum tf_data_bits {
                busy = 0x80,
                drq = 0x08
            };

            enum hba_pxis {
                tfes = (1 << 30)
            };

            enum ahci_read_status {
                r_success = 0,
                r_device_hung = 1,
                r_task_file_error = 2,
                r_device_too_busy = 3
            };

            enum ahci_write_status {
                w_success = 0,
                w_device_hung = 1,
                w_task_file_error = 2,
                w_device_too_busy = 3
            };

            struct hba_port {
                uint32 command_list_base;
                uint32 command_list_base_upper;
                uint32 fis_base;
                uint32 fis_base_upper;
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

            struct hba_command_header {
                uint08 command_fis_length       : 5;
                uint08 sata_pi                  : 1;
                uint08 write                    : 1;
                uint08 prefetchable             : 1;
                uint08 reset                    : 1;
                uint08 bist                     : 1;
                uint08 clear_busy               : 1;
                uint08 pad                      : 1;
                uint08 port_multiplier          : 4;
                uint16 prdt_length              : 16;
                uint32 prdb_count               : 32;
                uint32 command_table_base       : 32;
                uint32 command_table_base_upper : 32;
                uint08 pad_ex[16];
            };

            struct hba_prdt_entry {
                uint32 data_base_address;
                uint32 data_base_address_upper;
                uint32 pad;
                uint32 byte_count               : 22;
                uint32 pad_ex                   : 9;
                uint08 interrupt_on_completion  : 1;
            };

            struct hba_command_table {
                uint08 command_fis[64];
                uint08 sata_pi_command[16];
                uint08 pad[48];
                hba_prdt_entry prdt_entries[1];
            };

            struct ahci_port {
                hba_port* port;
                port_type type;
                uint08* dma_buffer;
                uint08 ahci_port_number;
            };

            struct fis_register_h2d {
                uint08 fis_type;
                uint08 port_multiplier  : 4;
                uint08 pad              : 3;
                uint08 cc               : 1;
                uint08 command;
                uint08 feature_low;
                uint08 lba0;	
                uint08 lba1;
                uint08 lba2;	
                uint08 device;
                uint08 lba3;	
                uint08 lba4;	
                uint08 lba5;	
                uint08 feature_high;
                uint08 count_low;		
                uint08 count_high;
                uint08 icc;		
                uint08 control;
                uint08 pad_ex[4];	
            };

            struct fis_register_d2h {
                uint08 fis_type;    
                uint08 port_multiplier  : 4;    
                uint08 pad              : 2;    
                uint08 interrupt        : 1;    
                uint08 pad_ex           : 1;    
                uint08 status;      
                uint08 error;       
                uint08 lba0;        
                uint08 lba1;        
                uint08 lba2;        
                uint08 device;      
                uint08 lba3;        
                uint08 lba4;        
                uint08 lba5;        
                uint08 pad_ex_ex;        
                uint08 count_low;
                uint08 count_high;
                uint08 pad_ex_ex_ex[6];    
            };

            struct fis_data {
                uint08 fis_type;	
                uint08 port_multiplier : 4;	
                uint08 pad             : 4;
                uint08 pad_ex[2];
                uint32 data[1];
            };

            struct fis_pio_setup {
                uint08 fis_type;
                uint08 port_multiplier      : 4;	
                uint08 pad                  : 1;		
                uint08 transfer_direction   : 1;		
                uint08 interrupt            : 1;		
                uint08 pad_ex               : 1;
                uint08 status;		
                uint08 error;		
                uint08 lba0;		
                uint08 lba1;		
                uint08 lba2;		
                uint08 device;		
                uint08 lba3;		
                uint08 lba4;		
                uint08 lba5;		
                uint08 pad_ex_ex;		
                uint08 count_low;		
                uint08 count_high;		
                uint08 pad_ex_ex_ex;		
                uint08 e_status;	
                uint16 transfer_count;	
                uint08 rsv4[2];
            };

            struct fis_dma_setup {
                uint08 fis_type;
                uint08 port_multiplier    : 4;	
                uint08 pad                : 1;	           
                uint08 transfer_direction : 1;	
                uint08 interrupt          : 1;	                
                uint08 auto_activate      : 1;
                uint08 pad_ex[2];  
                uint64 dma_buffer_id;
                uint32 pad_ex_ex;
                uint32 dma_buffer_offset; 
                uint32 transfer_count;
                uint32 pad_ex_ex_ex;
            };

            void initialize(pci::device_header* ahci_device_header);
            void probe_ports();
            void configure_port(uint port_number);
            void command_start(uint port_number);
            void command_stop(uint port_number);
            int command_read(uint port_number, uint64 sector, uint16 sector_count, void* buffer);
            int command_write(uint port_number, uint64 sector, uint16 sector_count, void* data);
            port_type get_port_type(hba_port* port);
            extern pci::device_header* ahci_controller;
            extern hba_memory* abar;
            extern ahci_port* ports[];
            extern uint08 usable_ports;
        }
    }
}