/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Manages objects whose DirectX 9.0 resources can be lost (when the device is lost, for example).
 *	Every losable resource registers itself with this class automatically (using a small circular dependency).
 *	This class can then inform those resources when the device is lost and regained, allowing them to re-register
 *	their resources.
 */


#ifdef LSN_DX9

#pragma once

#include "../../LSNLSpiroNes.h"
#include "CriticalSection/LSWCriticalSection.h"
#include "LSNDirectX9LosableResource.h"

#include <vector>

namespace lsn {

	/**
	 * Class CDirectX9LosableResourceManager
	 * \brief Manages losable resources, notifying them when the device is lost and regained.
	 *
	 * Description: Manages objects whose DirectX 9.0 resources can be lost (when the device is lost, for example).
	 *	Every losable resource registers itself with this class automatically (using a small circular dependency).
	 *	This class can then inform those resources when the device is lost and regained, allowing them to re-register
	 *	their resources.
	 */
	class CDirectX9LosableResourceManager {
	public :
		// == Functions.
		/**
		 * Destroy the losable resource manager.  Should be called when shutting down.
		 */
		static void												Destroy();

		/**
		 * Register a resource (also gives the resource a unique ID).
		 *
		 * \param _plrRes The resource to register.  Losable resources call this on
		 *	themselves directly, so this function should never be called by the user.
		 * \return Returns false if a memory error occurred.  If false is returned, the
		 *	engine must shut down.
		 */
		static bool 											RegisterRes( CDirectX9LosableResource * _plrRes );

		/**
		 * Removes a resource by its ID.
		 *
		 * \param _ui32Id Unique ID of the resource to remove from the list.
		 */
		static void 											RemoveRes( uint32_t _ui32Id );

		/**
		 * Notify all objects that the device has been lost.
		 */
		static void 											OnLostDevice();

		/**
		 * Notify all objects that the device has been reset.
		 */
		static void 											OnResetDevice();

		/**
		 * Gets the number of losable resources currently registered.
		 *
		 * \return Returns the number of registered losable resources.
		 */
		static size_t 											GetLosableResCount();


	protected :
		// == Members.
		/** List of resources. */
		static std::vector<CDirectX9LosableResource *>			m_vResources;

		/** Unique resource ID. */
		static uint32_t											m_ui32ResId;

		/** Thread safety. */
		static lsw::CCriticalSection							m_csCrit;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
