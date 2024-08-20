/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The root of Vulkan 1.0 functionality.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "../../OS/LSNWindows.h"
#include "../LSNGpuBase.h"
#include "Helpers/LSWHelpers.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include <vector>

namespace lsn {

	/**
	 * Class CVulkan
	 * \brief The root of Vulkan 1.0 functionality.
	 *
	 * Description: The root of Vulkan 1.0 functionality.
	 */
	class CVulkan {
	public :
		// == Types.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateDevice)( VkPhysicalDevice, const VkDeviceCreateInfo *, const VkAllocationCallbacks *, VkDevice * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyDevice)( VkDevice, const VkAllocationCallbacks * );

		typedef VkResult (VKAPI_PTR *							PFN_vkCreateInstance)( const VkInstanceCreateInfo *, const VkAllocationCallbacks *, VkInstance * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyInstance)( VkInstance, const VkAllocationCallbacks * );

		typedef void (VKAPI_PTR *								PFN_vkGetDeviceQueue)( VkDevice, uint32_t, uint32_t, VkQueue * );
		typedef VkResult (VKAPI_PTR *							PFN_vkEnumeratePhysicalDevices)( VkInstance, uint32_t *, VkPhysicalDevice * );
		typedef void (VKAPI_PTR *								PFN_vkGetPhysicalDeviceProperties)( VkPhysicalDevice, VkPhysicalDeviceProperties * );

		typedef VkResult (VKAPI_PTR *							PFN_vkCreateWin32SurfaceKHR)( VkInstance, const VkWin32SurfaceCreateInfoKHR *, const VkAllocationCallbacks *, VkSurfaceKHR * );
		typedef void (VKAPI_PTR *								PFN_vkDestroySurfaceKHR)( VkInstance, VkSurfaceKHR, const VkAllocationCallbacks * );

		typedef VkResult (VKAPI_PTR *							PFN_vkCreateSwapchainKHR)( VkDevice, const VkSwapchainCreateInfoKHR *, const VkAllocationCallbacks *, VkSwapchainKHR * );
		typedef void (VKAPI_PTR *								PFN_vkDestroySwapchainKHR)( VkDevice, VkSwapchainKHR, const VkAllocationCallbacks * );

		typedef VkResult (VKAPI_PTR *							PFN_vkEnumerateInstanceExtensionProperties)( const char *, uint32_t *, VkExtensionProperties * );
		typedef VkResult (VKAPI_PTR *							PFN_vkEnumerateDeviceExtensionProperties)( VkPhysicalDevice, const char *, uint32_t *, VkExtensionProperties * );

		typedef VkResult (VKAPI_PTR *							PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)( VkPhysicalDevice, VkSurfaceKHR, uint32_t *, VkSurfaceFormatKHR * );

		typedef VkResult (VKAPI_PTR *							PFN_vkCreateImage)( VkDevice, const VkImageCreateInfo *, const VkAllocationCallbacks *, VkImage * );
		typedef VkResult (VKAPI_PTR *							PFN_vkDestroyImage)( VkDevice, VkImage, const VkAllocationCallbacks * );

		typedef void (VKAPI_PTR *								PFN_vkGetImageMemoryRequirements)( VkDevice, VkImage, VkMemoryRequirements * );

		typedef VkResult (VKAPI_PTR *							PFN_vkAllocateMemory)( VkDevice, const VkMemoryAllocateInfo *, const VkAllocationCallbacks *, VkDeviceMemory * );
		typedef VkResult (VKAPI_PTR *							PFN_vkFreeMemory)( VkDevice, VkDeviceMemory, const VkAllocationCallbacks * );

		typedef VkResult (VKAPI_PTR *							PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)( VkPhysicalDevice, VkSurfaceKHR, uint32_t *, VkPresentModeKHR * );
		typedef VkResult (VKAPI_PTR *							PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)( VkPhysicalDevice, VkSurfaceKHR, VkSurfaceCapabilitiesKHR * );

		// Command buffers.
		typedef VkResult (VKAPI_PTR *							PFN_vkAllocateCommandBuffers)( VkDevice, const VkCommandBufferAllocateInfo *, VkCommandBuffer * );
		typedef VkResult (VKAPI_PTR *							PFN_vkBeginCommandBuffer)( VkCommandBuffer, const VkCommandBufferBeginInfo * );
		typedef VkResult (VKAPI_PTR *							PFN_vkEndCommandBuffer)( VkCommandBuffer );
		typedef VkResult (VKAPI_PTR *							PFN_vkResetCommandBuffer)( VkCommandBuffer, VkCommandBufferResetFlags );

		// Command pool.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateCommandPool)( VkDevice, const VkCommandPoolCreateInfo *, const VkAllocationCallbacks *, VkCommandPool * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyCommandPool)( VkDevice, VkCommandPool, const VkAllocationCallbacks * );
		typedef VkResult (VKAPI_PTR *							PFN_vkResetCommandPool)( VkDevice, VkCommandPool, VkCommandPoolResetFlags );

