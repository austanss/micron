#pragma once
#include "types.h"

namespace sys {
    namespace prog {

        struct elf_common_header {
            char        signature[4];
            uint08      bitness;
            uint08      endianness;
            uint08      header_version;
            uint08      abi;
            uint64      pad;
            uint16      resc_flags;
            uint16      isa;
            uint32      elf_version;
        } __attribute__((packed));

        struct elf64_program_header;

        struct elf64_header {
            elf_common_header   common;
            address             entry;
            uint64              pheader_table;
            uint64              sheader_table;
            uint32              spec_flags;
            uint16              header_size;
            uint16              pheader_table_entry_size;
            uint16              pheader_table_entries;
            uint16              sheader_table_entry_size;
            uint16              sheader_table_entries;
            uint16              sheader_table_names_index;
        } __attribute__((packed));

        struct elf32_header {
            elf_common_header   common;
            uint32              entry;
            uint32              pheader_table;
            uint32              sheader_table;
            uint32              spec_flags;
            uint16              header_size;
            uint16              pheader_table_entry_size;
            uint16              pheader_table_entries;
            uint16              sheader_table_entry_size;
            uint16              sheader_table_entries;
            uint16              sheader_table_names_index;
        } __attribute__((packed));

        enum instruction_set {
            agnostic    = 0x0,
            sparc       = 0x2,
            x86         = 0x3,
            mips        = 0x8,
            ppc         = 0x14,
            arm         = 0x28,
            superh      = 0x2A,
            ia64        = 0x32,
            x86_64      = 0x3E,
            aarch64     = 0xB7
        };

        struct elf64_program_header {
            uint32      segment_type;
            uint32      flags;
            uint64      offset;
            uint64      vaddr;
            uint64      undefined;
            uint64      file_size;
            uint64      memory_size;
            uint64      alignment;
        } __attribute__((packed));

        struct elf32_program_header {
            uint32      segment_type;
            uint32      offset;
            uint32      vaddr;
            uint32      undefined;
            uint32      file_size;
            uint32      memory_size;
            uint32      flags;
            uint32      alignment;
        } __attribute__((packed));

        enum segment_type {
            ignore  = 0,
            load    = 1,
            dynlink = 2,
            interp  = 3,
            note    = 4
        };

        // Returns the entry point address
        address load_elf(elf_common_header* elf);
    }
}