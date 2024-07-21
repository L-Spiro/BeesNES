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
	/**
	 * Create a new device instance.
	 * 
	 * \param _physicalDevice Must be one of the device handles returned from a call to vkEnumeratePhysicalDevices.
	 * \param _pCreateInfo A pointer to a VkDeviceCreateInfo structure containing information about how to create the device.
	 * \param _pAllocator Controls host memory allocation.
	 * \param _pDevice A pointer to a handle in which the created VkDevice is returned.
	 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_INITIALIZATION_FAILED, VK_ERROR_EXTENSION_NOT_PRESENT, VK_ERROR_FEATURE_NOT_PRESENT, VK_ERROR_TOO_MANY_OBJECTS, VK_ERROR_DEVICE_LOST.
	 **/
	CVulkan::PFN_vkCreateDevice CVulkan::m_pfCreateDevice = nullptr;

	/**
	 * Destroy a logical device.
	 * 
	 * \param _device The logical device to destroy.
	 * \param _pAllocator Controls host memory allocation.
	 **/
	CVulkan::PFN_vkDestroyDevice CVulkan::m_pfDestroyDevice = nullptr;

	/**
	 * Get a queue handle from a device.
	 * 
	 * \param _device The logical device that owns the queue.
	 * \param _queueFamilyIndex The index of the queue family to which the queue belongs.
	 * \param _queueIndex The index within this queue family of the queue to retrieve.
	 * \param _pQueue A pointer to a VkQueue object that will be filled with the handle for the requested queue.
	 **/
	CVulkan::PFN_vkGetDeviceQueue CVulkan::m_pfGetDeviceQueue = nullptr;

	/**
	 * Create a new Vulkan instance.
	 * 
	 * \param _pCreateInfo A pointer to a VkInstanceCreateInfo structure controlling creation of the instance.
	 * \param _pAllocator Controls host memory allocation.
	 * \param _pInstance Points a VkInstance handle in which the resulting instance is returned.
	 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_INITIALIZATION_FAILED, VK_ERROR_LAYER_NOT_PRESENT, VK_ERROR_EXTENSION_NOT_PRESENT, VK_ERROR_INCOMPATIBLE_DRIVER
	 **/
	CVulkan::PFN_vkCreateInstance CVulkan::m_pfCreateInstance = nullptr;

	/**
	 * Destroy an instance of Vulkan.
	 * 
	 * \param _instance The handle of the instance to destroy.
	 * \param _pAllocator Controls host memory allocation.
	 **/
	PFN_vkDestroyInstance CVulkan::m_pfDestroyInstance = nullptr;

	/**
	 * Enumerates the physical devices accessible to a Vulkan instance.
	 * 
	 * \param _instance A handle to a Vulkan instance previously created with vkCreateInstance.
	 * \param _pPhysicalDeviceCount A pointer to an integer related to the number of physical devices available or queried.
	 * \param _pPhysicalDevices Either NULL or a pointer to an array of VkPhysicalDevice handles.
	 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_INITIALIZATION_FAILED.
	 **/
	CVulkan::PFN_vkEnumeratePhysicalDevices CVulkan::m_pfEnumeratePhysicalDevices = nullptr;

	/**
	 * Returns properties of a physical device.
	 * 
	 * \param _physicalDevice The handle to the physical device whose properties will be queried.
	 * \param _pProperties A pointer to a VkPhysicalDeviceProperties structure in which properties are returned.
	 **/
	CVulkan::PFN_vkGetPhysicalDeviceProperties CVulkan::m_pfGetPhysicalDeviceProperties = nullptr;

