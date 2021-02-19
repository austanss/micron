//
// Created by rizet on 1/26/21.
//
#pragma once
#include <stdint.h>

/*
 * Credit: Jim Borden (borrrden) on GitHub
 * https://github.com/borrrden/borrrdex
 * There was no legal need for this source disclosure.
 * The code is licensed under the Unlicense.
 * I felt it was necessary to give credit.
*/
namespace cpu
{
    namespace gdt 
    {
        constexpr uint8_t GDT_MAX_DESCRIPTORS       = 16; 
        constexpr uint8_t GDT_DESC_ACCESS           = 0x01;
        constexpr uint8_t GDT_DESC_READWRITE        = 0x02;
        constexpr uint8_t GDT_DESC_DC               = 0x04;
        constexpr uint8_t GDT_DESC_EXECUTABLE       = 0x08;
        constexpr uint8_t GDT_DESC_CODEDATA         = 0x10;
        constexpr uint8_t GDT_DESC_DPL              = 0x60;
        constexpr uint8_t GDT_DESC_PRESENT          = 0x80;
        constexpr uint8_t GDT_GRAN_LIMITHIMAST      = 0x0F;
        constexpr uint8_t GDT_GRAN_OS               = 0x10;
        constexpr uint8_t GDT_GRAN_64BIT            = 0x20;
        constexpr uint8_t GDT_GRAN_32BIT            = 0x40;
        constexpr uint8_t GDT_GRAN_4K               = 0x80;

        // Predefined selectors
        constexpr uint8_t GDT_SELECTOR_KERNEL_CODE  = (0x01 << 3);
        constexpr uint8_t GDT_SELECTOR_KERNEL_DATA  = (0x02 << 3);
        constexpr uint8_t GDT_SELECTOR_USER_CODE    = (0x05 << 3);
        constexpr uint8_t GDT_SELECTOR_USER_DATA    = (0x04 << 3);


        typedef struct gdt_descriptor {
            uint16_t limit;         
            uint16_t base_low;      
            uint8_t base_mid;       
            uint8_t flags;          
            uint8_t granularity;    
            uint8_t base_high;      
        } __attribute__((packed)) gdt_desc_t;

        typedef struct gdt_system_descriptor {
            uint16_t limit_0;
            uint16_t addr_0;
            uint8_t addr_1;
            uint8_t type_0;
            uint8_t limit_1;
            uint8_t addr_2;
            uint32_t addr_3;
            uint32_t reserved;
        } __attribute__((packed)) gdt_system_desc_t;

        // Basically, this structure is an array of the above,
        // defined as a block of memory
        typedef struct gdt {
            uint16_t limit;         ///< The size in memory of all the gdt_desc_t entries (laid out sequentially)
            uint64_t base;          ///< The memory address of the first gdt_desc_t entry 
        } __attribute__((packed)) gdt_t;

        /**
        * Initializes the GDT.  Don't call more than once.  As a side effect,
        * the code segment is set to GDT_SELECTOR_KERNEL_CODE
        */
        extern "C" void gdt_init();

        /**
        * Writes a new entry into the gdt_t structure.
        * @param base          [Unused for 64-bit] The descriptor memory address
        * @param limit         [Unused for 64-bit] The descriptor size
        * @param access        The access flags (see GDT_DESC_*)
        * @param granularity   The remaining flags (see GDT_GRAN_*)
        */
        void gdt_install_descriptor(uint64_t base, uint64_t limit, uint8_t access, uint8_t granularity);

        void gdt_install_tss(uint64_t base, uint64_t limit);

        extern "C" void load_gdt(cpu::gdt::gdt_t *);
    }
}