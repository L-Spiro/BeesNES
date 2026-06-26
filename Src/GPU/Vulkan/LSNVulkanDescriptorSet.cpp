/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 descriptor set.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanDescriptorSet.h"

namespace lsn {

	// == Various constructors.
	CVulkanDescriptorSet::CVulkanDescriptorSet() {
	}
	CVulkanDescriptorSet::~CVulkanDescriptorSet() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch, freeing the descriptor set if the pool allows it.
	 **/
	void CVulkanDescriptorSet::Reset() {
		m_dsDescriptorSet.Reset();
	}

	/**
	 * Allocates a single descriptor set from a descriptor pool.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _dpDescriptorPool The descriptor pool to allocate the set from.
	 * \param _dslLayout The descriptor set layout specifying the bindings.
	 * \return Returns true if the descriptor set was successfully allocated.
	 **/
	bool CVulkanDescriptorSet::AllocateDescriptorSet(
		VkDevice _dDevice,
		VkDescriptorPool _dpDescriptorPool,
		VkDescriptorSetLayout _dslLayout ) {
		if ( !_dDevice || !_dpDescriptorPool || !_dslLayout ) { return false; }
		Reset();

		VkDescriptorSetAllocateInfo dsaiAllocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		dsaiAllocInfo.pNext = nullptr;
		dsaiAllocInfo.descriptorPool = _dpDescriptorPool;
		dsaiAllocInfo.descriptorSetCount = 1;
		dsaiAllocInfo.pSetLayouts = &_dslLayout;

		return m_dsDescriptorSet.Create( _dDevice, &dsaiAllocInfo );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
