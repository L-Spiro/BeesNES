/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 pipeline state.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanPipeline.h"

namespace lsn {

	// == Various constructors.
	CVulkanPipeline::CVulkanPipeline() {
	}
	CVulkanPipeline::~CVulkanPipeline() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanPipeline::Reset() {
		m_pPipeline.Reset();
	}

	/**
	 * Creates a graphics pipeline state object.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _pgpciCreateInfo A pointer to a VkGraphicsPipelineCreateInfo structure that describes the graphics pipeline state.
	 * \return Returns true if the pipeline state was created.
	 **/
	bool CVulkanPipeline::CreateGraphicsPipeline(
		VkDevice _dDevice,
		const VkGraphicsPipelineCreateInfo * _pgpciCreateInfo ) {
		if ( !_dDevice || !_pgpciCreateInfo ) { return false; }
		Reset();
		return m_pPipeline.Create( _dDevice, _pgpciCreateInfo );
	}

	/**
	 * Creates a compute pipeline state object.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _pcpciCreateInfo A pointer to a VkComputePipelineCreateInfo structure that describes the compute pipeline state.
	 * \return Returns true if the compute pipeline state was created.
	 **/
	bool CVulkanPipeline::CreateComputePipeline(
		VkDevice _dDevice,
		const VkComputePipelineCreateInfo * _pcpciCreateInfo ) {
		if ( !_dDevice || !_pcpciCreateInfo ) { return false; }
		Reset();
		return m_pPipeline.Create( _dDevice, _pcpciCreateInfo );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
