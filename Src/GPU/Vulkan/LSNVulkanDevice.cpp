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

	CVulkanDevice::CVulkanDevice() {
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

		// Initialize Vulkan.
		VkApplicationInfo aiAppInfo = CVulkan::CreateApplicationInfo();

		VkInstanceCreateInfo iciCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &aiAppInfo
		};

		CVulkan::LSN_INSTANCE iInstance( &iciCreateInfo );
		if ( !iInstance.Valid() ) { return false; }

		
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		try {
			// Enumerate physical devices.
			uint32_t ui32DeviceCount = 0;
			CVulkan::m_pfEnumeratePhysicalDevices( iInstance.iInstance, &ui32DeviceCount, nullptr );
			if ( !ui32DeviceCount ) { return false; }

			std::vector<VkPhysicalDevice> dDevices( ui32DeviceCount );
			CVulkan::m_pfEnumeratePhysicalDevices( iInstance.iInstance, &ui32DeviceCount, dDevices.data() );

			for ( const auto & dDevice : dDevices ) {
				VkPhysicalDeviceProperties deviceProperties;
				CVulkan::m_pfGetPhysicalDeviceProperties( dDevice, &deviceProperties );

				if ( _sAdapter.empty() || _sAdapter == std::string( deviceProperties.deviceName ) ) {
					physicalDevice = dDevice;
					break;
				}
			}

			if ( physicalDevice == VK_NULL_HANDLE ) { return false; }


			// Create logical device and queues.
			float fQueuePriority = 1.0f;
			VkDeviceQueueCreateInfo dqciQueueCreateInfo = {};
			dqciQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			dqciQueueCreateInfo.queueFamilyIndex = 0;
			dqciQueueCreateInfo.queueCount = 1;
			dqciQueueCreateInfo.pQueuePriorities = &fQueuePriority;

			VkDeviceCreateInfo dciDeviceCreateInfo = {};
			dciDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			dciDeviceCreateInfo.queueCreateInfoCount = 1;
			dciDeviceCreateInfo.pQueueCreateInfos = &dqciQueueCreateInfo;

			CVulkan::LSN_DEVICE dDevice( physicalDevice, &dciDeviceCreateInfo );
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

			VkSwapchainCreateInfoKHR sciSwapChainCreateInfo = {};
			sciSwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			sciSwapChainCreateInfo.surface = sSurface.sSurface;
			sciSwapChainCreateInfo.minImageCount = 2;
			sciSwapChainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
			sciSwapChainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			sciSwapChainCreateInfo.imageExtent = { 800, 600 };
			sciSwapChainCreateInfo.imageArrayLayers = 1;
			sciSwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			uint32_t ui32QueueFamilyIndices[] = { 0 };
			sciSwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			sciSwapChainCreateInfo.queueFamilyIndexCount = 1;
			sciSwapChainCreateInfo.pQueueFamilyIndices = ui32QueueFamilyIndices;
			sciSwapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			sciSwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			sciSwapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
			sciSwapChainCreateInfo.clipped = VK_TRUE;

			//VkSwapchainKHR sSwapChain;
			CVulkan::LSN_SWAPCHAIN sSwapChain( dDevice.dDevice, &sciSwapChainCreateInfo );
			if ( !sSwapChain.Valid() ) { return false; }

			m_sSwapchain = std::move( sSwapChain );
			m_iInstance = std::move( iInstance );
			m_dDevice = std::move( dDevice );
			m_sBackbuffer = std::move( sSurface );

		}
		catch ( ... ) { return false; }

		return false;
	}

	/**
	 * Frees all resources used by this object and leaves the object in a valid reusable state.
	 **/
	void CVulkanDevice::Reset() {
		m_sSwapchain.Reset();
		m_sBackbuffer.Reset();

		m_iInstance.Reset();
		m_dDevice.Reset();
		m_hLib.Reset();
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
