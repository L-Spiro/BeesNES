/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 semaphore.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanSemaphore
	 * \brief A Vulkan 1.0 semaphore.
	 *
	 * Description: A Vulkan 1.0 semaphore.
	 */
	class CVulkanSemaphore {
	public :
		// == Various constructors.
		CVulkanSemaphore();
		~CVulkanSemaphore();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a semaphore object.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \return Returns true if the semaphore was created.
		 **/
		bool													CreateSemaphore(
			VkDevice _dDevice );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan semaphore handle.
		 **/
		inline VkSemaphore										Get() const { return m_sSemaphore.sSemaphore; }


	protected :
		// == Members.
		/** The wrapped RAII semaphore object. */
		CVulkan::LSN_SEMAPHORE									m_sSemaphore;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
