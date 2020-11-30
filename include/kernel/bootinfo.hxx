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

typedef struct {
	uint16_t     Year;       // 1998 - 20XX
	uint8_t      Month;      // 1 - 12
	uint8_t      Day;        // 1 - 31
	uint8_t      Hour;       // 0 - 23
	uint8_t      Minute;     // 0 - 59
	uint8_t      Second;     // 0 - 59
	uint8_t      Pad1;
	uint32_t     Nanosecond; // 0 - 999,999,999
	int16_t      TimeZone;   // -1440 to 1440 or 2047
	uint8_t      Daylight;
	uint8_t      Pad2;
} Efi_Time;

typedef struct {
	uint32_t                     Resolution;     // 1e-6 parts per million
	uint32_t                     Accuracy;       // hertz
	int        		             SetsToZero;     // Set clears sub-second time
} Efi_Time_Capabillities;

#define EfiApi __attribute__((ms_abi))

typedef struct s_memory_region_desc {
	uint32_t type;
	uintptr_t physical_start;
	uintptr_t virtual_start;
	uint64_t count;
	uint64_t attributes;
} Memory_Map_Descriptor;

typedef struct {
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t  Data4[8];
} Efi_Guid;

typedef struct {
	Efi_Guid                   	 CapsuleGuid;
	uint32_t                     HeaderSize;
	uint32_t                     Flags;
	uint32_t                     CapsuleImageSize;
} Efi_Capsule_Header;

typedef enum {
	EfiResetCold,
	EfiResetWarm,
	EfiResetShutdown
} Efi_Reset_Type;

typedef
uint64_t
(EfiApi *Efi_Get_Time) (
Efi_Time                    *Time,
Efi_Time_Capabillities      *Capabilities
);

typedef
uint64_t
(EfiApi *Efi_Set_Time) (
Efi_Time                    *Time);

typedef
uint64_t
(EfiApi *Efi_Get_Wakeup_Time) (
int			                    *Enabled,
int 		                    *Pending,
Efi_Time						*Time
);

typedef
uint64_t
(EfiApi *Efi_Set_Wakeup_Time) (
int 		                     Enable,
Efi_Time	              		*Time
);

typedef
uint64_t
(EfiApi *Efi_Set_Virtual_Address_Map) (
uint64_t         				MemoryMapSize,
uint64_t 						DescriptorSize,
uint64_t 						DescriptorVersion,
Memory_Map_Descriptor	       *VirtualMap
);

typedef
uint64_t
(EfiApi *Efi_Convert_Pointer) (
uint64_t                       DebugDisposition,
void	                     **Address
);

typedef
uint64_t
(EfiApi *Efi_Get_Variable) (
uint16_t                      *VariableName,
Efi_Guid	                  *VendorGuid,
uint32_t	                  *Attribute,
uint64_t					  *DataSize,
void 						  *Data
);

typedef
uint64_t
(EfiApi *Efi_Get_Next_Variable_Name) (
uint64_t		                *VariableNameSize,
uint16_t 						*VariableName,
Efi_Guid						*VendorGuid
);

typedef
uint64_t
(EfiApi *Efi_Set_Variable) (
uint16_t						*VariableName,
Efi_Guid 						*VendorGuid,
uint32_t 						Attributes,
uint64_t 						DataSize,
void 							*Data
);

typedef
uint64_t
(EfiApi *Efi_Get_Next_High_Mono_Count) (
uint32_t						*HighCount
);

typedef
uint64_t
(EfiApi *Efi_Reset_System) (
Efi_Reset_Type  	        	ResetType,
uint64_t		              	ResetStatus,
uint64_t						DataSize,
uint16_t                   		*ResetData
);

typedef
uint64_t
(EfiApi *Efi_Update_Capsule) (
Efi_Capsule_Header		      **CapsuleHeaderArray,
uint64_t						CapsuleCount,
int 							ScatterGatherList
);

typedef
uint64_t
(EfiApi *Efi_Query_Capsule_Capabilities) (
Efi_Capsule_Header		      **CapsuleHeaderArray,
uint64_t						CapsuleCount,
uint64_t 						*MaximumCapsuleSize,
Efi_Reset_Type 					*ResetType
);

typedef
uint64_t
(EfiApi *Efi_Query_Variable_Info) (
uint32_t						Attributes,
uint64_t 						*MaximumVariableStorageSize,
uint64_t 						*RemainingVariableStorageSize,
uint64_t 						*MaximumVariableSize
);

typedef struct s_efi_table_header {
	uint64_t                     signature;
	uint32_t                     rev;
	uint32_t                     size;
	uint32_t                     crc;
	uint32_t                     reserved;
} Efi_Table_Header;

typedef struct s_efi_runtime_service_handle {
	Efi_Table_Header header;
	Efi_Get_Time                    GetTime;
	Efi_Set_Time                    SetTime;
	Efi_Get_Wakeup_Time             GetWakeupTime;
	Efi_Set_Wakeup_Time             SetWakeupTime;
	Efi_Set_Virtual_Address_Map     SetVirtualAddressMap;
	Efi_Convert_Pointer             ConvertPointer;
	Efi_Get_Variable                GetVariable;
	Efi_Get_Next_Variable_Name      GetNextVariableName;
	Efi_Set_Variable                SetVariable;
	Efi_Get_Next_High_Mono_Count    GetNextHighMonotonicCount;
	Efi_Reset_System                ResetSystem;
	Efi_Update_Capsule              UpdateCapsule;
	Efi_Query_Capsule_Capabilities  QueryCapsuleCapabilities;
	Efi_Query_Variable_Info         QueryVariableInfo;
} Efi_Runtime_Services;

extern Efi_Runtime_Services *UEFI;

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
	Framebuffer *vbe_framebuffer;
	Memory_Map_Descriptor *memory_map;
	uint64_t mmap_size;
	uint64_t mmap_descriptor_size;
	Efi_Runtime_Services *runtime_services;
} Boot_Info;
};

#endif

