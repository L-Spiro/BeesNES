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
		typedef VkResult (VKAPI_PTR *							PFN_vkCreateInstance)( const VkInstanceCreateInfo *, const VkAllocationCallbacks *, VkInstance * );
		typedef void (VKAPI_PTR *								PFN_vkDestroyInstance)( VkInstance, const VkAllocationCallbacks * );
		typedef VkResult (VKAPI_PTR *							PFN_vkEnumeratePhysicalDevices)( VkInstance, uint32_t *, VkPhysicalDevice * );
		typedef void (VKAPI_PTR *								PFN_vkGetPhysicalDeviceProperties)( VkPhysicalDevice, VkPhysicalDeviceProperties * );

		/** A Vulkan instance wrapper. */
		struct LSN_INSTANCE {
			inline LSN_INSTANCE( const VkInstanceCreateInfo * _piciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
				rRes( CVulkan::m_pfCreateInstance( _piciCreateInfo, _pacAllocator, &iInstance ) ),
				pacAllocCallbacks( _pacAllocator ) {
			}
			inline ~LSN_INSTANCE() {
				if ( Valid() ) {
					rRes = VK_ERROR_INITIALIZATION_FAILED;
					CVulkan::m_pfDestroyInstance( iInstance, pacAllocCallbacks );
					pacAllocCallbacks = nullptr;
				}
			}


			// == Functions.
			/**
			 * Determines if the instance is valid.
			 *
			 * \return Returns true if the initial call to vkCreateInstance() was successful.
			 **/
			inline bool											Valid() const { return VK_SUCCESS == rRes; }


			// == Members.
			VkInstance iInstance								= { 0 };


		private :
			VkResult rRes										= VK_ERROR_INITIALIZATION_FAILED;
			const VkAllocationCallbacks * pacAllocCallbacks		= nullptr;
		};


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
		 * Create a new Vulkan instance.
		 * 
		 * \param _pCreateInfo A pointer to a VkInstanceCreateInfo structure controlling creation of the instance.
		 * \param _pAllocator Controls host memory allocation.
		 * \param _pInstance Points a VkInstance handle in which the resulting instance is returned.
		 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_INITIALIZATION_FAILED, VK_ERROR_LAYER_NOT_PRESENT, VK_ERROR_EXTENSION_NOT_PRESENT, VK_ERROR_INCOMPATIBLE_DRIVER
		 **/
		static PFN_vkCreateInstance								m_pfCreateInstance;

		/**
		 * Destroy an instance of Vulkan.
		 * 
		 * \param _instance The handle of the instance to destroy.
		 * \param _pAllocator Controls host memory allocation.
		 **/
		static PFN_vkDestroyInstance							m_pfDestroyInstance;

		/**
		 * Enumerates the physical devices accessible to a Vulkan instance.
		 * 
		 * \param _instance A handle to a Vulkan instance previously created with vkCreateInstance.
		 * \param _pPhysicalDeviceCount A pointer to an integer related to the number of physical devices available or queried.
		 * \param _pPhysicalDevices Either NULL or a pointer to an array of VkPhysicalDevice handles.
		 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_INITIALIZATION_FAILED.
		 **/
		static PFN_vkEnumeratePhysicalDevices					m_pfEnumeratePhysicalDevices;

		/**
		 * Returns properties of a physical device.
		 * 
		 * \param _physicalDevice The handle to the physical device whose properties will be queried.
		 * \param _pProperties A pointer to a VkPhysicalDeviceProperties structure in which properties are returned.
		 **/
		static PFN_vkGetPhysicalDeviceProperties				m_pfGetPhysicalDeviceProperties;

	protected :
		// == Members.
		static BOOL												m_bSupported;								/**< Is Vulkan 1.0 supported? */
		static std::vector<VkPhysicalDeviceProperties>			m_vDisplayDevices;							/**< The array of display devices. */
		static lsw::LSW_HMODULE									m_hDll;										/**< The vulkan-1.dll DLL. */


		// == Functions.
		/**
		 * Loads the vulkan-1.dll DLL.  If the DLL is not already loaded and loads successfully, the Vulkan function pointers are set.
		 *
		 * \param _hDll The HMODULE into which to load the DLL.
		 * \return Returns true if the DLL was loaded or is already loaded.
		 **/
		static bool												LoadVulkan( lsw::LSW_HMODULE &_hDll );

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
