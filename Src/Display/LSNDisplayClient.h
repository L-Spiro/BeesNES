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
			m_pofOutFormat( LSN_POF_RGB ),
			m_stRenderTargetStride( 0 ) {
		}
		virtual ~CDisplayClient();


		// == Enumerations.
		/** The output format. */
		enum LSN_PPU_OUT_FORMAT {
			LSN_POF_RGB,						/**< Output 24-bit RGB pixels. */
			LSN_POF_9BIT_PALETTE,				/**< Output 9-bit pallete entries. */
			LSN_POF_6BIT_PALETTE,				/**< Output 6-bit palette entries (0-63, 0x00-0x3F). */
		};


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
		 * Gets the display ratio in pixels.
		 *
		 * \return Returns the ratio of the display area.
		 */
		virtual double							DisplayRatio() const = 0 { return 1.0; }

		/**
		 * Sets the render target.
		 *
		 * \param _pui8Target Pointer to the pixel buffer to be used as a render target.  This is filled in during frame rendering.
		 * \param _stStride The stride of each row in the render target.
		 * \param _pofFormat The output format from the PPU.
		 */
		virtual void							SetRenderTarget( uint8_t * _pui8Target, size_t _stStride, LSN_PPU_OUT_FORMAT _pofFormat ) {
			m_pui8RenderTarget = _pui8Target;
			m_stRenderTargetStride = _stStride;
			m_pofOutFormat = _pofFormat;
		}

		/**
		 * Gets the frame count.
		 *
		 * \return Returns the frame count.
		 */
		virtual uint64_t						FrameCount() const = 0 { return 0; }

		/**
		 * Gets the PPU region.
		 *
		 * \return Returns the PPU region.
		 */
		virtual LSN_PPU_METRICS					PpuRegion() const = 0 { return LSN_PM_NTSC; }

		/**
		 * If true, extra room is added to the side of the view to display some debug information.
		 *
		 * \return Returns true in order to add an extra 128 pixels horizontally for debug display, otherwise false.  Defaults to false.
		 */
		virtual bool							DebugSideDisplay() const { return false; }


	protected :
		// == Members.
		/** The host pointr. */
		CDisplayHost *							m_pdhHost;
		/** The render target. */
		uint8_t *								m_pui8RenderTarget;
		/** The output format. */
		LSN_PPU_OUT_FORMAT						m_pofOutFormat;
		/** The render target row stride. */
		size_t									m_stRenderTargetStride;
		
	};

}	// namespace lsn
