/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 descriptor pool.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanDescriptorPool
	 * \brief A Vulkan 1.0 descriptor pool.
	 *
	 * Description: A Vulkan 1.0 descriptor pool.
	 */
	class CVulkanDescriptorPool {
	public :
		// == Various constructors.
		CVulkanDescriptorPool();
		~CVulkanDescriptorPool();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a descriptor pool object.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _pdpciCreateInfo A pointer to a VkDescriptorPoolCreateInfo structure specifying the state of the descriptor pool.
		 * \return Returns true if the descriptor pool was created.
		 **/
		bool													CreateDescriptorPool(
			VkDevice _dDevice,
			const VkDescriptorPoolCreateInfo * _pdpciCreateInfo );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan descriptor pool handle.
		 **/
		inline VkDescriptorPool									Get() const { return m_dpDescriptorPool.dpDescriptorPool; }


	protected :
		// == Members.
		/** The wrapped RAII descriptor pool object. */
		CVulkan::LSN_DESCRIPTOR_POOL							m_dpDescriptorPool;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
