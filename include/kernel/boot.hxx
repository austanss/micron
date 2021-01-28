/**
 * @file boot.h
 * @author ajxs
 * @date Aug 2019
 * @brief Boot functionality.
 * Contains definitions for boot structures.
 */

#pragma once

#include <stdint.h>

extern "C"
{
/**
 * @brief Memory region descriptor.
 * Describes a region of memory. This is passed to the kernel by the bootloader.
 */
namespace boot {

	typedef struct {
		uint16_t     				year;       // 1998 - 20XX
		uint8_t      				month;      // 1 - 12
		uint8_t      				day;        // 1 - 31
		uint8_t      				hour;       // 0 - 23
		uint8_t      				ninute;     // 0 - 59
		uint8_t      				second;     // 0 - 59
		uint8_t      				pad1;
		uint32_t     				nanosecond; // 0 - 999,999,999
		int16_t      				timezone;   // -1440 to 1440 or 2047
		uint8_t      				daylight;
		uint8_t      				pad2;
	} efi_time;

	typedef struct {
		uint32_t                   	resolution;     	// 1e-6 parts per million
		uint32_t                   	accuracy;      		// hertz
		int        		           	sets_to_zero;     	// Set clears sub-second time
	} efi_time_capabillities;

	#define efi_api __attribute__((ms_abi))

	typedef struct s_memory_region_desc {
		uint32_t type;
		uintptr_t physical_start;
		uintptr_t virtual_start;
		uint64_t count;
		uint64_t attributes;
	} memory_map_descriptor;

	typedef struct {
		uint32_t data1;
		uint16_t data2;
		uint16_t data3;
		uint8_t  data4[8];
	} efi_guid;

	typedef struct {
		efi_guid                   	capsule_guid;
		uint32_t                 	header_size;
		uint32_t              		flags;
		uint32_t                	capsule_image_size;
	} efi_capsule_header;

	typedef enum {
		efi_reset_cold,
		efi_reset_warm,
		efi_reset_shutdown
	} efi_reset_type;

	typedef
	uint64_t
	(efi_api *efi_get_time) (
	efi_time                    	*time,
	efi_time_capabillities      	*capabilities
	);

	typedef
	uint64_t
	(efi_api *efi_set_time) (
	efi_time                    	*time);

	typedef
	uint64_t
	(efi_api *efi_get_wakeup_time) (
	int			                    *enabled,
	int 		                    *pending,
	efi_time						*time
	);

	typedef
	uint64_t
	(efi_api *efi_set_wakeup_time) (
	int 		                    enable,
	efi_time	              		*time
	);

	typedef
	uint64_t
	(efi_api *efi_set_virtual_address_map) (
	uint64_t         				memory_map_size,
	uint64_t 						descriptor_size,
	uint64_t 					    descriptor_version,
	memory_map_descriptor	        *virtual_map
	);

	typedef
	uint64_t
	(efi_api *efi_convert_pointer) (
	uint64_t                        debug_disposition,
	void	                        **address
	);

	typedef
	uint64_t
	(efi_api *efi_get_variable) (
	uint16_t                        *variable_name,
	efi_guid	                    *vendor_guid,
	uint32_t	                    *attribute,
	uint64_t					    *data_size,
	void 						    *data
	);

	typedef
	uint64_t
	(efi_api *efi_get_next_variable_name) (
	uint64_t		                *variable_name_size,
	uint16_t 						*variable_name,
	efi_guid						*vendor_guid
	);

	typedef
	uint64_t
	(efi_api *efi_set_variable) (
	uint16_t						*variable_name,
	efi_guid 						*vendor_guid,
	uint32_t 						attributes,
	uint64_t 						data_size,
	void 							*data
	);

	typedef
	uint64_t
	(efi_api *efi_get_next_high_mono_count) (
	uint32_t						*high_count
	);

	typedef
	uint64_t
	(efi_api *efi_reset_system) (
	efi_reset_type  	        	reset_type,
	uint64_t		              	reset_status,
	uint64_t						data_size,
	uint16_t                   		*reset_data
	);

	typedef
	uint64_t
	(efi_api *efi_update_capsule) (
	efi_capsule_header		        **capsule_header_array,
	uint64_t						capsule_count,
	int 							scatter_gather_list
	);

	typedef
	uint64_t
	(efi_api *efi_query_capsule_capabilities) (
	efi_capsule_header		        **capsule_header_array,
	uint64_t						capsule_count,
	uint64_t 					    *maximum_capsule_size,
	efi_reset_type 					*reset_type
	);

	typedef
	uint64_t
	(efi_api *efi_query_variable_info) (
	uint32_t						attributes,
	uint64_t 						*maximum_variable_storage_size,
	uint64_t 						*remaining_variable_storage_size,
	uint64_t 						*maximum_variable_size
	);

	typedef struct s_efi_table_header {
	uint64_t             	        signature;
	uint32_t                	    rev;
	uint32_t                    	size;
	uint32_t                     	crc;
	uint32_t                     	reserved;
	} efi_table_header;

	typedef struct s_efi_runtime_service_handle {
	efi_table_header 				header;
	efi_get_time                    get_time;
	efi_set_time                    set_time;
	efi_get_wakeup_time             get_wakeup_time;
	efi_set_wakeup_time             set_wakeup_time;
	efi_set_virtual_address_map     set_virtual_address_map;
	efi_convert_pointer             convert_pointer;
	efi_get_variable                get_variable;
	efi_get_next_variable_name      get_next_variable_name;
	efi_set_variable                set_variable;
	efi_get_next_high_mono_count    get_next_high_monotonic_count;
	efi_reset_system                reset_system;
	efi_update_capsule              update_capsule;
	efi_query_capsule_capabilities  query_capsule_capabilities;
	efi_query_variable_info         query_variable_info;
	} efi_runtime_services;

	extern efi_runtime_services *uefi;

	typedef struct s_framebuffer {
	uint32_t 						*framebuffer_base;
	uint64_t 						framebuffer_size;
	uint32_t 						framebuffer_mode;
	uint32_t 						x_resolution;
	uint32_t 						y_resolution;
	uint32_t 						pixels_per_scan_line;
	} gop_framebuffer;

	/**
	* @brief Boot info struct.
	* Contains information passed to the kernel at boot time by the bootloader.
	*/
	typedef struct s_boot_info {
	uint64_t 						verification;
	gop_framebuffer 				*vbe_framebuffer;
	memory_map_descriptor 			*memory_map;
	uint64_t 						mmap_size;
	uint64_t 						mmap_descriptor_size;
	efi_runtime_services 			*runtime_services;
	} boot_info;
}
}