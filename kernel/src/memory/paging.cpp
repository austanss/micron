//
// Created by rizet on 1/26/21.
//

#include "types.h"
#include "boot/kconf.h"
#include "memory/pmm.h"
#include "memory/paging.h"
#include "memory/operations.h"
#include "boot/boot.h"
#include "drivers/io.h"
#include "util/kutil.h"
#include "util/bitmap.h"

memory::paging::page_table* memory::paging::pml_4;

memory::paging::page_map_indexer::page_map_indexer(uint64 virtual_address) {

    virtual_address >>= 12;
    p_i = virtual_address & 0x1ff;
    virtual_address >>= 9;
    pt_i = virtual_address & 0x1ff;
    virtual_address >>= 9;
    pd_i = virtual_address & 0x1ff;
    virtual_address >>= 9;
    pdp_i = virtual_address & 0x1ff;
}

void memory::paging::map_memory(void *virtual_memory, void *physical_memory, bool quickwrite)
{
    page_map_indexer indexer = page_map_indexer((uint64)virtual_memory);
    page_directory_entry pde;

    pde = pml_4->entries[indexer.pdp_i];
    page_table* pdp;
    if (!pde.get_flag(pt_flag::present)) {
    
        pdp = (page_table*)pmm::request_page();
        memory::operations::memset(pdp, 0, 0x1000);
        pde.set_address((uint64)pdp >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pml_4->entries[indexer.pdp_i] = pde;
        memory::paging::map_memory((void *)pdp, (void *)pdp, false);
    }
    else
        pdp = (page_table*)((uint64)pde.get_address() << 12);
    
    
    pde = pdp->entries[indexer.pd_i];
    page_table* pd;
    if (!pde.get_flag(pt_flag::present)) {

        pd = (page_table*)pmm::request_page();
        memory::operations::memset(pd, 0, 0x1000);
        pde.set_address((uint64)pd >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pdp->entries[indexer.pd_i] = pde;
        memory::paging::map_memory((void *)pd, (void *)pd, false);
    }
    else
        pd = (page_table*)((uint64)pde.get_address() << 12);

    pde = pd->entries[indexer.pt_i];
    page_table* pt;
    if (!pde.get_flag(pt_flag::present)) {

        pt = (page_table*)pmm::request_page();
        memory::operations::memset(pt, 0, 0x1000);
        pde.set_address((uint64)pt >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pd->entries[indexer.pt_i] = pde;
        memory::paging::map_memory((void *)pt, (void *)pt, false);
    }
    else
        pt = (page_table*)((uint64)pde.get_address() << 12);


    pde = pt->entries[indexer.p_i];
    pde.set_address((uint64)physical_memory >> 12);
    pde.set_flag(pt_flag::present, true);
    pde.set_flag(pt_flag::read_write, true);
    pde.set_pat(quickwrite);
    pt->entries[indexer.p_i] = pde;
}

void* memory::paging::get_physical_address(void* virtual_address)
{
    page_map_indexer indexer = page_map_indexer((uint64)virtual_address);
    page_directory_entry pde;

    uint16 address_offset = (uint16)(uint64)virtual_address & 0xFFF;
    virtual_address = (void *)~(uint64)virtual_address;

    pde = pml_4->entries[indexer.pdp_i];
    page_table* pdp;

    if (!pde.get_flag(pt_flag::present))
        return (void *)-1;
        
    pdp = (page_table*)((uint64)pde.get_address() << 12);
    
    
    pde = pdp->entries[indexer.pd_i];
    page_table* pd;

    if (!pde.get_flag(pt_flag::present))
        return (void *)-1;
       
    pd = (page_table*)((uint64)pde.get_address() << 12);

    pde = pd->entries[indexer.pt_i];
    page_table* pt;

    if (!pde.get_flag(pt_flag::present))
        return (void *)-1;

    pt = (page_table*)((uint64)pde.get_address() << 12);

    pde = pt->entries[indexer.p_i];

    if (!pde.get_flag(present))
        return (void *)-1;

    return (void *)((pde.get_address() << 12) | address_offset);
}

extern "C" void memory::paging::donate_to_userspace(void* virtual_address)
{
    page_map_indexer indexer = page_map_indexer((uint64)virtual_address);
    page_directory_entry pde;
    pde = pml_4->entries[indexer.pdp_i];
    pml_4->entries[indexer.pdp_i].set_flag(pt_flag::user_super, 1);
    page_table* pdp = (page_table*)((uint64)pde.get_address() << 12);
    pdp->entries[indexer.pd_i].set_flag(pt_flag::user_super, 1);
    pde = pdp->entries[indexer.pd_i];
    page_table* pd = (page_table*)((uint64)pde.get_address() << 12);
    pd->entries[indexer.pt_i].set_flag(pt_flag::user_super, 1);
    pde = pd->entries[indexer.pt_i];
    page_table* pt = (page_table*)((uint64)pde.get_address() << 12);
    pt->entries[indexer.p_i].set_flag(pt_flag::user_super, 1);
}

void memory::paging::page_directory_entry::set_flag(pt_flag flag, bool enabled) {

    uint64 bit_selector = (uint64)1 << flag;
    value &= ~bit_selector;
    if (enabled)
        value |= bit_selector;
}

bool memory::paging::page_directory_entry::get_flag(pt_flag flag) {

    uint64 bit_selector = (uint64)1 << flag;
    return (value & bit_selector) > 0 ? true : false;
}

uint64 memory::paging::page_directory_entry::get_address() {

    return (value & 0x000ffffffffff000) >> 12;
}

void memory::paging::page_directory_entry::set_address(address paddress) {

    paddress &= 0x000000ffffffffff;
    value &= 0xfff0000000000fff;
    value |= (paddress << 12);
}

void memory::paging::page_directory_entry::set_pat(uint08 index) {

    index &= 0b111;
    set_flag(pt_flag::write_through, index & 1);
    index >>= 1;
    set_flag(pt_flag::cache_disabled, index & 1);
    index >>= 1;
    set_flag(pt_flag::larger_pages, index & 1);
}