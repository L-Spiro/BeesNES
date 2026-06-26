/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 command pool.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanCommandPool
	 * \brief A Vulkan 1.0 command pool.
	 *
	 * Description: A Vulkan 1.0 command pool.
	 */
	class CVulkanCommandPool {
	public :
		// == Various constructors.
		CVulkanCommandPool();
		~CVulkanCommandPool();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a command pool object.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _ui32QueueFamilyIndex The queue family index for which the command pool is created.
		 * \param _cpcfFlags Flags indicating the behavior of the command pool (e.g., VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT).
		 * \param _pacAllocator Optional allocator callbacks.
		 * \return Returns true if the command pool was created.
		 **/
		bool													CreateCommandPool(
			VkDevice _dDevice,
			uint32_t _ui32QueueFamilyIndex,
			VkCommandPoolCreateFlags _cpcfFlags = 0,
			const VkAllocationCallbacks * _pacAllocator = nullptr );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan command pool handle.
		 **/
		inline VkCommandPool									Get() const { return m_cpCommandPool.cpCommandPool; }


	protected :
		// == Members.
		/** The command pool we wrap. */
		CVulkan::LSN_COMMAND_POOL								m_cpCommandPool;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
