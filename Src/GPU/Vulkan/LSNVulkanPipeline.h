/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 pipeline state.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanPipeline
	 * \brief A Vulkan 1.0 pipeline state.
	 *
	 * Description: A Vulkan 1.0 pipeline state.
	 */
	class CVulkanPipeline {
	public :
		// == Various constructors.
		CVulkanPipeline();
		~CVulkanPipeline();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a graphics pipeline state object.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _pgpciCreateInfo A pointer to a VkGraphicsPipelineCreateInfo structure that describes the graphics pipeline state.
		 * \return Returns true if the pipeline state was created.
		 **/
		bool													CreateGraphicsPipeline(
			VkDevice _dDevice,
			const VkGraphicsPipelineCreateInfo * _pgpciCreateInfo );

		/**
		 * Creates a compute pipeline state object.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _pcpciCreateInfo A pointer to a VkComputePipelineCreateInfo structure that describes the compute pipeline state.
		 * \return Returns true if the compute pipeline state was created.
		 **/
		bool													CreateComputePipeline(
			VkDevice _dDevice,
			const VkComputePipelineCreateInfo * _pcpciCreateInfo );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan pipeline handle.
		 **/
		inline VkPipeline										Get() const { return m_pPipeline.pPipeline; }


	protected :
		// == Members.
		/** The wrapped RAII pipeline object. */
		CVulkan::LSN_PIPELINE									m_pPipeline;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
