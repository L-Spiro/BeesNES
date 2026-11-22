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

			// Enumerate physical devices.
			uint32_t ui32DeviceCount = 0;
			CVulkan::m_pfEnumeratePhysicalDevices( iInstance.iInstance, &ui32DeviceCount, nullptr );
			if ( !ui32DeviceCount ) { return false; }

			std::vector<VkPhysicalDevice> dDevices( ui32DeviceCount );
			CVulkan::m_pfEnumeratePhysicalDevices( iInstance.iInstance, &ui32DeviceCount, dDevices.data() );

			// Pick the device:  the named adapter if there is one, otherwise the best kind (discrete, then integrated, then virtual, then
			//	anything else, then CPU/software devices).  The first listed device used to be taken, which can be a software or translation-
			//	layer device (llvmpipe, the Vulkan-on-Direct3D 12 compatibility pack) that is far too slow or cannot present.  Each candidate
			//	must support the swap-chain extension and have a queue family that can both draw and present to this surface (family 0 was
			//	assumed before), and must offer an 8-bit sRGB surface format.
			auto aKindScore = []( VkPhysicalDeviceType _pdtType ) -> int32_t {
				switch ( _pdtType ) {
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU : { return 4; }
					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU : { return 3; }
					case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU : { return 2; }
					case VK_PHYSICAL_DEVICE_TYPE_CPU : { return 0; }
					default : { return 1; }
				}
			};
			int32_t i32BestScore = -1;
			uint32_t ui32BestFamily = 0;
			for ( const auto & dThisDevice : dDevices ) {
				VkPhysicalDeviceProperties deviceProperties;
				CVulkan::m_pfGetPhysicalDeviceProperties( dThisDevice, &deviceProperties );

				const bool bNamed = !_sAdapter.empty() && _sAdapter == std::string( deviceProperties.deviceName );
				if ( !_sAdapter.empty() && !bNamed ) { continue; }
				if ( !CVulkan::CheckDeviceExtensionSupport( dThisDevice, vDeviceExtensions ) ) { continue; }
				if ( CVulkan::GetBackSurfaceFormat( dThisDevice, sSurface.sSurface ) == VK_FORMAT_UNDEFINED ) { continue; }

				// A queue family that can draw and present.
				uint32_t ui32Families = 0;
				CVulkan::m_pfGetPhysicalDeviceQueueFamilyProperties( dThisDevice, &ui32Families, nullptr );
				std::vector<VkQueueFamilyProperties> vFamilies( ui32Families );
				if ( ui32Families ) { CVulkan::m_pfGetPhysicalDeviceQueueFamilyProperties( dThisDevice, &ui32Families, vFamilies.data() ); }
				uint32_t ui32Family = UINT32_MAX;
				for ( uint32_t I = 0; I < ui32Families; ++I ) {
					if ( !vFamilies[I].queueCount || !(vFamilies[I].queueFlags & VK_QUEUE_GRAPHICS_BIT) ) { continue; }
					VkBool32 bPresent = VK_FALSE;
					if ( CVulkan::m_pfGetPhysicalDeviceSurfaceSupportKHR( dThisDevice, I, sSurface.sSurface, &bPresent ) == VK_SUCCESS && bPresent ) {
						ui32Family = I;
						break;
					}
				}
				if ( ui32Family == UINT32_MAX ) { continue; }

				const int32_t i32Score = aKindScore( deviceProperties.deviceType );
				if ( i32Score > i32BestScore ) {
					i32BestScore = i32Score;
					pdPhysicalDevice = dThisDevice;
					ui32BestFamily = ui32Family;
				}
				if ( bNamed ) { break; }
			}

			if ( pdPhysicalDevice == VK_NULL_HANDLE ) { return false; }
			
			// Create logical device and queues.
			float fQueuePriority[] = { 1.0f };
			m_ui32QueueFamilyIndex = ui32BestFamily;
			
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
			}
			else {
				stfbTrans = scCapabilities.currentTransform;
			}

			// Resolution/Extent.
			VkExtent2D eExtent;
			if ( scCapabilities.currentExtent.width != 0xFFFFFFFF ) {
				eExtent = scCapabilities.currentExtent;
			}
			else {
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
			// Allow the images to be copied to the CPU (video capture) where supported.
			const bool bCanCopy = (scCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) != 0;
			if ( bCanCopy ) { sciSwapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT; }
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
			m_ui32SwapImages = ui32SwapImages;
			m_eSwapExtent = eExtent;
			m_bSwapCanCopy = bCanCopy;
			
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

		// A minimized window has no area, and a swap chain cannot be 0 pixels in size.  Keep the current one until the window has an
		//	area again (checked before flushing the queue, since this is retried every frame while minimized).
		RECT rClient;
		::GetClientRect( m_hWnd, &rClient );
		if ( rClient.right <= rClient.left || rClient.bottom <= rClient.top ) { return false; }

		if ( !FlushCommandQueue() ) { return false; }

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
		// Keep the image count Create() chose (minImageCount + 1), clamped to the current capabilities.  This used to drop to 2, and with
		//	only minImageCount images the presentation engine may hold all of them (one on screen, one queued), so every
		//	vkAcquireNextImageKHR() waits for the next composition.  That wait happens on the UI thread.
		uint32_t ui32Images = m_ui32SwapImages ? m_ui32SwapImages : (scCapabilities.minImageCount + 1);
		ui32Images = std::max<uint32_t>( ui32Images, scCapabilities.minImageCount );
		if ( scCapabilities.maxImageCount > 0 && ui32Images > scCapabilities.maxImageCount ) {
			ui32Images = scCapabilities.maxImageCount;
		}
		sciSwapChainCreateInfo.minImageCount = ui32Images;
		sciSwapChainCreateInfo.imageFormat = m_fSwapFormat;
		sciSwapChainCreateInfo.imageExtent.width = std::clamp( ui32Width, scCapabilities.minImageExtent.width, scCapabilities.maxImageExtent.width );
		sciSwapChainCreateInfo.imageExtent.height = std::clamp( ui32Height, scCapabilities.minImageExtent.height, scCapabilities.maxImageExtent.height );
		if ( !sciSwapChainCreateInfo.imageExtent.width || !sciSwapChainCreateInfo.imageExtent.height ) { return false; }
		sciSwapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		sciSwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		sciSwapChainCreateInfo.imageArrayLayers = 1;
		sciSwapChainCreateInfo.presentMode = m_pmPresentMode;
		sciSwapChainCreateInfo.clipped = VK_TRUE;
		sciSwapChainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		sciSwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		// Allow the images to be copied to the CPU (video capture) where supported.
		const bool bCanCopy = (scCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) != 0;
		if ( bCanCopy ) { sciSwapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT; }
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
		m_eSwapExtent = sciSwapChainCreateInfo.imageExtent;
		m_bSwapCanCopy = bCanCopy;

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
		m_ui32SwapImages = 0;
		m_eSwapExtent = { 0, 0 };
		m_bSwapCanCopy = false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
