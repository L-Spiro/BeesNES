/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 device memory allocation.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanDeviceMemory.h"

namespace lsn {

	// == Various constructors.
	CVulkanDeviceMemory::CVulkanDeviceMemory() {
	}
	CVulkanDeviceMemory::~CVulkanDeviceMemory() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanDeviceMemory::Reset() {
		m_dmDeviceMemory.Reset();
	}

	/**
	 * Allocates device memory.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _pmaiAllocateInfo A pointer to a VkMemoryAllocateInfo structure describing parameters of the allocation.
	 * \return Returns true if the device memory was allocated.
	 **/
	bool CVulkanDeviceMemory::AllocateMemory(
		VkDevice _dDevice,
		const VkMemoryAllocateInfo * _pmaiAllocateInfo ) {
		if ( !_dDevice || !_pmaiAllocateInfo ) { return false; }
		Reset();
		return m_dmDeviceMemory.Create( _dDevice, _pmaiAllocateInfo );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
