/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 buffer.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanBuffer
	 * \brief A Vulkan 1.0 buffer.
	 *
	 * Description: A Vulkan 1.0 buffer.
	 */
	class CVulkanBuffer {
	public :
		// == Various constructors.
		CVulkanBuffer();
		~CVulkanBuffer();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a buffer object.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _pbciCreateInfo A pointer to a VkBufferCreateInfo structure specifying the parameters for creating the buffer.
		 * \return Returns true if the buffer was created.
		 **/
		bool													CreateBuffer(
			VkDevice _dDevice,
			const VkBufferCreateInfo * _pbciCreateInfo );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan buffer handle.
		 **/
		inline VkBuffer											Get() const { return m_bBuffer.bBuffer; }


	protected :
		// == Members.
		/** The wrapped RAII buffer object. */
		CVulkan::LSN_BUFFER										m_bBuffer;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
