#pragma once
#include "types.hxx"

namespace sys {
    namespace acpi {
        struct sdt_header {
            uint08 signature[4];
            uint32 length;
            uint08 revision;
            uint08 checksum;
            uint08 vendor_id[6];
            uint08 vendor_table_id[8];
            uint32 vendor_revision;
            uint32 creator_id;
            uint32 creator_revision;
        } __attribute__((packed));

        struct rsdp2 {
            uint08 signature[8];
            uint08 checksum;
            uint08 vendor_id[6];
            uint08 revision;
            uint32 rsdt_address;
            uint32 length;
            sdt_header* xsdt;
            uint08 echecksum;
            uint08 pad[3];
        } __attribute__((packed));

        struct mcfg_header {
            sdt_header header;
            uint08 pad[8];
        } __attribute__((packed));

        extern rsdp2* rsdp;
    
        sys::acpi::sdt_header* get_table(char* signature);
    }
}