		// Fence.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateFence)( VkDevice, const VkFenceCreateInfo *, const VkAllocationCallbacks *, VkFence * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyFence)( VkDevice, VkFence, const VkAllocationCallbacks * );
		typedef VkResult (VKAPI_PTR *							PFN_vkWaitForFences)( VkDevice, uint32_t, const VkFence *, VkBool32, uint64_t );
		typedef VkResult (VKAPI_PTR *							PFN_vkResetFences)( VkDevice, uint32_t, const VkFence * );

		// Semaphore.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateSemaphore)( VkDevice, const VkSemaphoreCreateInfo *, const VkAllocationCallbacks *, VkSemaphore * );
		typedef void (VKAPI_PTR *								PFN_vkDestroySemaphore)( VkDevice, VkSemaphore, const VkAllocationCallbacks * );

		// Buffer.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateBuffer)( VkDevice, const VkBufferCreateInfo *, const VkAllocationCallbacks *, VkBuffer * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyBuffer)( VkDevice, VkBuffer, const VkAllocationCallbacks * );
		typedef void (VKAPI_PTR *								PFN_vkGetBufferMemoryRequirements)( VkDevice, VkBuffer, VkMemoryRequirements * );

		// Buffer memory.
		typedef VkResult (VKAPI_PTR *							PFN_vkBindBufferMemory)( VkDevice, VkBuffer, VkDeviceMemory, VkDeviceSize );

		// Image memory.
		typedef VkResult (VKAPI_PTR *							PFN_vkBindImageMemory)( VkDevice, VkImage, VkDeviceMemory, VkDeviceSize );

		// Image View.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateImageView)( VkDevice, const VkImageViewCreateInfo *, const VkAllocationCallbacks *, VkImageView * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyImageView)( VkDevice, VkImageView, const VkAllocationCallbacks * );

		// Render Pass.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateRenderPass)( VkDevice, const VkRenderPassCreateInfo *, const VkAllocationCallbacks *, VkRenderPass * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyRenderPass)( VkDevice, VkRenderPass, const VkAllocationCallbacks * );

		// Framebuffer.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateFramebuffer)( VkDevice, const VkFramebufferCreateInfo *, const VkAllocationCallbacks *, VkFramebuffer * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyFramebuffer)( VkDevice, VkFramebuffer, const VkAllocationCallbacks * );

		// Pipeline.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateGraphicsPipelines)( VkDevice, VkPipelineCache, uint32_t, const VkGraphicsPipelineCreateInfo *, const VkAllocationCallbacks *, VkPipeline * );
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateComputePipelines)( VkDevice, VkPipelineCache, uint32_t, const VkComputePipelineCreateInfo *, const VkAllocationCallbacks *, VkPipeline * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyPipeline)( VkDevice, VkPipeline, const VkAllocationCallbacks * );

		// Pipeline Layout.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreatePipelineLayout)( VkDevice, const VkPipelineLayoutCreateInfo *, const VkAllocationCallbacks *, VkPipelineLayout * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyPipelineLayout)( VkDevice, VkPipelineLayout, const VkAllocationCallbacks * );

		// Shader Module.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateShaderModule)( VkDevice, const VkShaderModuleCreateInfo *, const VkAllocationCallbacks *, VkShaderModule * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyShaderModule)( VkDevice, VkShaderModule, const VkAllocationCallbacks * );

		// Descriptor Set Layout.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateDescriptorSetLayout)( VkDevice, const VkDescriptorSetLayoutCreateInfo *, const VkAllocationCallbacks *, VkDescriptorSetLayout * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyDescriptorSetLayout)( VkDevice, VkDescriptorSetLayout, const VkAllocationCallbacks * );

