/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Vulkan 1 GPU object.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanDevice.h"

namespace lsn {

	CVulkanDevice::CVulkanDevice() {}
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

#ifdef LSN_WINDOWS
		const std::vector<const char*> vInstanceExtensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			//VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		};
#elif defined( LSN_APPLE )
		const std::vector<const char*> vInstanceExtensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_EXT_METAL_SURFACE_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		};
#else
		const std::vector<const char*> vInstanceExtensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		};
#endif
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
			m_ui32QueueFamilyIndex = 0;
			
			VkDeviceQueueCreateInfo dqciQueueCreateInfo = {};
			dqciQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			dqciQueueCreateInfo.queueFamilyIndex = m_ui32QueueFamilyIndex;
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
			CVulkan::m_pfGetDeviceQueue( dDevice.dDevice, m_ui32QueueFamilyIndex, 0, &qGraphicsQueue );

			// Create surface.
#ifdef LSN_WINDOWS
			VkWin32SurfaceCreateInfoKHR sciSurfaceCreateInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
			sciSurfaceCreateInfo.hwnd = _hWnd;
			sciSurfaceCreateInfo.hinstance = ::GetModuleHandleW( NULL );
			CVulkan::LSN_SURFACE sSurface( iInstance.iInstance, &sciSurfaceCreateInfo );
#elif defined( LSN_APPLE )
			VkMetalSurfaceCreateInfoEXT sciSurfaceCreateInfo = { VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT };
			sciSurfaceCreateInfo.pLayer = _hWnd; // Opaque pointer evaluated as CAMetalLayer
			CVulkan::LSN_SURFACE sSurface( iInstance.iInstance, &sciSurfaceCreateInfo );
#else
			VkXlibSurfaceCreateInfoKHR sciSurfaceCreateInfo = { VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR };
			// Assign Xlib Display/Window parameters from _hWnd opaque pointer.
			CVulkan::LSN_SURFACE sSurface( iInstance.iInstance, &sciSurfaceCreateInfo );