#ifdef LSN_WINDOWS
	/**
	 * Create a VkSurfaceKHR object for an Win32 native window.
	 * 
	 * \param _instance The instance to associate the surface with.
	 * \param _pCreateInfo A pointer to a VkWin32SurfaceCreateInfoKHR structure containing parameters affecting the creation of the surface object.
	 * \param _pAllocator The allocator used for host memory allocated for the surface object when there is no more specific allocator available.
	 * \param _pSurface A pointer to a VkSurfaceKHR handle in which the created surface object is returned.
	 * \return On success, this command returns: VK_SUCCESS.  On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY
	 **/
	CVulkan::PFN_vkCreateWin32SurfaceKHR CVulkan::m_pfCreateWin32SurfaceKHR = nullptr;

	/**
	 * Destroy a VkSurfaceKHR object.
	 * 
	 * \param _instance The instance used to create the surface.
	 * \param _surface The surface to destroy.
	 * \param _pAllocator The allocator used for host memory allocated for the surface object when there is no more specific allocator available.
	 **/
	CVulkan::PFN_vkDestroySurfaceKHR CVulkan::m_pfDestroySurfaceKHR = nullptr;

	/**
	 * Create a swapchain.
	 * 
	 * \param _device The device to create the swapchain for.
	 * \param _pCreateInfo A pointer to a VkSwapchainCreateInfoKHR structure specifying the parameters of the created swapchain.
	 * \param _pAllocator The allocator used for host memory allocated for the swapchain object when there is no more specific allocator available.
	 * \param _pSwapchain A pointer to a VkSwapchainKHR handle in which the created swapchain object will be returned.
	 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_DEVICE_LOST, VK_ERROR_SURFACE_LOST_KHR, VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, VK_ERROR_INITIALIZATION_FAILED, VK_ERROR_COMPRESSION_EXHAUSTED_EXT.
	 **/
	CVulkan::PFN_vkCreateSwapchainKHR CVulkan::m_pfCreateSwapchainKHR = nullptr;

	/**
	 * Destroy a swapchain object.
	 * 
	 * \param _device The VkDevice associated with swapchain.
	 * \param _swapchain The swapchain to destroy.
	 * \param _pAllocator The allocator used for host memory allocated for the swapchain object when there is no more specific allocator available.
	 **/
	CVulkan::PFN_vkDestroySwapchainKHR CVulkan::m_pfDestroySwapchainKHR = nullptr;

	/**
	 * Query color formats supported by surface.
	 * 
	 * \param _physicalDevice The physical device that will be associated with the swapchain to be created, as described for vkCreateSwapchainKHR.
	 * \param _surface The surface that will be associated with the swapchain.
	 * \param _pSurfaceFormatCount A pointer to an integer related to the number of format pairs available or queried.
	 * \param _pSurfaceFormats Either NULL or a pointer to an array of VkSurfaceFormatKHR structures.
	 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_SURFACE_LOST_KHR.
	 **/
	CVulkan::PFN_vkGetPhysicalDeviceSurfaceFormatsKHR CVulkan::m_pfGetPhysicalDeviceSurfaceFormatsKHR = nullptr;

	/**
	 * Query supported presentation modes.
	 * 
	 * \param _physicalDevice The physical device that will be associated with the swapchain to be created, as described for vkCreateSwapchainKHR.
	 * \param _surface The surface that will be associated with the swapchain.
	 * \param _pPresentModeCount A pointer to an integer related to the number of presentation modes available or queried, as described below.
	 * \param _pPresentModes Either NULL or a pointer to an array of VkPresentModeKHR values, indicating the supported presentation modes
	 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_SURFACE_LOST_KHR.
	 **/
	CVulkan::PFN_vkGetPhysicalDeviceSurfacePresentModesKHR CVulkan::m_pfGetPhysicalDeviceSurfacePresentModesKHR = nullptr;

	/**
	 * Query surface capabilities.
	 * 
	 * \param _physicalDevice The physical device that will be associated with the swapchain to be created, as described for vkCreateSwapchainKHR.
	 * \param _surface The surface that will be associated with the swapchain.
	 * \param _pSurfaceCapabilities A pointer to a VkSurfaceCapabilitiesKHR structure in which the capabilities are returned.
	 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_SURFACE_LOST_KHR.
	 **/
	CVulkan::PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR CVulkan::m_pfGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
#endif	// #ifdef LSN_WINDOWS

	/**
	 * Returns up to requested number of global extension properties.
	 * 
	 * \param _pLayerName Either NULL or a pointer to a null-terminated UTF-8 string naming the layer to retrieve extensions from.
	 * \param _pPropertyCount A pointer to an integer related to the number of extension properties available or queried.
	 * \param _pProperties Either NULL or a pointer to an array of VkExtensionProperties structures.
	 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_LAYER_NOT_PRESENT.
	 **/
	CVulkan::PFN_vkEnumerateInstanceExtensionProperties CVulkan::m_pfEnumerateInstanceExtensionProperties = nullptr;

	/**
	 * Returns properties of available physical device extensions.
	 * 
	 * \param _physicalDevice The physical device that will be queried.
	 * \param _pLayerName Either NULL or a pointer to a null-terminated UTF-8 string naming the layer to retrieve extensions from.
	 * \param _pPropertyCount A pointer to an integer related to the number of extension properties available or queried, and is treated in the same fashion as the vkEnumerateInstanceExtensionProperties::pPropertyCount parameter.
	 * \param _pProperties Either NULL or a pointer to an array of VkExtensionProperties structures.
	 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_LAYER_NOT_PRESENT.
	 **/
	CVulkan::PFN_vkEnumerateDeviceExtensionProperties CVulkan::m_pfEnumerateDeviceExtensionProperties = nullptr;

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

		LSN_LOAD_FUNC( EnumerateInstanceExtensionProperties );
		LSN_LOAD_FUNC( EnumerateDeviceExtensionProperties );

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