		// Descriptor Pool.
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateDescriptorPool)( VkDevice, const VkDescriptorPoolCreateInfo *, const VkAllocationCallbacks *, VkDescriptorPool * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyDescriptorPool)( VkDevice, VkDescriptorPool, const VkAllocationCallbacks * );
		typedef VkResult (VKAPI_PTR *							PFN_vkResetDescriptorPool)( VkDevice, VkDescriptorPool, VkDescriptorPoolResetFlags );

		// Descriptor Sets.
		typedef VkResult (VKAPI_PTR *							PFN_vkAllocateDescriptorSets)( VkDevice, const VkDescriptorSetAllocateInfo *, VkDescriptorSet * );
		typedef VkResult (VKAPI_PTR *							PFN_vkFreeDescriptorSets)( VkDevice, VkDescriptorPool, uint32_t, const VkDescriptorSet * );

		// Command buffer submission.
		typedef VkResult (VKAPI_PTR *							PFN_vkQueueSubmit)( VkQueue, uint32_t, const VkSubmitInfo *, VkFence );
		typedef VkResult (VKAPI_PTR *							PFN_vkQueuePresentKHR)( VkQueue, const VkPresentInfoKHR * );
		typedef VkResult (VKAPI_PTR *							PFN_vkQueueWaitIdle)( VkQueue );

		// Device wait.
		typedef VkResult (VKAPI_PTR *							PFN_vkDeviceWaitIdle)( VkDevice );

		// Memory mapping.
		typedef VkResult (VKAPI_PTR *							PFN_vkMapMemory)( VkDevice, VkDeviceMemory, VkDeviceSize, VkDeviceSize, VkMemoryMapFlags, void ** );
		typedef void (VKAPI_PTR *								PFN_vkUnmapMemory)( VkDevice, VkDeviceMemory );

		// Memory flushing.
		typedef VkResult (VKAPI_PTR *							PFN_vkFlushMappedMemoryRanges)( VkDevice, uint32_t, const VkMappedMemoryRange * );
		typedef VkResult (VKAPI_PTR *							PFN_vkInvalidateMappedMemoryRanges)( VkDevice, uint32_t, const VkMappedMemoryRange * );


#include "LSNVulkanWrappers.inl"
		


		// == Functions.
		/**
		 * Cleans up any resources used by Vulkan 1.0.
		 **/
		static void												ShutDown();

		/**
		 * Determines whether Vulkan 1.0 is available.
		 *
		 * \return Returns TRUE if Vulkan 1.0 functionality is available.
		 **/
		static inline BOOL										Supported();

		/**
		 * Enumerates display devices via Vulkan 1.0.
		 * 
		 * \param _vDevices Display devices are added to _vDevices.
		 * \return Returns true if no allocations issues were encountered while adding display devices to _vDevices.
		 **/
		static bool												EnumerateDisplayDevices( std::vector<VkPhysicalDeviceProperties> &_vDevices );

		/**
		 * Creates a VkApplicationInfo structure.
		 * 
		 * \param _pcApplicationName The name of the applictaion.  If nullptr, a default value is used.
		 * \return Returns a VkApplicationInfo structure.
		 **/
		static VkApplicationInfo								CreateApplicationInfo( const char * _pcApplicationName = nullptr );

		/**
		 * Loads the vulkan-1.dll DLL.  If the DLL is not already loaded and loads successfully, the Vulkan function pointers are set.
		 *
		 * \param _hDll The HMODULE into which to load the DLL.
		 * \return Returns true if the DLL was loaded or is already loaded.
		 **/
		static bool												LoadVulkan( lsw::LSW_HMODULE &_hDll );

		/**
		 * Checks for a set of given extensions.
		 * 
		 * \param _vExtensions The list of extensions for which to check.
		 * \return Returns true if all extensions are found.
		 **/
		static bool												CheckInstanceExtensionSupport( const std::vector<const char *> &_vExtensions );

		/**
		 * Checks for a set of given extensions on a given device.
		 * 
		 * \param _pdDevice The device for which to check extensions.
		 * \param _vExtensions The list of extensions for which to check.
		 * \return Returns true if all extensions are found.
		 **/
		static bool												CheckDeviceExtensionSupport( VkPhysicalDevice _pdDevice, const std::vector<const char *> &_vExtensions );

		/**
		 * Gets the format of the back surface.
		 *
		 * \param _pdDevice The device.
		 * \param _sSurface The window/back surface.
		 * \return Finds a valid format for the back buffer.
		 */
		static VkFormat											GetBackSurfaceFormat( VkPhysicalDevice _pdDevice, VkSurfaceKHR _sSurface );

		/**
		 * Gets the present mode.
		 *
		 * \param _pdDevice The device.
		 * \param _sSurface The window/back surface.
		 * \return Returns the present mode.
		 */
		static VkPresentModeKHR									GetPresentMode( VkPhysicalDevice _pdDevice, VkSurfaceKHR _sSurface );


#include "LSNVulkanFuncDecl.inl"


	protected :
		// == Members.
		static BOOL												m_bSupported;								/**< Is Vulkan 1.0 supported? */
		static std::vector<VkPhysicalDeviceProperties>			m_vDisplayDevices;							/**< The array of display devices. */
		static lsw::LSW_HMODULE									m_hDll;										/**< The vulkan-1.dll DLL. */


		// == Functions.
		/**
		 * Checks for Vulkan 1.0 support.
		 * 
		 * \return Returns true if Vulkan 1.0 is supported.
		 **/
		static bool												IsSupported();

		/**
		 * Populates m_vDisplayDevices.
		 **/
		static void												GatherDevices();
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Determines whether Vulkan 1.0 is available.
	 *
	 * \return Returns TRUE if Vulkan 1.0 functionality is available.
	 **/
	inline BOOL CVulkan::Supported() {
		if ( m_bSupported == 3 ) { m_bSupported = IsSupported(); }
		return m_bSupported;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
