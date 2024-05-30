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

		LSN_LOAD_FUNC( CreateInstance );
		LSN_LOAD_FUNC( DestroyInstance );
		LSN_LOAD_FUNC( EnumeratePhysicalDevices );
		LSN_LOAD_FUNC( GetPhysicalDeviceProperties );

#undef LSN_LOAD_FUNC
		
		return true;
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
