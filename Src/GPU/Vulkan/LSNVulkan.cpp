/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The root of Vulkan 1.0 functionality.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkan.h"

namespace lsn {

	// == Members.
#include "LSNVulkanFuncDef.inl"
	

	

	BOOL CVulkan::m_bSupported = 3;																										/**< Is Vulkan 1.0 supported? */
	std::vector<VkPhysicalDeviceProperties> CVulkan::m_vDisplayDevices;																	/**< The array of display devices. */
	lsw::LSW_HMODULE CVulkan::m_hDll;																									/**< The vulkan-1.dll DLL. */

	// == Functions.
	/**
	 * Cleans up any resources used by Vulkan 1.0.
	 **/
	void CVulkan::ShutDown() {
		m_hDll.Reset();
		m_vDisplayDevices.clear();
		m_vDisplayDevices = std::vector<VkPhysicalDeviceProperties>();
		m_bSupported = 3;
	}

	/**
	 * Enumerates display devices via Vulkan 1.0.
	 * 
	 * \param _vDevices Display devices are added to _vDevices.
	 * \return Returns true if no allocations issues were encountered while adding display devices to _vDevices.
	 **/
	bool CVulkan::EnumerateDisplayDevices( std::vector<VkPhysicalDeviceProperties> &_vDevices ) {
		lsw::LSW_HMODULE hLib;
		if ( !LoadVulkan( hLib ) ) { return false; }


		// Initialize Vulkan.
		VkApplicationInfo aiAppInfo = CreateApplicationInfo();

		VkInstanceCreateInfo iciCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &aiAppInfo
		};

		LSN_INSTANCE iInstance( &iciCreateInfo );
		if ( !iInstance.Valid() ) { return false; }

		// Enumerate physical devices.
		uint32_t ui32DeviceCount = 0;
		CVulkan::m_pfEnumeratePhysicalDevices( iInstance.iInstance, &ui32DeviceCount, nullptr );
		if ( !ui32DeviceCount ) { return false; }

		std::vector<VkPhysicalDevice> dDevices( ui32DeviceCount );
		CVulkan::m_pfEnumeratePhysicalDevices( iInstance.iInstance, &ui32DeviceCount, dDevices.data() );

