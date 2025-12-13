/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The DirectX 12 GPU object.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanDevice.h"

namespace lsn {

	CVulkanDevice::CVulkanDevice() :
		m_qSwapQueue( nullptr ) {
	}
	CVulkanDevice::~CVulkanDevice() {
		Reset();
	}

	// == Functions.
	/**
	 * Creates a Vulkan 1 device.
	 *
	 * \param _hWnd The window to which to attach.
	 * \param _sAdapter The adapter to use.
	 * \return Returns true if the device was created.
	 **/
	bool CVulkanDevice::Create( HWND _hWnd, const std::string &_sAdapter ) {
		Reset();

		lsw::LSW_HMODULE hLib;
		if ( !CVulkan::LoadVulkan( hLib ) ) { return false; }

		const std::vector<const char*> vInstanceExtensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			//VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME  // If using debug utils
		};
		if ( !CVulkan::CheckInstanceExtensionSupport( vInstanceExtensions ) ) {
			return false;
		}


		// Initialize Vulkan.
		VkApplicationInfo aiAppInfo = CVulkan::CreateApplicationInfo();

		VkInstanceCreateInfo iciCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &aiAppInfo,
			.enabledExtensionCount = static_cast<uint32_t>(vInstanceExtensions.size()),
			.ppEnabledExtensionNames = vInstanceExtensions.data()
		};

		CVulkan::LSN_INSTANCE iInstance( &iciCreateInfo );
		if ( !iInstance.Valid() ) { return false; }

		
		VkPhysicalDevice pdPhysicalDevice = VK_NULL_HANDLE;
		try {
			const std::vector<const char *> vDeviceExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			// Enumerate physical devices.
			uint32_t ui32DeviceCount = 0;
			CVulkan::m_pfEnumeratePhysicalDevices( iInstance.iInstance, &ui32DeviceCount, nullptr );
			if ( !ui32DeviceCount ) { return false; }

			std::vector<VkPhysicalDevice> dDevices( ui32DeviceCount );
			CVulkan::m_pfEnumeratePhysicalDevices( iInstance.iInstance, &ui32DeviceCount, dDevices.data() );

			for ( const auto & dThisDevice : dDevices ) {
				VkPhysicalDeviceProperties deviceProperties;
				CVulkan::m_pfGetPhysicalDeviceProperties( dThisDevice, &deviceProperties );

				if ( _sAdapter.empty() || _sAdapter == std::string( deviceProperties.deviceName ) ) {
					if ( !CVulkan::CheckDeviceExtensionSupport( dThisDevice, vDeviceExtensions ) ) {
						continue;
					}
					pdPhysicalDevice = dThisDevice;
					break;
				}
			}

			if ( pdPhysicalDevice == VK_NULL_HANDLE ) { return false; }
			

			// Create logical device and queues.
			float fQueuePriority[] = { 1.0f };
			VkDeviceQueueCreateInfo dqciQueueCreateInfo = {};
			dqciQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			dqciQueueCreateInfo.queueFamilyIndex = 0;
			dqciQueueCreateInfo.queueCount = std::size( fQueuePriority );
			dqciQueueCreateInfo.pQueuePriorities = fQueuePriority;

			VkDeviceCreateInfo dciDeviceCreateInfo = {};
			dciDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			dciDeviceCreateInfo.queueCreateInfoCount = 1;
			dciDeviceCreateInfo.pQueueCreateInfos = &dqciQueueCreateInfo;
			dciDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(vDeviceExtensions.size());
			dciDeviceCreateInfo.ppEnabledExtensionNames = vDeviceExtensions.data();

			
			CVulkan::LSN_DEVICE dDevice( pdPhysicalDevice, &dciDeviceCreateInfo );
			if ( !dDevice.Valid() ) { return false; }
			VkQueue qGraphicsQueue;
			CVulkan::m_pfGetDeviceQueue( dDevice.dDevice, 0, 0, &qGraphicsQueue );

			// Create swap chain.
			VkWin32SurfaceCreateInfoKHR wsciSurfaceCreateInfo = {};
			wsciSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			wsciSurfaceCreateInfo.hwnd = _hWnd;
			wsciSurfaceCreateInfo.hinstance = ::GetModuleHandleW( NULL );

			CVulkan::LSN_SURFACE sSurface( iInstance.iInstance, &wsciSurfaceCreateInfo );
			if ( !sSurface.Valid() ) { return false; }

			// Get client rect size.
			RECT rClient;
			::GetClientRect( _hWnd, &rClient );


			// Create the swap-chain.
			// What format should it be?
			VkFormat fSwapFormat = CVulkan::GetBackSurfaceFormat( pdPhysicalDevice, sSurface.sSurface );
			if ( fSwapFormat == VK_FORMAT_UNDEFINED ) { return false; }

			// Surface capabilities.
			VkSurfaceCapabilitiesKHR scSurfaceCapabilities;
			CVulkan::m_pfGetPhysicalDeviceSurfaceCapabilitiesKHR( pdPhysicalDevice, sSurface.sSurface,
				&scSurfaceCapabilities );
			/*if ( PrintVulkanError( "CVulkan::InitVulkan(): ",
				::vkGetPhysicalDeviceSurfaceCapabilitiesKHR( pdPhysicalDevice, m_sSurface,
				&scSurfaceCapabilities ) ) ) { return false; }*/

			// Present mode.
			VkPresentModeKHR pmPresentMode = CVulkan::GetPresentMode( pdPhysicalDevice, sSurface.sSurface );

			// Number of back-buffer images.
			uint32_t ui32SwapImages = scSurfaceCapabilities.minImageCount + 1;
			if ( (scSurfaceCapabilities.maxImageCount > 0) &&
				(ui32SwapImages > scSurfaceCapabilities.maxImageCount) ) {
				ui32SwapImages = scSurfaceCapabilities.maxImageCount;
			}

			// Surface transform.
			VkSurfaceTransformFlagBitsKHR stfbTrans;
			if ( scSurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ) {
				stfbTrans = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			}
			else {
				stfbTrans = scSurfaceCapabilities.currentTransform;
			}

			/*VkSwapchainCreateInfoKHR sciSwapChainCreateInfo = {};
			sciSwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			sciSwapChainCreateInfo.surface = sSurface.sSurface;
			sciSwapChainCreateInfo.minImageCount = 2;
			sciSwapChainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
			sciSwapChainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			sciSwapChainCreateInfo.imageExtent = { static_cast<UINT>(rClient.right - rClient.left), static_cast<UINT>(rClient.bottom - rClient.top) };
			sciSwapChainCreateInfo.imageArrayLayers = 1;
			sciSwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;*/

			VkSwapchainCreateInfoKHR sciSwapChainCreateInfo = {};
			sciSwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			sciSwapChainCreateInfo.pNext = NULL;
			sciSwapChainCreateInfo.surface = sSurface.sSurface;
			sciSwapChainCreateInfo.minImageCount = 2;
			sciSwapChainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
			sciSwapChainCreateInfo.imageExtent.width = static_cast<UINT>(rClient.right - rClient.left);
			sciSwapChainCreateInfo.imageExtent.height = static_cast<UINT>(rClient.bottom - rClient.top);
			sciSwapChainCreateInfo.preTransform = stfbTrans;
			sciSwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			sciSwapChainCreateInfo.imageArrayLayers = 1;
			sciSwapChainCreateInfo.presentMode = pmPresentMode;
			sciSwapChainCreateInfo.oldSwapchain = NULL;
			sciSwapChainCreateInfo.clipped = true;
			sciSwapChainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
			sciSwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			sciSwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			sciSwapChainCreateInfo.queueFamilyIndexCount = 0;
			sciSwapChainCreateInfo.pQueueFamilyIndices = NULL;

			uint32_t ui32QueueFamilyIndices[] = { 0 };
			sciSwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			sciSwapChainCreateInfo.queueFamilyIndexCount = 1;
			sciSwapChainCreateInfo.pQueueFamilyIndices = ui32QueueFamilyIndices;
			sciSwapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			sciSwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			sciSwapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
			sciSwapChainCreateInfo.clipped = VK_TRUE;

			CVulkan::LSN_SWAPCHAIN sSwapChain( dDevice.dDevice, &sciSwapChainCreateInfo );
			if ( !sSwapChain.Valid() ) { return false; }

			m_qSwapQueue = qGraphicsQueue;
			m_sSwapchain = std::move( sSwapChain );
			m_iInstance = std::move( iInstance );
			m_dDevice = std::move( dDevice );
			m_sBackbuffer = std::move( sSurface );
			m_hLib = std::move( hLib );
			return true;
		}
		catch ( ... ) { return false; }

		return false;
	}

	/**
	 * Frees all resources used by this object and leaves the object in a valid reusable state.
	 **/
	void CVulkanDevice::Reset() {
		m_qSwapQueue = nullptr;
		m_sSwapchain.Reset();
		m_sBackbuffer.Reset();

		m_dDevice.Reset();
		m_iInstance.Reset();
		
		m_hLib.Reset();
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
