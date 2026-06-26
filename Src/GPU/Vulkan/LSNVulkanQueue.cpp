/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 queue.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanQueue.h"

namespace lsn {

	// == Various constructors.
	CVulkanQueue::CVulkanQueue() {
	}
	CVulkanQueue::~CVulkanQueue() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanQueue::Reset() {
		// Vulkan queues are destroyed implicitly when their owning VkDevice is destroyed.
		// There is no vkDestroyQueue.
		m_qQueue = VK_NULL_HANDLE;
	}

	/**
	 * Retrieves a queue handle from a logical device.
	 * 
	 * \param _dDevice The Vulkan logical device that owns the queue.
	 * \param _ui32QueueFamilyIndex The index of the queue family to which the queue belongs.
	 * \param _ui32QueueIndex The index within this queue family of the queue to retrieve.
	 * \return Returns true if the queue handle was successfully retrieved.
	 **/
	bool CVulkanQueue::GetDeviceQueue(
		VkDevice _dDevice,
		uint32_t _ui32QueueFamilyIndex,
		uint32_t _ui32QueueIndex ) {
		
		if ( !_dDevice ) { return false; }
		Reset();
		
		CVulkan::m_pfGetDeviceQueue( _dDevice, _ui32QueueFamilyIndex, _ui32QueueIndex, &m_qQueue );
		
		return m_qQueue != VK_NULL_HANDLE;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
