/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 buffer.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanBuffer.h"

namespace lsn {

	// == Various constructors.
	CVulkanBuffer::CVulkanBuffer() {
	}
	CVulkanBuffer::~CVulkanBuffer() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanBuffer::Reset() {
		m_bBuffer.Reset();
	}

	/**
	 * Creates a buffer object.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _pbciCreateInfo A pointer to a VkBufferCreateInfo structure specifying the parameters for creating the buffer.
	 * \return Returns true if the buffer was created.
	 **/
	bool CVulkanBuffer::CreateBuffer(
		VkDevice _dDevice,
		const VkBufferCreateInfo * _pbciCreateInfo ) {
		if ( !_dDevice || !_pbciCreateInfo ) { return false; }
		Reset();
		return m_bBuffer.Create( _dDevice, _pbciCreateInfo );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
