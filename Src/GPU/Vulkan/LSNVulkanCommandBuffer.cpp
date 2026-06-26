/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 command buffer.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanCommandBuffer.h"

namespace lsn {

	// == Various constructors.
	CVulkanCommandBuffer::CVulkanCommandBuffer() {
	}
	CVulkanCommandBuffer::~CVulkanCommandBuffer() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch, freeing the command buffer.
	 **/
	void CVulkanCommandBuffer::Reset() {
		m_cbCommandBuffer.Reset();
	}

	/**
	 * Creates a command buffer.
	 *
	 * \param _dDevice The Vulkan logical device.
	 * \param _cpCommandPool The command pool from which to allocate the command buffer.
	 * \param _cblLevel Specifies whether the command buffer is a primary or secondary command buffer.
	 * \return Returns true if the command buffer was successfully allocated.
	 **/
	bool CVulkanCommandBuffer::CreateCommandBuffer(
		VkDevice _dDevice,
		VkCommandPool _cpCommandPool,
		VkCommandBufferLevel _cblLevel ) {
		
		if ( !_dDevice || !_cpCommandPool ) { return false; }
		Reset();

		VkCommandBufferAllocateInfo cbaiAllocInfo = {};
		cbaiAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbaiAllocInfo.pNext = nullptr;
		cbaiAllocInfo.commandPool = _cpCommandPool;
		cbaiAllocInfo.level = _cblLevel;
		cbaiAllocInfo.commandBufferCount = 1;

		return m_cbCommandBuffer.Create( _dDevice, &cbaiAllocInfo );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
