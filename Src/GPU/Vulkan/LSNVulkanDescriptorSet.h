/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 descriptor set.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanDescriptorSet
	 * \brief A Vulkan 1.0 descriptor set.
	 *
	 * Description: A Vulkan 1.0 descriptor set.
	 */
	class CVulkanDescriptorSet {
	public :
		// == Various constructors.
		CVulkanDescriptorSet();
		~CVulkanDescriptorSet();


		// == Functions.
		/**
		 * Resets the object back to scratch, freeing the descriptor set if the pool allows it.
		 **/
		void													Reset();

		/**
		 * Allocates a single descriptor set from a descriptor pool.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _dpDescriptorPool The descriptor pool to allocate the set from.
		 * \param _dslLayout The descriptor set layout specifying the bindings.
		 * \return Returns true if the descriptor set was successfully allocated.
		 **/
		bool													AllocateDescriptorSet(
			VkDevice _dDevice,
			VkDescriptorPool _dpDescriptorPool,
			VkDescriptorSetLayout _dslLayout );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan descriptor set handle.
		 **/
		inline VkDescriptorSet									Get() const { return m_dsDescriptorSet.dsDescriptorSet; }


	protected :
		// == Members.
		/** The wrapped RAII descriptor set object. */
		CVulkan::LSN_DESCRIPTOR_SET								m_dsDescriptorSet;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
