/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 semaphore.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanSemaphore.h"

namespace lsn {

	// == Various constructors.
	CVulkanSemaphore::CVulkanSemaphore() {
	}
	CVulkanSemaphore::~CVulkanSemaphore() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanSemaphore::Reset() {
		m_sSemaphore.Reset();
	}

	/**
	 * Creates a semaphore object.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \return Returns true if the semaphore was created.
	 **/
	bool CVulkanSemaphore::CreateSemaphore(
		VkDevice _dDevice ) {
		if ( !_dDevice ) { return false; }
		Reset();

		VkSemaphoreCreateInfo sciCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		sciCreateInfo.pNext = nullptr;
		sciCreateInfo.flags = 0;

		return m_sSemaphore.Create( _dDevice, &sciCreateInfo );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
