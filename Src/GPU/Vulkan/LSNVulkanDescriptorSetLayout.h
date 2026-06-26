/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 descriptor set layout.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanDescriptorSetLayout
	 * \brief A Vulkan 1.0 descriptor set layout.
	 *
	 * Description: A Vulkan 1.0 descriptor set layout.
	 */
	class CVulkanDescriptorSetLayout {
	public :
		// == Various constructors.
		CVulkanDescriptorSetLayout();
		~CVulkanDescriptorSetLayout();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a descriptor set layout object.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _pdslciCreateInfo A pointer to a VkDescriptorSetLayoutCreateInfo structure specifying the state of the descriptor set layout.
		 * \return Returns true if the descriptor set layout was created.
		 **/
		bool													CreateDescriptorSetLayout(
			VkDevice _dDevice,
			const VkDescriptorSetLayoutCreateInfo * _pdslciCreateInfo );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan descriptor set layout handle.
		 **/
		inline VkDescriptorSetLayout							Get() const { return m_dslDescriptorSetLayout.dslDescriptorSetLayout; }


	protected :
		// == Members.
		/** The wrapped RAII descriptor set layout object. */
		CVulkan::LSN_DESCRIPTOR_SET_LAYOUT						m_dslDescriptorSetLayout;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
