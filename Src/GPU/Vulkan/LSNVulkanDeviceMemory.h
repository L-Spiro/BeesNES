/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 device memory allocation.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanDeviceMemory
	 * \brief A Vulkan 1.0 device memory allocation.
	 *
	 * Description: A Vulkan 1.0 device memory allocation.
	 */
	class CVulkanDeviceMemory {
	public :
		// == Various constructors.
		CVulkanDeviceMemory();
		~CVulkanDeviceMemory();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Allocates device memory.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _pmaiAllocateInfo A pointer to a VkMemoryAllocateInfo structure describing parameters of the allocation.
		 * \return Returns true if the device memory was allocated.
		 **/
		bool													AllocateMemory(
			VkDevice _dDevice,
			const VkMemoryAllocateInfo * _pmaiAllocateInfo );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan device memory handle.
		 **/
		inline VkDeviceMemory									Get() const { return m_dmDeviceMemory.dmDeviceMemory; }


	protected :
		// == Members.
		/** The wrapped RAII device memory object. */
		CVulkan::LSN_DEVICE_MEMORY								m_dmDeviceMemory;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
