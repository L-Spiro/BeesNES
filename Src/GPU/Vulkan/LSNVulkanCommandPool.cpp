/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 command pool.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanCommandPool.h"

namespace lsn {

	// == Various constructors.
	CVulkanCommandPool::CVulkanCommandPool() {
	}
	CVulkanCommandPool::~CVulkanCommandPool() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanCommandPool::Reset() {
		m_cpCommandPool.Reset();
	}

	/**
	 * Creates a command pool object.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _ui32QueueFamilyIndex The queue family index for which the command pool is created.
	 * \param _cpcfFlags Flags indicating the behavior of the command pool (e.g., VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT).
	 * \param _pacAllocator Optional allocator callbacks.
	 * \return Returns true if the command pool was created.
	 **/
	bool CVulkanCommandPool::CreateCommandPool(
		VkDevice _dDevice,
		uint32_t _ui32QueueFamilyIndex,
		VkCommandPoolCreateFlags _cpcfFlags,
		const VkAllocationCallbacks * _pacAllocator ) {
		
		if ( !_dDevice ) { return false; }
		Reset();

		VkCommandPoolCreateInfo cpciCreateInfo = {};
		cpciCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cpciCreateInfo.pNext = nullptr;
		cpciCreateInfo.flags = _cpcfFlags;
		cpciCreateInfo.queueFamilyIndex = _ui32QueueFamilyIndex;

		return m_cpCommandPool.Create( _dDevice, &cpciCreateInfo, _pacAllocator );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
