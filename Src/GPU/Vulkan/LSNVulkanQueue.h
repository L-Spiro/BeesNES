/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 queue.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanQueue
	 * \brief A Vulkan 1.0 queue.
	 *
	 * Description: A Vulkan 1.0 queue.
	 */
	class CVulkanQueue {
	public :
		// == Various constructors.
		CVulkanQueue();
		~CVulkanQueue();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Retrieves a queue handle from a logical device.
		 * 
		 * \param _dDevice The Vulkan logical device that owns the queue.
		 * \param _ui32QueueFamilyIndex The index of the queue family to which the queue belongs.
		 * \param _ui32QueueIndex The index within this queue family of the queue to retrieve.
		 * \return Returns true if the queue handle was successfully retrieved.
		 **/
		bool													GetDeviceQueue(
			VkDevice _dDevice,
			uint32_t _ui32QueueFamilyIndex,
			uint32_t _ui32QueueIndex = 0 );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan queue handle.
		 **/
		inline VkQueue											Get() const { return m_qQueue; }


	protected :
		// == Members.
		/** The queue we wrap. */
		VkQueue													m_qQueue = VK_NULL_HANDLE;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
