#pragma once
#include "types.h"
#include "util/guid.h"

namespace sys {
    namespace fs {
        namespace gpt {
            struct parition_table_header {
                uint08  signature[8];
                uint32  revision;
                uint32  size;
                uint32  header_checksum;
                uint32  pad;
                uint64  this_lba;
                uint64  alt_lba;
                uint64  first_usable_block;
                uint64  last_usable_block;
                guid    disk_guid;
                uint64  partition_table_lba;
                uint32  partition_table_entries;
                uint32  partition_table_entry_size;
                uint32  partition_table_checksum;
                uint08  pad_ex[0x200 - 0x5C];
            } __attribute__((packed));

            struct partition_table_entry {
                guid    partition_type_guid;
                guid    partition_guid;
                uint64  start_lba;
                uint64  end_lba;
                uint64  attributes;
                // UNICODE16-LE encoded
                uint16  signature[36];
            } __attribute__((packed));
        }
    }
}