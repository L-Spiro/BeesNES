/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 descriptor set layout.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanDescriptorSetLayout.h"

namespace lsn {

	// == Various constructors.
	CVulkanDescriptorSetLayout::CVulkanDescriptorSetLayout() {
	}
	CVulkanDescriptorSetLayout::~CVulkanDescriptorSetLayout() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanDescriptorSetLayout::Reset() {
		m_dslDescriptorSetLayout.Reset();
	}

	/**
	 * Creates a descriptor set layout object.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _pdslciCreateInfo A pointer to a VkDescriptorSetLayoutCreateInfo structure specifying the state of the descriptor set layout.
	 * \return Returns true if the descriptor set layout was created.
	 **/
	bool CVulkanDescriptorSetLayout::CreateDescriptorSetLayout(
		VkDevice _dDevice,
		const VkDescriptorSetLayoutCreateInfo * _pdslciCreateInfo ) {
		if ( !_dDevice || !_pdslciCreateInfo ) { return false; }
		Reset();
		return m_dslDescriptorSetLayout.Create( _dDevice, _pdslciCreateInfo );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
