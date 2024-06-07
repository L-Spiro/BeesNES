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
		typedef VkResult (VKAPI_PTR *							PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)( VkPhysicalDevice, VkSurfaceKHR, uint32_t *, VkPresentModeKHR * );
		typedef VkResult (VKAPI_PTR *							PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)( VkPhysicalDevice, VkSurfaceKHR, VkSurfaceCapabilitiesKHR * );


		/** A Vulkan VkDevice wrapper. */
		struct LSN_DEVICE {
			inline LSN_DEVICE() :
				dDevice( nullptr ),
				rRes( VK_ERROR_INITIALIZATION_FAILED ),
				pacAllocCallbacks( nullptr ) {
			}
			inline LSN_DEVICE( VkPhysicalDevice _pdPhysicalDevice, const VkDeviceCreateInfo * _pdciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
				rRes( CVulkan::m_pfCreateDevice( _pdPhysicalDevice, _pdciCreateInfo, _pacAllocator, &dDevice ) ),
				pacAllocCallbacks( _pacAllocator ) {
			}
			inline ~LSN_DEVICE() {
				Reset();
			}


			// == Operators.
			/**
			 * Moves from _iOther to this object.  _iOther is left in a valid state and could be reused.
			 * 
			 * \param _iOther The object to move.
			 * \return Returns this object after the move.
			 **/
			LSN_DEVICE &										operator = ( LSN_DEVICE &&_iOther ) noexcept {
				if ( this != &_iOther ) {
					Reset();
					rRes = _iOther.rRes;
					dDevice = _iOther.dDevice;
					pacAllocCallbacks = _iOther.pacAllocCallbacks;

					_iOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
					_iOther.dDevice = nullptr;
					_iOther.pacAllocCallbacks = nullptr;
				}
				return (*this);
			};


			// == Functions.
			/**
			 * Create a new device instance.
			 * 
			 * \param _piciCreateInfo A pointer to a VkInstanceCreateInfo structure controlling creation of the instance.
			 * \param _pacAllocator Controls host memory allocation.
			 * \return Returns true if vkCreateInstance() returns VK_SUCCESS.
			 **/
			inline bool											Create( VkPhysicalDevice _pdPhysicalDevice, const VkDeviceCreateInfo * _pdciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
				Reset();
				rRes = CVulkan::m_pfCreateDevice( _pdPhysicalDevice, _pdciCreateInfo, _pacAllocator, &dDevice );
				pacAllocCallbacks = _pacAllocator;
				return Valid();
			}

			/**
			 * Determines if the instance is valid.
			 *
			 * \return Returns true if the initial call to vkCreateInstance() was successful.
			 **/
			inline bool											Valid() const { return VK_SUCCESS == rRes; }

			/**
			 * Resets the object to scratch.
			 **/
			inline void											Reset() {
				if ( Valid() ) {
					rRes = VK_ERROR_INITIALIZATION_FAILED;
					CVulkan::m_pfDestroyDevice( dDevice, pacAllocCallbacks );
					pacAllocCallbacks = nullptr;
					dDevice = nullptr;
				}
			}

			/**
			 * Gets the return code after creation.
			 *
			 * \return Returns the result of creation.
			 **/
			inline VkResult										Result() const { return rRes; }


			// == Members.
			VkDevice											dDevice					= nullptr;


		private :
			VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
			const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
		};

		/** A Vulkan VkInstance wrapper. */
		struct LSN_INSTANCE {
			inline LSN_INSTANCE() :
				iInstance( nullptr ),
				rRes( VK_ERROR_INITIALIZATION_FAILED ),
				pacAllocCallbacks( nullptr ) {
			}
			inline LSN_INSTANCE( const VkInstanceCreateInfo * _piciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
				rRes( CVulkan::m_pfCreateInstance( _piciCreateInfo, _pacAllocator, &iInstance ) ),
				pacAllocCallbacks( _pacAllocator ) {
			}
			inline ~LSN_INSTANCE() {
				Reset();
			}


			// == Operators.
			/**
			 * Moves from _iOther to this object.  _iOther is left in a valid state and could be reused.
			 * 
			 * \param _iOther The object to move.
			 * \return Returns this object after the move.
			 **/
			LSN_INSTANCE &										operator = ( LSN_INSTANCE &&_iOther ) noexcept {
				if ( this != &_iOther ) {
					Reset();
					rRes = _iOther.rRes;
					iInstance = _iOther.iInstance;
					pacAllocCallbacks = _iOther.pacAllocCallbacks;

					_iOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
					_iOther.iInstance = nullptr;
					_iOther.pacAllocCallbacks = nullptr;
				}
				return (*this);
			};


			// == Functions.
			/**
			 * Create a new Vulkan instance.
			 * 
			 * \param _piciCreateInfo A pointer to a VkInstanceCreateInfo structure controlling creation of the instance.
			 * \param _pacAllocator Controls host memory allocation.
			 * \return Returns true if vkCreateInstance() returns VK_SUCCESS.
			 **/
			inline bool											Create( const VkInstanceCreateInfo * _piciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
				Reset();
				rRes = CVulkan::m_pfCreateInstance( _piciCreateInfo, _pacAllocator, &iInstance );
				pacAllocCallbacks = _pacAllocator;
				return Valid();
			}

			/**
			 * Determines if the instance is valid.
			 *
			 * \return Returns true if the initial call to vkCreateInstance() was successful.
			 **/
			inline bool											Valid() const { return VK_SUCCESS == rRes; }

			/**
			 * Resets the object to scratch.
			 **/
			inline void											Reset() {
				if ( Valid() ) {
					rRes = VK_ERROR_INITIALIZATION_FAILED;
					CVulkan::m_pfDestroyInstance( iInstance, pacAllocCallbacks );
					pacAllocCallbacks = nullptr;
					iInstance = nullptr;
				}
			}

			/**
			 * Gets the return code after creation.
			 *
			 * \return Returns the result of creation.
			 **/
			inline VkResult										Result() const { return rRes; }


			// == Members.
			VkInstance											iInstance				= nullptr;


		private :
			VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
			const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
		};

		/** A Vulkan VkSurfaceKHR wrapper. */
		struct LSN_SURFACE {
			inline LSN_SURFACE() :
				sSurface( 0 ),
				iInstance( nullptr ),
				rRes( VK_ERROR_INITIALIZATION_FAILED ),
				pacAllocCallbacks( nullptr ) {
			}
#ifdef LSN_WINDOWS
			inline LSN_SURFACE( VkInstance _iInstance, const VkWin32SurfaceCreateInfoKHR * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
				rRes( CVulkan::m_pfCreateWin32SurfaceKHR( _iInstance, _psciCreateInfo, _pacAllocator, &sSurface ) ),
				iInstance( _iInstance ),
				pacAllocCallbacks( _pacAllocator ) {
			}
#endif	// #ifdef LSN_WINDOWS
			inline ~LSN_SURFACE() {
				Reset();
			}


			// == Operators.
			/**
			 * Moves from _iOther to this object.  _iOther is left in a valid state and could be reused.
			 * 
			 * \param _iOther The object to move.
			 * \return Returns this object after the move.
			 **/
			LSN_SURFACE &										operator = ( LSN_SURFACE &&_iOther ) noexcept {
				if ( this != &_iOther ) {
					Reset();
					rRes = _iOther.rRes;
					sSurface = _iOther.sSurface;
					iInstance = _iOther.iInstance;
					pacAllocCallbacks = _iOther.pacAllocCallbacks;

					_iOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
					_iOther.sSurface = 0;
					_iOther.iInstance = nullptr;
					_iOther.pacAllocCallbacks = nullptr;
				}
				return (*this);
			};


			// == Functions.
			/**
			 * Create a new Vulkan surface.
			 * 
			 * \param _iInstance The instance to associate the surface with.
			 * \param _psciCreateInfo A pointer to a VkWin32SurfaceCreateInfoKHR structure containing parameters affecting the creation of the surface object.
			 * \param _pacAllocator The allocator used for host memory allocated for the surface object when there is no more specific allocator available.
			 * \return Returns true if vkCreateWin32SurfaceKHR() returns VK_SUCCESS.
			 **/
			inline bool											Create( VkInstance _iInstance, const VkWin32SurfaceCreateInfoKHR * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
				Reset();
				rRes = CVulkan::m_pfCreateWin32SurfaceKHR( _iInstance, _psciCreateInfo, _pacAllocator, &sSurface );
				iInstance = _iInstance;
				pacAllocCallbacks = _pacAllocator;
				return Valid();
			}

			/**
			 * Determines if the instance is valid.
			 *
			 * \return Returns true if the initial call to vkCreateInstance() was successful.
			 **/
			inline bool											Valid() const { return VK_SUCCESS == rRes; }

			/**
			 * Resets the object to scratch.
			 **/
			inline void											Reset() {
				if ( Valid() ) {
					rRes = VK_ERROR_INITIALIZATION_FAILED;
					CVulkan::m_pfDestroySurfaceKHR( iInstance, sSurface, pacAllocCallbacks );
					iInstance = nullptr;
					pacAllocCallbacks = nullptr;
					sSurface = 0;
				}
			}

			/**
			 * Gets the return code after creation.
			 *
			 * \return Returns the result of creation.
			 **/
			inline VkResult										Result() const { return rRes; }


			// == Members.
			VkSurfaceKHR										sSurface				= 0;


		private :
			VkInstance											iInstance				= nullptr;
			VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
			const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
		};

		/** A Vulkan VkSwapchainKHR wrapper. */
		struct LSN_SWAPCHAIN {
			inline LSN_SWAPCHAIN() :
				sSwapchain( 0 ),
				dDevice( nullptr ),
				rRes( VK_ERROR_INITIALIZATION_FAILED ),
				pacAllocCallbacks( nullptr ) {
			}
#ifdef LSN_WINDOWS
			inline LSN_SWAPCHAIN( VkDevice _dDevice, const VkSwapchainCreateInfoKHR * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) :
				rRes( CVulkan::m_pfCreateSwapchainKHR( _dDevice, _psciCreateInfo, _pacAllocator, &sSwapchain ) ),
				dDevice( _dDevice ),
				pacAllocCallbacks( _pacAllocator ) {
			}
#endif	// #ifdef LSN_WINDOWS
			inline ~LSN_SWAPCHAIN() {
				Reset();
			}


			// == Operators.
			/**
			 * Moves from _iOther to this object.  _iOther is left in a valid state and could be reused.
			 * 
			 * \param _iOther The object to move.
			 * \return Returns this object after the move.
			 **/
			LSN_SWAPCHAIN &										operator = ( LSN_SWAPCHAIN &&_iOther ) noexcept {
				if ( this != &_iOther ) {
					Reset();
					rRes = _iOther.rRes;
					sSwapchain = _iOther.sSwapchain;
					dDevice = _iOther.dDevice;
					pacAllocCallbacks = _iOther.pacAllocCallbacks;

					_iOther.rRes = VK_ERROR_INITIALIZATION_FAILED;
					_iOther.sSwapchain = 0;
					_iOther.dDevice = nullptr;
					_iOther.pacAllocCallbacks = nullptr;
				}
				return (*this);
			};


			// == Functions.
			/**
			 * Create a new Vulkan swapchain.
			 * 
			 * \param _iInstance The device to create the swapchain for.
			 * \param _psciCreateInfo A pointer to a VkSwapchainCreateInfoKHR structure specifying the parameters of the created swapchain.
			 * \param _pacAllocator The allocator used for host memory allocated for the swapchain object when there is no more specific allocator available.
			 * \return Returns true if vkCreateSwapchainKHR() returns VK_SUCCESS.
			 **/
			inline bool											Create( VkDevice _dDevice, const VkSwapchainCreateInfoKHR * _psciCreateInfo, const VkAllocationCallbacks * _pacAllocator = nullptr ) {
				Reset();
				rRes = CVulkan::m_pfCreateSwapchainKHR( _dDevice, _psciCreateInfo, _pacAllocator, &sSwapchain );
				dDevice = _dDevice;
				pacAllocCallbacks = _pacAllocator;
				return Valid();
			}

			/**
			 * Determines if the instance is valid.
			 *
			 * \return Returns true if the initial call to vkCreateInstance() was successful.
			 **/
			inline bool											Valid() const { return VK_SUCCESS == rRes; }

			/**
			 * Resets the object to scratch.
			 **/
			inline void											Reset() {
				if ( Valid() ) {
					rRes = VK_ERROR_INITIALIZATION_FAILED;
					CVulkan::m_pfDestroySwapchainKHR( dDevice, sSwapchain, pacAllocCallbacks );
					dDevice = nullptr;
					pacAllocCallbacks = nullptr;
					sSwapchain = 0;
				}
			}

			/**
			 * Gets the return code after creation.
			 *
			 * \return Returns the result of creation.
			 **/
			inline VkResult										Result() const { return rRes; }


			// == Members.
			VkSwapchainKHR										sSwapchain				= 0;


		private :
			VkDevice											dDevice					= nullptr;
			VkResult											rRes					= VK_ERROR_INITIALIZATION_FAILED;
			const VkAllocationCallbacks *						pacAllocCallbacks		= nullptr;
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


		/**
		 * Create a new device instance.
		 * 
		 * \param _physicalDevice Must be one of the device handles returned from a call to vkEnumeratePhysicalDevices.
		 * \param _pCreateInfo A pointer to a VkDeviceCreateInfo structure containing information about how to create the device.
		 * \param _pAllocator Controls host memory allocation.
		 * \param _pDevice A pointer to a handle in which the created VkDevice is returned.
		 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_INITIALIZATION_FAILED, VK_ERROR_EXTENSION_NOT_PRESENT, VK_ERROR_FEATURE_NOT_PRESENT, VK_ERROR_TOO_MANY_OBJECTS, VK_ERROR_DEVICE_LOST.
		 **/
		static PFN_vkCreateDevice								m_pfCreateDevice;

		/**
		 * Destroy a logical device.
		 * 
		 * \param _device The logical device to destroy.
		 * \param _pAllocator Controls host memory allocation.
		 **/
		static PFN_vkDestroyDevice								m_pfDestroyDevice;

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
		 * Get a queue handle from a device.
		 * 
		 * \param _device The logical device that owns the queue.
		 * \param _queueFamilyIndex The index of the queue family to which the queue belongs.
		 * \param _queueIndex The index within this queue family of the queue to retrieve.
		 * \param _pQueue A pointer to a VkQueue object that will be filled with the handle for the requested queue.
		 **/
		static PFN_vkGetDeviceQueue								m_pfGetDeviceQueue;

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
		static PFN_vkCreateWin32SurfaceKHR						m_pfCreateWin32SurfaceKHR;

		/**
		 * Destroy a VkSurfaceKHR object.
		 * 
		 * \param _instance The instance used to create the surface.
		 * \param _surface The surface to destroy.
		 * \param _pAllocator The allocator used for host memory allocated for the surface object when there is no more specific allocator available.
		 **/
		static PFN_vkDestroySurfaceKHR							m_pfDestroySurfaceKHR;

		/**
		 * Create a swapchain.
		 * 
		 * \param _device The device to create the swapchain for.
		 * \param _pCreateInfo A pointer to a VkSwapchainCreateInfoKHR structure specifying the parameters of the created swapchain.
		 * \param _pAllocator The allocator used for host memory allocated for the swapchain object when there is no more specific allocator available.
		 * \param _pSwapchain A pointer to a VkSwapchainKHR handle in which the created swapchain object will be returned.
		 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_DEVICE_LOST, VK_ERROR_SURFACE_LOST_KHR, VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, VK_ERROR_INITIALIZATION_FAILED, VK_ERROR_COMPRESSION_EXHAUSTED_EXT.
		 **/
		static PFN_vkCreateSwapchainKHR							m_pfCreateSwapchainKHR;

		/**
		 * Destroy a swapchain object.
		 * 
		 * \param _device The VkDevice associated with swapchain.
		 * \param _swapchain The swapchain to destroy.
		 * \param _pAllocator The allocator used for host memory allocated for the swapchain object when there is no more specific allocator available.
		 **/
		static PFN_vkDestroySwapchainKHR						m_pfDestroySwapchainKHR;

		/**
		 * Query color formats supported by surface.
		 * 
		 * \param _physicalDevice The physical device that will be associated with the swapchain to be created, as described for vkCreateSwapchainKHR.
		 * \param _surface The surface that will be associated with the swapchain.
		 * \param _pSurfaceFormatCount A pointer to an integer related to the number of format pairs available or queried.
		 * \param _pSurfaceFormats Either NULL or a pointer to an array of VkSurfaceFormatKHR structures.
		 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_SURFACE_LOST_KHR.
		 **/
		static PFN_vkGetPhysicalDeviceSurfaceFormatsKHR			m_pfGetPhysicalDeviceSurfaceFormatsKHR;

		/**
		 * Query supported presentation modes.
		 * 
		 * \param _physicalDevice The physical device that will be associated with the swapchain to be created, as described for vkCreateSwapchainKHR.
		 * \param _surface The surface that will be associated with the swapchain.
		 * \param _pPresentModeCount A pointer to an integer related to the number of presentation modes available or queried, as described below.
		 * \param _pPresentModes Either NULL or a pointer to an array of VkPresentModeKHR values, indicating the supported presentation modes
		 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_SURFACE_LOST_KHR.
		 **/
		static PFN_vkGetPhysicalDeviceSurfacePresentModesKHR	m_pfGetPhysicalDeviceSurfacePresentModesKHR;

		/**
		 * Query surface capabilities.
		 * 
		 * \param _physicalDevice The physical device that will be associated with the swapchain to be created, as described for vkCreateSwapchainKHR.
		 * \param _surface The surface that will be associated with the swapchain.
		 * \param _pSurfaceCapabilities A pointer to a VkSurfaceCapabilitiesKHR structure in which the capabilities are returned.
		 * \return On success, this command returns: VK_SUCCESS. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_SURFACE_LOST_KHR.
		 **/
		static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR	m_pfGetPhysicalDeviceSurfaceCapabilitiesKHR;
#endif	// #ifdef LSN_WINDOWS

		/**
		 * Returns up to requested number of global extension properties.
		 * 
		 * \param _pLayerName Either NULL or a pointer to a null-terminated UTF-8 string naming the layer to retrieve extensions from.
		 * \param _pPropertyCount A pointer to an integer related to the number of extension properties available or queried.
		 * \param _pProperties Either NULL or a pointer to an array of VkExtensionProperties structures.
		 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_LAYER_NOT_PRESENT.
		 **/
		static PFN_vkEnumerateInstanceExtensionProperties		m_pfEnumerateInstanceExtensionProperties;

		/**
		 * Returns properties of available physical device extensions.
		 * 
		 * \param _physicalDevice The physical device that will be queried.
		 * \param _pLayerName Either NULL or a pointer to a null-terminated UTF-8 string naming the layer to retrieve extensions from.
		 * \param _pPropertyCount A pointer to an integer related to the number of extension properties available or queried, and is treated in the same fashion as the vkEnumerateInstanceExtensionProperties::pPropertyCount parameter.
		 * \param _pProperties Either NULL or a pointer to an array of VkExtensionProperties structures.
		 * \return On success, this command returns: VK_SUCCESS, VK_INCOMPLETE. On failure, this command returns: VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_LAYER_NOT_PRESENT.
		 **/
		static PFN_vkEnumerateDeviceExtensionProperties			m_pfEnumerateDeviceExtensionProperties;


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
