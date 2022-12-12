/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class to allow communication with a CDisplayHost (the rendering window).
 */


#pragma once

#include "../LSNLSpiroNes.h"

namespace lsn {

	class CDisplayHost;

	/**
	 * Class CDisplayClient
	 * \brief The base class to allow communication with a CDisplayHost (the rendering window).
	 *
	 * Description: The base class to allow communication with a CDisplayHost (the rendering window).
	 */
	class CDisplayClient {
	public :
		// == Various constructors.
		CDisplayClient() :
			m_pdhHost( nullptr ),
			m_pui8RenderTarget( nullptr ),
			m_stRenderTargetStride( 0 ) {
		}
		virtual ~CDisplayClient();


		// == Functions.
		/**
		 * Sets the CDisplayHost pointer.  Allows this object to send message to the host.
		 *
		 * \param _pdhHost The point pointer.
		 */
		void									SetDisplayHost( CDisplayHost * _pdhHost ) { m_pdhHost = _pdhHost; }

		/**
		 * Detatches from the display host.
		 */
		virtual void							DetatchFromDisplayHost() {
			m_pdhHost = nullptr;
			m_pui8RenderTarget = nullptr;
			m_stRenderTargetStride = 0;
		}

		/**
		 * Gets the display width in pixels.  Used to create render targets.
		 *
		 * \return Returns the pixel width of the display area.
		 */
		virtual uint32_t						DisplayWidth() const = 0 { return 0; }

		/**
		 * Gets the display height in pixels.  Used to create render targets.
		 *
		 * \return Returns the pixel height of the display area.
		 */
		virtual uint32_t						DisplayHeight() const = 0 { return 0; }

		/**
		 * Sets the render target.
		 *
		 * \param _pui8Target Pointer to the pixel buffer to be used as a render target.  This is filled in during frame rendering.
		 * \param _stStride The stride of each row in the render target.
		 */
		virtual void							SetRenderTarget( uint8_t * _pui8Target, size_t _stStride ) {
			m_pui8RenderTarget = _pui8Target;
			m_stRenderTargetStride = _stStride;
		}


	protected :
		// == Members.
		/** The host pointr. */
		CDisplayHost *							m_pdhHost;
		/** The render target. */
		uint8_t *								m_pui8RenderTarget;
		/** The render target row stride. */
		size_t									m_stRenderTargetStride;
	};

}	// namespace lsn
