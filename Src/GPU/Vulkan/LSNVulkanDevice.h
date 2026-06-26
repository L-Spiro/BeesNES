/**
 * Copyright L. Spiro 2026
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
		virtual bool											Create( HWND _hWnd, const std::string &_sAdapter ) override;

		/**
		 * Gets a pointer to the logical device.
		 *
		 * \return Returns a handle to the Vulkan device.
		 **/
		inline VkDevice											GetDevice() const { return m_dDevice.dDevice; }

		/**
		 * Gets a pointer to the command queue.
		 *
		 * \return Returns a handle to the command queue.
		 **/
		inline VkQueue											GetCommandQueue() const { return m_qGraphicsQueue; }

		/**
		 * Gets the physical device.
		 *
		 * \return Returns the physical device handle.
		 **/
		inline VkPhysicalDevice									GetPhysicalDevice() const { return m_pdPhysicalDevice; }

		/**
		 * Gets a pointer to the swap chain.
		 *
		 * \return Returns a handle to the swap chain.
		 **/
		inline VkSwapchainKHR									GetSwapChain() const { return m_sSwapchain.sSwapchain; }

		/**
		 * Resizes the swap chain backbuffers to match the current target window dimensions.
		 * Note: It is the caller's responsibility to release any active Render Target Views referencing the swap chain before calling this.
		 *
		 * \return Returns true if the swap chain was successfully resized.
		 **/
		bool													ResizeSwapChain();

		/**
		 * Flushes the command queue, blocking the CPU until the GPU has finished all pending operations.
		 *
		 * \return Returns true if the flush was successful.
		 **/
		bool													FlushCommandQueue();
		
		/**
		 * Frees all resources used by this object and leaves the object in a valid reusable state.
		 **/
		virtual void											Reset();


	protected :
		// == Members.
		CVulkan::LSN_INSTANCE									m_iInstance;						/**< The Vulkan instance. */
		CVulkan::LSN_SURFACE									m_sBackbuffer;						/**< Back buffer. */
		CVulkan::LSN_DEVICE										m_dDevice;							/**< The Vulkan device. */
		CVulkan::LSN_SWAPCHAIN									m_sSwapchain;						/**< The swapchain. */
		CVulkan::LSN_FENCE										m_fFence;							/**< The fence used to flush the command queue. */
		
		VkPhysicalDevice										m_pdPhysicalDevice = VK_NULL_HANDLE;/**< The physic device. */
		VkQueue													m_qGraphicsQueue = VK_NULL_HANDLE;	/**< The swapchain queue. */
		HWND													m_hWnd = NULL;						/**< The owning window. */
		
		VkFormat												m_fSwapFormat = VK_FORMAT_UNDEFINED;
		VkPresentModeKHR										m_pmPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		uint32_t												m_ui32QueueFamilyIndex = 0;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
