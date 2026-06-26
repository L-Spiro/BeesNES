/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 fence.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanFence.h"

namespace lsn {

	// == Various constructors.
	CVulkanFence::CVulkanFence() {
	}
	CVulkanFence::~CVulkanFence() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanFence::Reset() {
		m_fFence.Reset();
	}

	/**
	 * Creates a fence object.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _bSignaled If true, creates the fence in the signaled state.
	 * \return Returns true if the fence was created.
	 **/
	bool CVulkanFence::CreateFence(
		VkDevice _dDevice,
		bool _bSignaled ) {
		if ( !_dDevice ) { return false; }
		Reset();

		VkFenceCreateInfo fciCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fciCreateInfo.flags = _bSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

		return m_fFence.Create( _dDevice, &fciCreateInfo );
	}

	/**
	 * Blocks the CPU until the fence is signaled by the GPU.
	 *
	 * \param _ui64Timeout The timeout value in nanoseconds.
	 * \return Returns true if the wait was successful.
	 **/
	bool CVulkanFence::Wait( uint64_t _ui64Timeout ) const {
		return m_fFence.Wait( _ui64Timeout ) == VK_SUCCESS;
	}

	/**
	 * Resets the fence to the unsignaled state.
	 *
	 * \return Returns true if the fence was successfully reset.
	 **/
	bool CVulkanFence::ResetFence() const {
		return m_fFence.ResetFence() == VK_SUCCESS;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