		for ( const auto & dDevice : dDevices ) {
			VkPhysicalDeviceProperties deviceProperties;
			CVulkan::m_pfGetPhysicalDeviceProperties( dDevice, &deviceProperties );
			try {
				_vDevices.push_back( deviceProperties );
			}
			catch ( ... ) { return false; }
		}
		return true;
	}

	/**
	 * Creates a VkApplicationInfo structure.
	 * 
	 * \param _pcApplicationName The name of the applictaion.  If nullptr, a default value is used.
	 * \return Returns a VkApplicationInfo structure.
	 **/
	VkApplicationInfo CVulkan::CreateApplicationInfo( const char * _pcApplicationName ) {
		return {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = _pcApplicationName ? _pcApplicationName : "BeesNES",
			.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
			.pEngineName = "BusyBeesNES",
			.engineVersion = VK_MAKE_VERSION( 1, 0, 0 ),
			.apiVersion = VK_API_VERSION_1_0 };
	}

	/**
	 * Loads the vulkan-1.dll DLL.  If the DLL is not already loaded and loads successfully, the Vulkan function pointers are set.
	 *
	 * \param _hDll The HMODULE into which to load the DLL.
	 * \return Returns true if the DLL was loaded or is already loaded.
	 **/
	bool CVulkan::LoadVulkan( lsw::LSW_HMODULE &_hDll ) {
		if ( _hDll.Valid() ) { return true; }
		if ( !_hDll.LoadLibrary( "vulkan-1.dll" ) ) { return false; }

#define LSN_LOAD_FUNC( NAME )																						\
	m_pf ## NAME = reinterpret_cast<PFN_vk ## NAME>(::GetProcAddress( _hDll.hHandle, "vk" #NAME ));					\
	if ( !m_pf ## NAME ) { return false; }

		LSN_LOAD_FUNC( CreateDevice );
		LSN_LOAD_FUNC( DestroyDevice );

		LSN_LOAD_FUNC( GetDeviceQueue );

		LSN_LOAD_FUNC( CreateInstance );
		LSN_LOAD_FUNC( DestroyInstance );

		LSN_LOAD_FUNC( EnumeratePhysicalDevices );
		LSN_LOAD_FUNC( GetPhysicalDeviceProperties );

#ifdef LSN_WINDOWS
		LSN_LOAD_FUNC( CreateWin32SurfaceKHR );
		LSN_LOAD_FUNC( DestroySurfaceKHR );
		LSN_LOAD_FUNC( CreateSwapchainKHR );
		LSN_LOAD_FUNC( DestroySwapchainKHR );

		LSN_LOAD_FUNC( GetPhysicalDeviceSurfaceFormatsKHR );
		LSN_LOAD_FUNC( GetPhysicalDeviceSurfacePresentModesKHR );
		LSN_LOAD_FUNC( GetPhysicalDeviceSurfaceCapabilitiesKHR );
#endif	// #ifdef LSN_WINDOWS

		LSN_LOAD_FUNC( CreateImage );
		LSN_LOAD_FUNC( DestroyImage );

		LSN_LOAD_FUNC( GetImageMemoryRequirements );

		LSN_LOAD_FUNC( AllocateMemory );
		LSN_LOAD_FUNC( FreeMemory );

		LSN_LOAD_FUNC( EnumerateInstanceExtensionProperties );
		LSN_LOAD_FUNC( EnumerateDeviceExtensionProperties );

		// Loading function pointers for command buffers.
		LSN_LOAD_FUNC( AllocateCommandBuffers );
		LSN_LOAD_FUNC( BeginCommandBuffer );
		LSN_LOAD_FUNC( EndCommandBuffer );
		LSN_LOAD_FUNC( ResetCommandBuffer );

		// Loading function pointers for command pools.
		LSN_LOAD_FUNC( CreateCommandPool );
		LSN_LOAD_FUNC( DestroyCommandPool );
		LSN_LOAD_FUNC( ResetCommandPool );

		// Loading function pointers for fences.
		LSN_LOAD_FUNC( CreateFence );
		LSN_LOAD_FUNC( DestroyFence );
		LSN_LOAD_FUNC( WaitForFences );
		LSN_LOAD_FUNC( ResetFences );

		// Loading function pointers for semaphores.
		LSN_LOAD_FUNC( CreateSemaphore );
		LSN_LOAD_FUNC( DestroySemaphore );

		// Loading function pointers for buffers.
		LSN_LOAD_FUNC( CreateBuffer );
		LSN_LOAD_FUNC( DestroyBuffer );
		LSN_LOAD_FUNC( GetBufferMemoryRequirements );

		// Loading function pointers for binding memory.
		LSN_LOAD_FUNC( BindBufferMemory );
		LSN_LOAD_FUNC( BindImageMemory );

		// Loading function pointers for image views.
		LSN_LOAD_FUNC( CreateImageView );
		LSN_LOAD_FUNC( DestroyImageView );

		// Loading function pointers for render passes.
		LSN_LOAD_FUNC( CreateRenderPass );
		LSN_LOAD_FUNC( DestroyRenderPass );

		// Loading function pointers for framebuffers.
		LSN_LOAD_FUNC( CreateFramebuffer );
		LSN_LOAD_FUNC( DestroyFramebuffer );

		// Loading function pointers for pipelines.
		LSN_LOAD_FUNC( CreateGraphicsPipelines );
		LSN_LOAD_FUNC( CreateComputePipelines );
		LSN_LOAD_FUNC( DestroyPipeline );

		// Loading function pointers for pipeline layouts.
		LSN_LOAD_FUNC( CreatePipelineLayout );
		LSN_LOAD_FUNC( DestroyPipelineLayout );

		// Loading function pointers for shader modules.
		LSN_LOAD_FUNC( CreateShaderModule );
		LSN_LOAD_FUNC( DestroyShaderModule );

		// Loading function pointers for descriptor set layouts.
		LSN_LOAD_FUNC( CreateDescriptorSetLayout );
		LSN_LOAD_FUNC( DestroyDescriptorSetLayout );

		// Loading function pointers for descriptor pools.
		LSN_LOAD_FUNC( CreateDescriptorPool );
		LSN_LOAD_FUNC( DestroyDescriptorPool );
		LSN_LOAD_FUNC( ResetDescriptorPool );

		// Loading function pointers for descriptor sets.
		LSN_LOAD_FUNC( AllocateDescriptorSets );
		LSN_LOAD_FUNC( FreeDescriptorSets );

		// Loading function pointers for queue submission and presentation.
		LSN_LOAD_FUNC( QueueSubmit );
		LSN_LOAD_FUNC( QueuePresentKHR );
		LSN_LOAD_FUNC( QueueWaitIdle );

		// Loading function pointers for device idle.
		LSN_LOAD_FUNC( DeviceWaitIdle );

		// Loading function pointers for memory mapping and flushing.
		LSN_LOAD_FUNC( MapMemory );
		LSN_LOAD_FUNC( UnmapMemory );
		LSN_LOAD_FUNC( FlushMappedMemoryRanges );
		LSN_LOAD_FUNC( InvalidateMappedMemoryRanges );


#undef LSN_LOAD_FUNC
		
		return true;
	}

	/**
	 * Checks for a set of given extensions.
	 * 
	 * \param _vExtensions The list of extensions for which to check.
	 * \return Returns true if all extensions are found.
	 **/
	bool CVulkan::CheckInstanceExtensionSupport( const std::vector<const char *> &_vExtensions ) {
		try {
			uint32_t ui32ExtensionCount;
			m_pfEnumerateInstanceExtensionProperties( nullptr, &ui32ExtensionCount, nullptr );

			std::vector<VkExtensionProperties> vAvailableExtensions( ui32ExtensionCount );
			m_pfEnumerateInstanceExtensionProperties( nullptr, &ui32ExtensionCount, vAvailableExtensions.data() );

			for ( const char * pcExtension : _vExtensions ) {
				bool bFound = false;
				for ( const auto & pcAvailableExtension : vAvailableExtensions ) {
					if ( std::strcmp( pcExtension, pcAvailableExtension.extensionName ) == 0 ) {
						bFound = true;
						break;
					}
				}
				if ( !bFound ) { return false; }
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Checks for a set of given extensions on a given device.
	 * 
	 * \param _pdDevice The device for which to check extensions.
	 * \param _vExtensions The list of extensions for which to check.
	 * \return Returns true if all extensions are found.
	 **/
	bool CVulkan::CheckDeviceExtensionSupport( VkPhysicalDevice _pdDevice, const std::vector<const char *> &_vExtensions ) {
		try {
			uint32_t ui32ExtensionCount;
			m_pfEnumerateDeviceExtensionProperties( _pdDevice, nullptr, &ui32ExtensionCount, nullptr );

			std::vector<VkExtensionProperties> vAvailableExtensions(ui32ExtensionCount);
			m_pfEnumerateDeviceExtensionProperties( _pdDevice, nullptr, &ui32ExtensionCount, vAvailableExtensions.data() );

			for ( const char * pcExtension : _vExtensions ) {
				bool bFound = false;
				for ( const auto & pcAvailableExtension : vAvailableExtensions ) {
					if ( std::strcmp( pcExtension, pcAvailableExtension.extensionName ) == 0 ) {
						bFound = true;
						break;
					}
				}
				if ( !bFound ) {
					return false;
				}
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Gets the format of the back surface.
	 *
	 * \param _pdDevice The device.
	 * \param _sSurface The window/back surface.
	 * \return Finds a valid format for the back buffer.
	 */
	VkFormat CVulkan::GetBackSurfaceFormat( VkPhysicalDevice _pdDevice, VkSurfaceKHR _sSurface ) {
		uint32_t ui32Count = 0;
		if ( CVulkan::m_pfGetPhysicalDeviceSurfaceFormatsKHR( _pdDevice, _sSurface,
			&ui32Count, NULL ) ) {
			return VK_FORMAT_UNDEFINED;
		}
		if ( !ui32Count ) { return VK_FORMAT_UNDEFINED; }
		try {
			std::vector<VkSurfaceFormatKHR> vFormats( ui32Count );

			// Now get the actual list.
			if ( CVulkan::m_pfGetPhysicalDeviceSurfaceFormatsKHR( _pdDevice, _sSurface,
				&ui32Count, &vFormats[0] ) ) {
				return VK_FORMAT_UNDEFINED;
			}

			VkFormat fRet;
			if ( ui32Count == 1 && vFormats[0].format == VK_FORMAT_UNDEFINED ) {
				fRet = VK_FORMAT_B8G8R8A8_UNORM;
			}
			else {
				fRet = vFormats[0].format;
			}
			return fRet;
		}
		catch ( ... ) { return VK_FORMAT_UNDEFINED; }
	}

	/**
	 * Gets the present mode.
	 *
	 * \param _pdDevice The device.
	 * \param _sSurface The window/back surface.
	 * \return Returns the present mode.
	 */
	VkPresentModeKHR CVulkan::GetPresentMode( VkPhysicalDevice _pdDevice, VkSurfaceKHR _sSurface ) {
		uint32_t ui32Count = 0;
		if ( CVulkan::m_pfGetPhysicalDeviceSurfacePresentModesKHR( _pdDevice, _sSurface,
			&ui32Count, NULL ) ) { return VK_PRESENT_MODE_FIFO_KHR; }
		if ( !ui32Count ) { return VK_PRESENT_MODE_FIFO_KHR; }

		try {
			std::vector<VkPresentModeKHR> vPresentMode( ui32Count );
			if ( CVulkan::m_pfGetPhysicalDeviceSurfacePresentModesKHR( _pdDevice, _sSurface,
				&ui32Count, &vPresentMode[0] ) ) {
				return VK_PRESENT_MODE_FIFO_KHR;
			}

			VkPresentModeKHR pmRet = VK_PRESENT_MODE_FIFO_KHR;
			for ( uint32_t I = 0; I < ui32Count; ++I ) {
				if ( vPresentMode[I] == VK_PRESENT_MODE_MAILBOX_KHR ) {
					pmRet = vPresentMode[I];
					break;
				}
				if ( vPresentMode[I] == VK_PRESENT_MODE_IMMEDIATE_KHR ) {
					pmRet = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}

			return pmRet;
		}
		catch ( ... ) { return VK_PRESENT_MODE_FIFO_KHR; }
	}

	/**
	 * Checks for Vulkan 1.0 support.
	 * 
	 * \return Returns true if Vulkan 1.0 is supported.
	 **/
	bool CVulkan::IsSupported() {
		GatherDevices();

		return m_vDisplayDevices.size() != 0;
	}

	/**
	 * Populates m_vDisplayDevices.
	 **/
	void CVulkan::GatherDevices() {
		if ( !m_vDisplayDevices.size() ) {
			EnumerateDisplayDevices( m_vDisplayDevices );
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
