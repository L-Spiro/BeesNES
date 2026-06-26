/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 pipeline layout.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanPipelineLayout
	 * \brief A Vulkan 1.0 pipeline layout.
	 *
	 * Description: A Vulkan 1.0 pipeline layout.
	 */
	class CVulkanPipelineLayout {
	public :
		// == Various constructors.
		CVulkanPipelineLayout();
		~CVulkanPipelineLayout();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a pipeline layout object.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _pplciCreateInfo A pointer to a VkPipelineLayoutCreateInfo structure specifying the state of the pipeline layout.
		 * \return Returns true if the pipeline layout was created.
		 **/
		bool													CreatePipelineLayout(
			VkDevice _dDevice,
			const VkPipelineLayoutCreateInfo * _pplciCreateInfo );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan pipeline layout handle.
		 **/
		inline VkPipelineLayout									Get() const { return m_plPipelineLayout.plPipelineLayout; }


	protected :
		// == Members.
		/** The wrapped RAII pipeline layout object. */
		CVulkan::LSN_PIPELINE_LAYOUT							m_plPipelineLayout;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
