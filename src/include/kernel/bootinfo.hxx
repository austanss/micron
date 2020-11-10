/**
 * @file boot.h
 * @author ajxs
 * @date Aug 2019
 * @brief Boot functionality.
 * Contains definitions for boot structures.
 */

#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

extern "C"
{
/**
 * @brief Memory region descriptor.
 * Describes a region of memory. This is passed to the kernel by the bootloader.
 */
typedef struct s_memory_region_desc {
	uint32_t type;
	uintptr_t physical_start;
	uintptr_t virtual_start;
	uint64_t count;
	uint64_t attributes;
} Memory_Map_Descriptor;

typedef struct s_framebuffer {
	uint64_t framebuffer_base_addr;
	uint64_t framebuffer_size;
	uint32_t framebuffer_mode;
	uint32_t x_resolution;
	uint32_t y_resolution;
	uint32_t pixels_per_scan_line;
} Framebuffer;

/**
 * @brief Boot info struct.
 * Contains information passed to the kernel at boot time by the bootloader.
 */
typedef struct s_boot_info {
	uint64_t verification;
	Memory_Map_Descriptor *memory_map;
	Framebuffer vbe_framebuffer;
	uint64_t mmap_size;
	uint64_t mmap_descriptor_size;
} Boot_Info;
};

#endif

