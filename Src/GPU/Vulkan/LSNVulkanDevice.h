/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Vulkan 1 GPU object.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "../LSNGpuBase.h"
#include "LSNVulkan.h"

namespace lsn {

	/**
	 * Class CVulkanDevice
	 * \brief The Vulkan 1 GPU object.
	 *
	 * Description: The Vulkan 1 GPU object.
	 */
	class CVulkanDevice : public CGpuBase {
	public :
		CVulkanDevice();
		virtual ~CVulkanDevice();


		// == Functions.
		/**
		 * Creates a Vulkan 1 device.
		 *
		 * \param _hWnd The window to which to attach.
		 * \param _sAdapter The adapter to use.
		 * \return Returns true if the device was created.
		 **/
		virtual bool											Create( HWND _hWnd, const std::string &_sAdapter );

	protected :
		// == Members.
		CVulkan::LSN_DEVICE										m_dDevice;					/**< The Vulkan device. */
		CVulkan::LSN_INSTANCE									m_iInstance;				/**< The Vulkan instance. */
		CVulkan::LSN_SWAPCHAIN									m_sSwapchain;				/**< The swapchain. */
		CVulkan::LSN_SURFACE									m_sBackbuffer;				/**< Back buffer. */



		// == Functions.
		/**
		 * Frees all resources used by this object and leaves the object in a valid reusable state.
		 **/
		virtual void											Reset();

	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
