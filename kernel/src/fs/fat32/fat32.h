#pragma once
#include "types.h"

namespace fs {
    namespace fat32 {
        struct bios_parameter_block {
            uint08  eb3c90[3];
            uint64  oem_id;
            uint16  sector_size;
            uint08  cluster_size;
            uint16  reserved_sector_count;
            uint08  fat_count;
            uint16  directory_entry_count;
            uint16  short_sector_count;
            uint08  media_descriptor_type;
            uint16  sectors_per_fat;
            uint16  sectors_per_track;
            uint16  head_count;
            uint32  hidden_sector_count;
            uint32  large_sector_count;
        } __attribute__((packed));

        struct bios_parameter_block_ex {
            uint32  sectors_per_fat;
            uint16  flags;
            uint16  fat_version;
            uint32  root_directory_cluster;
            uint16  fsinfo_sector;
            uint16  backup_sector;
            uint08  pad[12];
            uint08  drive_no;
            uint08  pad_ex;
            uint08  signature;
            uint32  volume_serial_id;
            char    volume_label[11];
            char    system_id[8];
            uint08  boot_code[420];
            uint16  aa55;
        };

        struct fs_info {
            uint32  lead_signature;
            uint08  pad[480];
            uint32  signature;
            uint32  free_cluster_count;
            uint32  start_cluster;
            uint08  pad_ex[12];
            uint32  trail_signature;
        };

        struct directory_entry {
            char name[8];
            char extension[3];
            uint08 attributes;
            uint08 user_attributes;
            char undelete;
            uint16 creation_time;
            uint16 creation_date;
            uint16 access_date;
            uint16 cluster_high;
            uint16 modification_time;
            uint16 modification_date;
            uint16 cluster_low;
            uint32 file_size;
        } __attribute__((packed));
    }
}