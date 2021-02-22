//
// Created by rizet on 1/26/21.
//
#pragma once
#include "types.hxx"

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
        constexpr byte GDT_MAX_DESCRIPTORS       = 16; 
        constexpr byte GDT_DESC_ACCESS           = 0x01;
        constexpr byte GDT_DESC_READWRITE        = 0x02;
        constexpr byte GDT_DESC_DC               = 0x04;
        constexpr byte GDT_DESC_EXECUTABLE       = 0x08;
        constexpr byte GDT_DESC_CODEDATA         = 0x10;
        constexpr byte GDT_DESC_DPL              = 0x60;
        constexpr byte GDT_DESC_PRESENT          = 0x80;
        constexpr byte GDT_GRAN_LIMITHIMAST      = 0x0F;
        constexpr byte GDT_GRAN_OS               = 0x10;
        constexpr byte GDT_GRAN_64BIT            = 0x20;
        constexpr byte GDT_GRAN_32BIT            = 0x40;
        constexpr byte GDT_GRAN_4K               = 0x80;

        // Predefined selectors
        constexpr byte GDT_SELECTOR_KERNEL_CODE  = (0x01 << 3);
        constexpr byte GDT_SELECTOR_KERNEL_DATA  = (0x02 << 3);
        constexpr byte GDT_SELECTOR_USER_DATA    = (0x03 << 3);
        constexpr byte GDT_SELECTOR_USER_CODE    = (0x04 << 3);

        typedef struct gdt_descriptor {
            word limit;         
            word base_low;      
            byte base_mid;       
            byte flags;          
            byte granularity;    
            byte base_high;      
        } __attribute__((packed)) gdt_desc_t;

        typedef struct gdt_system_descriptor {
            word limit_0;
            word addr_0;
            byte addr_1;
            byte type_0;
            byte limit_1;
            byte addr_2;
            dword addr_3;
            dword reserved;
        } __attribute__((packed)) gdt_system_desc_t;
        
        typedef struct gdt {
            word limit;         
            address base;       
        } __attribute__((packed)) gdt_t;

        extern "C" void gdt_init();

        void gdt_install_descriptor(address base, address limit, byte access, byte granularity);

        void gdt_install_tss(address base, address limit);

        extern "C" void load_gdt(cpu::gdt::gdt_t *);
    }
}