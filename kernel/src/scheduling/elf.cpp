#include "elf.h"
#include "drivers/tty/tty.h"
#include "memory/pmm.h"
#include "memory/paging.h"
#include "memory/operations.h"

char* $bitness_string(uint bitness)
{
    if (bitness == 1)
        return (char *)"32-bit";
    else if (bitness == 2)
        return (char *)"64-bit";
    else
        return (char *)"bad bitness";
}

char* $endianness_string(uint endianness)
{
    if (endianness == 1)
        return (char *)"little-endian";
    else if (endianness == 2)
        return (char *)"big-endian";
    else
        return (char *)"bad endianess";
}

void $load_program_segment(sys::prog::elf_common_header* elf, sys::prog::elf64_program_header segment) 
{
    for (uint i = 0; i < segment.memory_size / 0x1000 + 1; i++)
    {
        memory::paging::map_memory((void *)(segment.vaddr + i * 0x1000), memory::pmm::request_page(), false);
        memory::paging::donate_to_userspace((void *)(segment.vaddr + i * 0x1000));
    }
    
    memory::operations::memset((void *)segment.vaddr, 0x00, segment.memory_size);

    memory::operations::memcpy((void *)segment.vaddr, (void *)((uint64)elf + segment.offset), segment.file_size);
}

address sys::prog::load_elf(elf_common_header* elf) 
{
    if (elf->bitness != 2) return (address)nullptr;

    elf64_header* elfx = (elf64_header *)elf;

    elf64_program_header* elfpht = (elf64_program_header *)((uint64)elfx + elfx->pheader_table);

    for (uint i = 0; i < elfx->pheader_table_entries; i++) 
    {
        if (!elfpht[i].segment_type) continue;

        if (elfpht[i].segment_type == 1)
            $load_program_segment(elf, elfpht[i]);
    }

    return elfx->entry;
}
