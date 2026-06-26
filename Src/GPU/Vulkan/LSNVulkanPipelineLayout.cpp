/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 pipeline layout.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanPipelineLayout.h"

namespace lsn {

	// == Various constructors.
	CVulkanPipelineLayout::CVulkanPipelineLayout() {
	}
	CVulkanPipelineLayout::~CVulkanPipelineLayout() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanPipelineLayout::Reset() {
		m_plPipelineLayout.Reset();
	}

	/**
	 * Creates a pipeline layout object.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _pplciCreateInfo A pointer to a VkPipelineLayoutCreateInfo structure specifying the state of the pipeline layout.
	 * \return Returns true if the pipeline layout was created.
	 **/
	bool CVulkanPipelineLayout::CreatePipelineLayout(
		VkDevice _dDevice,
		const VkPipelineLayoutCreateInfo * _pplciCreateInfo ) {
		if ( !_dDevice || !_pplciCreateInfo ) { return false; }
		Reset();
		return m_plPipelineLayout.Create( _dDevice, _pplciCreateInfo );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
