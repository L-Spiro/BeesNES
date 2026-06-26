/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 command buffer.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanCommandBuffer
	 * \brief A Vulkan 1.0 command buffer.
	 *
	 * Description: A Vulkan 1.0 command buffer.
	 */
	class CVulkanCommandBuffer {
	public :
		// == Various constructors.
		CVulkanCommandBuffer();
		~CVulkanCommandBuffer();


		// == Functions.
		/**
		 * Resets the object back to scratch, freeing the command buffer.
		 **/
		void													Reset();

		/**
		 * Creates a command buffer.
		 *
		 * \param _dDevice The Vulkan logical device.
		 * \param _cpCommandPool The command pool from which to allocate the command buffer.
		 * \param _cblLevel Specifies whether the command buffer is a primary or secondary command buffer.
		 * \return Returns true if the command buffer was successfully allocated.
		 **/
		bool													CreateCommandBuffer(
			VkDevice _dDevice,
			VkCommandPool _cpCommandPool,
			VkCommandBufferLevel _cblLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan command buffer handle.
		 **/
		inline VkCommandBuffer									Get() const { return m_cbCommandBuffer.cbCommandBuffer; }


	protected :
		// == Members.
		/** The wrapped RAII command buffer object. */
		CVulkan::LSN_COMMAND_BUFFER								m_cbCommandBuffer;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
