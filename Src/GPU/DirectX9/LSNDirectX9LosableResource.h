/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: All objects that have to take action when the DirectX device is lost should inherit
 *	from this class.  This is for DirectX 9.0 only.
 */


#ifdef LSN_DX9

#pragma once

#include "../../LSNLSpiroNes.h"

namespace lsn {

	/**
	 * Class CDirectX9LosableResource
	 * \brief Provides an interface for DirectX 9 objects that need to be reset when the device is lost.
	 *
	 * Description: All objects that have to take action when the DirectX device is lost should inherit
	 *	from this class.  This is for DirectX 9.0 only.
	 */
	class CDirectX9LosableResource {
		friend class											CDirectX9LosableResourceManager;
	public :
		// == Various constructors.
		CDirectX9LosableResource();
		~CDirectX9LosableResource();


		// == Functions.
		/**
		 * Must perform some action when the device is lost.
		 */
		virtual void											OnDeviceLost() = 0;

		/**
		 * Must renew resources when the device is reset.
		 *
		 * \return Return true if the renewal is successful, false otherwise.
		 */
		virtual bool											OnDeviceReset() = 0;


	protected :
		// == Members.
		/** Do we need to reset the resource? */
		bool													m_bResourceCanBeLost;

	private :
		/** An ID that lets us remove ourselves from the global resource list. */
		uint32_t												m_ui32UniqueLosableResourceId;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
