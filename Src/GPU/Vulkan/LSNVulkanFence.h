/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 fence.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanFence
	 * \brief A Vulkan 1.0 fence.
	 *
	 * Description: A Vulkan 1.0 fence.
	 */
	class CVulkanFence {
	public :
		// == Various constructors.
		CVulkanFence();
		~CVulkanFence();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates a fence object.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _bSignaled If true, creates the fence in the signaled state.
		 * \return Returns true if the fence was created.
		 **/
		bool													CreateFence(
			VkDevice _dDevice,
			bool _bSignaled = false );

		/**
		 * Blocks the CPU until the fence is signaled by the GPU.
		 *
		 * \param _ui64Timeout The timeout value in nanoseconds.
		 * \return Returns true if the wait was successful.
		 **/
		bool													Wait( uint64_t _ui64Timeout = UINT64_MAX ) const;

		/**
		 * Resets the fence to the unsignaled state.
		 *
		 * \return Returns true if the fence was successfully reset.
		 **/
		bool													ResetFence() const;

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan fence handle.
		 **/
		inline VkFence											Get() const { return m_fFence.fFence; }


	protected :
		// == Members.
		/** The wrapped RAII fence object. */
		CVulkan::LSN_FENCE										m_fFence;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