#endif
			if ( !sSurface.Valid() ) { return false; }

			// Query capabilities.
			VkSurfaceCapabilitiesKHR scCapabilities;
			if ( CVulkan::m_pfGetPhysicalDeviceSurfaceCapabilitiesKHR( pdPhysicalDevice, sSurface.sSurface, &scCapabilities ) != VK_SUCCESS ) {
				return false;
			}

			// Format & Present Mode.
			VkFormat fSwapFormat = CVulkan::GetBackSurfaceFormat( pdPhysicalDevice, sSurface.sSurface );
			if ( fSwapFormat == VK_FORMAT_UNDEFINED ) { return false; }
			VkPresentModeKHR pmPresentMode = CVulkan::GetPresentMode( pdPhysicalDevice, sSurface.sSurface );

			// Number of back-buffer images.
			uint32_t ui32SwapImages = scCapabilities.minImageCount + 1;
			if ( scCapabilities.maxImageCount > 0 && ui32SwapImages > scCapabilities.maxImageCount ) {
				ui32SwapImages = scCapabilities.maxImageCount;
			}

			// Surface transform.
			VkSurfaceTransformFlagBitsKHR stfbTrans;
			if ( scCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ) {
				stfbTrans = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			} else {
				stfbTrans = scCapabilities.currentTransform;
			}

			// Resolution/Extent.
			VkExtent2D eExtent;
			if ( scCapabilities.currentExtent.width != 0xFFFFFFFF ) {
				eExtent = scCapabilities.currentExtent;
			} else {
				RECT rClient;
				::GetClientRect( _hWnd, &rClient );
				eExtent.width = std::clamp<uint32_t>( rClient.right - rClient.left, scCapabilities.minImageExtent.width, scCapabilities.maxImageExtent.width );
				eExtent.height = std::clamp<uint32_t>( rClient.bottom - rClient.top, scCapabilities.minImageExtent.height, scCapabilities.maxImageExtent.height );
			}

			// Create the Swapchain.
			VkSwapchainCreateInfoKHR sciSwapChainCreateInfo = {};
			sciSwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			sciSwapChainCreateInfo.pNext = NULL;
			sciSwapChainCreateInfo.surface = sSurface.sSurface;
			sciSwapChainCreateInfo.minImageCount = ui32SwapImages;
			sciSwapChainCreateInfo.imageFormat = fSwapFormat;
			sciSwapChainCreateInfo.imageExtent = eExtent;
			sciSwapChainCreateInfo.preTransform = stfbTrans;
			sciSwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			sciSwapChainCreateInfo.imageArrayLayers = 1;
			sciSwapChainCreateInfo.presentMode = pmPresentMode;
			sciSwapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
			sciSwapChainCreateInfo.clipped = VK_TRUE;
			sciSwapChainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			sciSwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			sciSwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			sciSwapChainCreateInfo.queueFamilyIndexCount = 1;
			sciSwapChainCreateInfo.pQueueFamilyIndices = &m_ui32QueueFamilyIndex;

			CVulkan::LSN_SWAPCHAIN sSwapChain( dDevice.dDevice, &sciSwapChainCreateInfo );
			if ( !sSwapChain.Valid() ) { return false; }

			// Create synchronization objects.
			VkFenceCreateInfo fciFenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			fciFenceCreateInfo.flags = 0; // Unsignaled state

			CVulkan::LSN_FENCE fFence( dDevice.dDevice, &fciFenceCreateInfo );
			if ( !fFence.Valid() ) { return false; }

			m_hWnd = _hWnd;
			m_pdPhysicalDevice = pdPhysicalDevice;
			m_fSwapFormat = fSwapFormat;
			m_pmPresentMode = pmPresentMode;
			
			m_fFence = std::move( fFence );
			m_qGraphicsQueue = qGraphicsQueue;
			m_sSwapchain = std::move( sSwapChain );
			m_dDevice = std::move( dDevice );
			m_sBackbuffer = std::move( sSurface );
			m_iInstance = std::move( iInstance );
			m_hLib = std::move( hLib );
			return true;
		}
		catch ( ... ) { return false; }

		return false;
	}

	/**
	 * Flushes the command queue, blocking the CPU until the GPU has finished all pending operations.
	 *
	 * \return Returns true if the flush was successful.
	 **/
	bool CVulkanDevice::FlushCommandQueue() {
		if ( !m_qGraphicsQueue || !m_fFence.Valid() ) { return false; }

		if ( CVulkan::m_pfResetFences( m_dDevice.dDevice, 1, &m_fFence.fFence ) != VK_SUCCESS ) { return false; }

		if ( CVulkan::m_pfQueueSubmit( m_qGraphicsQueue, 0, nullptr, m_fFence.fFence ) != VK_SUCCESS ) { return false; }

		if ( CVulkan::m_pfWaitForFences( m_dDevice.dDevice, 1, &m_fFence.fFence, VK_TRUE, UINT64_MAX ) != VK_SUCCESS ) { return false; }

		return true;
	}

	/**
	 * Resizes the swap chain backbuffers to match the current target window dimensions.
	 * Note: It is the caller's responsibility to release any active Render Target Views referencing the swap chain before calling this.
	 *
	 * \return Returns true if the swap chain was successfully resized.
	 **/
	bool CVulkanDevice::ResizeSwapChain() {
		if ( !m_sSwapchain.Valid() ) { return false; }

		if ( !FlushCommandQueue() ) { return false; }

		RECT rClient;
		::GetClientRect( m_hWnd, &rClient );

		VkSurfaceCapabilitiesKHR scCapabilities;
		if ( CVulkan::m_pfGetPhysicalDeviceSurfaceCapabilitiesKHR( m_pdPhysicalDevice, m_sBackbuffer.sSurface, &scCapabilities ) != VK_SUCCESS ) {
			return false;
		}

		uint32_t ui32Width = static_cast<uint32_t>(rClient.right - rClient.left);
		uint32_t ui32Height = static_cast<uint32_t>(rClient.bottom - rClient.top);

		VkSwapchainCreateInfoKHR sciSwapChainCreateInfo = {};
		sciSwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		sciSwapChainCreateInfo.pNext = NULL;
		sciSwapChainCreateInfo.surface = m_sBackbuffer.sSurface;
		// Maintain current image count or clamp to capabilities.
		sciSwapChainCreateInfo.minImageCount = 2;
		if ( scCapabilities.maxImageCount > 0 && sciSwapChainCreateInfo.minImageCount > scCapabilities.maxImageCount ) {
			sciSwapChainCreateInfo.minImageCount = scCapabilities.maxImageCount;
		}
		sciSwapChainCreateInfo.imageFormat = m_fSwapFormat;
		sciSwapChainCreateInfo.imageExtent.width = std::clamp( ui32Width, scCapabilities.minImageExtent.width, scCapabilities.maxImageExtent.width );
		sciSwapChainCreateInfo.imageExtent.height = std::clamp( ui32Height, scCapabilities.minImageExtent.height, scCapabilities.maxImageExtent.height );
		sciSwapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		sciSwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		sciSwapChainCreateInfo.imageArrayLayers = 1;
		sciSwapChainCreateInfo.presentMode = m_pmPresentMode;
		sciSwapChainCreateInfo.clipped = VK_TRUE;
		sciSwapChainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		sciSwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		sciSwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		sciSwapChainCreateInfo.queueFamilyIndexCount = 1;
		sciSwapChainCreateInfo.pQueueFamilyIndices = &m_ui32QueueFamilyIndex;
		
		// The new swapchain takes the old one as a reference so Vulkan can optimally reallocate.
		sciSwapChainCreateInfo.oldSwapchain = m_sSwapchain.sSwapchain;

		CVulkan::LSN_SWAPCHAIN sNewSwapchain( m_dDevice.dDevice, &sciSwapChainCreateInfo );
		if ( !sNewSwapchain.Valid() ) { return false; }

		// This assignment invokes the RAII move assignment operator, which properly 
		// destroys the *old* swapchain before taking ownership of the new one.
		m_sSwapchain = std::move( sNewSwapchain );

		return true;
	}

	/**
	 * Frees all resources used by this object and leaves the object in a valid reusable state.
	 **/
	void CVulkanDevice::Reset() {
		m_fFence.Reset();
		m_sSwapchain.Reset();
		m_sBackbuffer.Reset();
		m_dDevice.Reset();
		m_iInstance.Reset();
		
		m_hLib.Reset();

		m_pdPhysicalDevice = VK_NULL_HANDLE;
		m_qGraphicsQueue = VK_NULL_HANDLE;
		m_hWnd = NULL;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
