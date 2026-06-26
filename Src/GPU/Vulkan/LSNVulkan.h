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
