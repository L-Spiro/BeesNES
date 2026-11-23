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

#include <vector>

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
		 * Gets the format of the swap-chain images.
		 *
		 * \return Returns the format of the swap-chain images.
		 **/
		inline VkFormat											GetSwapFormat() const { return m_fSwapFormat; }

		/**
		 * Gets the size of the swap-chain images.
		 *
		 * \return Returns the size of the swap-chain images.
		 **/
		inline VkExtent2D										GetSwapExtent() const { return m_eSwapExtent; }

		/**
		 * Determines whether the swap-chain images can be copied from (VK_IMAGE_USAGE_TRANSFER_SRC_BIT), which capturing them requires.
		 *
		 * \return Returns true if the swap-chain images can be the source of copies.
		 **/
		inline bool												SwapChainCanCopy() const { return m_bSwapCanCopy; }

		/**
		 * Gets a number that changes every time the swap chain is created or re-created.  Image views and framebuffers made from the swap-chain
		 *	images must be re-created when it changes (comparing image handles is not enough:  a driver can reuse them).
		 *
		 * \return Returns the generation of the swap chain.
		 **/
		inline uint64_t											SwapChainGeneration() const { return m_ui64SwapGeneration; }

		/**
		 * Destroys the swap chains replaced by ResizeSwapChain().  They are kept until the image views and framebuffers made from their images
		 *	have been destroyed (call this after releasing them).
		 **/
		void													DestroyRetiredSwapChains();

		/**
		 * Determines whether images of a format can be sampled with linear filtering (optional for 32-bit floating-point formats).
		 *
		 * \param _fFormat The format to check.
		 * \return Returns true if images of the format can be sampled with VK_FILTER_LINEAR.
		 **/
		bool													SupportsLinearFilter( VkFormat _fFormat ) const;

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
		std::vector<CVulkan::LSN_SWAPCHAIN>						m_vRetiredSwapchains;				/**< Swap chains replaced by ResizeSwapChain() whose image views may still exist. */
		CVulkan::LSN_FENCE										m_fFence;							/**< The fence used to flush the command queue. */
		
		VkPhysicalDevice										m_pdPhysicalDevice = VK_NULL_HANDLE;/**< The physic device. */
		VkQueue													m_qGraphicsQueue = VK_NULL_HANDLE;	/**< The swapchain queue. */
		HWND													m_hWnd = NULL;						/**< The owning window. */
		
		VkFormat												m_fSwapFormat = VK_FORMAT_UNDEFINED;	/**< The swap-chain image format. */
		VkPresentModeKHR										m_pmPresentMode = VK_PRESENT_MODE_FIFO_KHR;	/**< The swap-chain present mode. */
		uint32_t												m_ui32QueueFamilyIndex = 0;			/**< The graphics/present queue family. */
		uint32_t												m_ui32SwapImages = 0;				/**< The swap-chain image count chosen by Create().  ResizeSwapChain() must keep it. */
		VkExtent2D												m_eSwapExtent = { 0, 0 };			/**< The size of the swap-chain images. */
		bool													m_bSwapCanCopy = false;				/**< True if the swap-chain images were created with VK_IMAGE_USAGE_TRANSFER_SRC_BIT. */
		uint64_t												m_ui64SwapGeneration = 0;			/**< Changes every time the swap chain is created. */
		static uint64_t											s_ui64NextGeneration;				/**< The next swap-chain generation (shared so that a new device never repeats one). */
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
