/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 descriptor pool.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanDescriptorPool.h"

namespace lsn {

	// == Various constructors.
	CVulkanDescriptorPool::CVulkanDescriptorPool() {
	}
	CVulkanDescriptorPool::~CVulkanDescriptorPool() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanDescriptorPool::Reset() {
		m_dpDescriptorPool.Reset();
	}

	/**
	 * Creates a descriptor pool object.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _pdpciCreateInfo A pointer to a VkDescriptorPoolCreateInfo structure specifying the state of the descriptor pool.
	 * \return Returns true if the descriptor pool was created.
	 **/
	bool CVulkanDescriptorPool::CreateDescriptorPool(
		VkDevice _dDevice,
		const VkDescriptorPoolCreateInfo * _pdpciCreateInfo ) {
		if ( !_dDevice || !_pdpciCreateInfo ) { return false; }
		Reset();
		return m_dpDescriptorPool.Create( _dDevice, _pdpciCreateInfo );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
