#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for Direct3D 9 filters.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNGpuFilterBase.h"

#include "../GPU/DirectX9/LSNDirectX9.h"
#include "../GPU/DirectX9/LSNDirectX9Device.h"
#include "../GPU/DirectX9/LSNDirectX9NesPresenter.h"

#include <vector>


namespace lsn {

	/**
	 * Class CDx9FilterBase
	 * \brief A standard 24-bit RGB filter.
	 *
	 * Description: A standard 24-bit RGB filter.
	 */
	class CDx9FilterBase : public CGpuFilterBase {
	public :
		CDx9FilterBase();
		virtual ~CDx9FilterBase();


		// == Functions.
		/**
		 * Sets the basic parameters for the filter.
		 *
		 * \param _stBuffers The number of render targets to create.
		 * \param _ui16Width The console screen width.  Typically 256.
		 * \param _ui16Height The console screen height.  Typically 240.
		 * \return Returns the input format requested of the PPU.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height );

		/**
		 * Called when the filter is about to become active.
		 */
		virtual void										Activate();

		/**
		 * Called when the filter is about to become inactive.
		 */
		virtual void										DeActivate();

		/**
		 * \brief Registers the DX9 child target window class (no background erase).
		 * 
		 * \return Returns true if the class is registered or already existed.
		 */
		static bool LSN_FASTCALL							RegisterDx9TargetClass();

		// == Members.
		/** Global: window class name for the DX9 child target. */
		static const wchar_t *								LSN_DX9_TARGET_CLASS;

		/**
		 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.  The final buffer, along with
		 *	its width, height, bit-depth, and stride, are returned.
		 *
		 * \param _pui8Input The buffer to be filtered, which will be a pointer to one of the buffers returned by OutputBuffer() previously.  Its format will be that returned in InputFormat().
		 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width, in pixels, of the result.
		 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height, in pixels, of the result.
		 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth of the result.
		 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes, of the result.
		 * \param _ui64PpuFrame The PPU frame associated with the input data.
		 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
		 * \return Returns a pointer to the filtered output buffer.
		 */
		//virtual uint8_t *									ApplyFilter( uint8_t * _pui8Input, uint32_t &/*_ui32Width*/, uint32_t &/*_ui32Height*/, uint16_t &/*_ui16BitDepth*/, uint32_t &/*_ui32Stride*/, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/ );

	protected :
		// == Types.
		/** The shared base data for all Direct3D 9 filters.  The device (and consequently the loaded DLL) is shared to avoid having to recreate it when switching between filters and ROM files. */
		struct LSN_DX9_GLOBAL_STATE {
			CDirectX9Device									dx9Device;												/**< DirectX 9 device (created on demand). */
			int32_t											i32RefCnt = 0;											/**< The reference count. */


			// == Functions.
			/**
			 * \brief Initializes the DirectX 9 device for this window.
			 * 
			 * Dynamically loads d3d9.dll via the DX9 wrapper and creates a device bound to this HWND.
			 * Call once (e.g., after the window is created) and, if successful, the Paint() path will
			 * render using DirectX 9 instead of the software blitter.
			 *
			 * \return Returns true if the DX9 device was created and is ready.
			 **/
			bool											CreateDx9() {
				if ( dx9Device.GetDirectX9Device() ) {
					++i32RefCnt;
					return true;
				}
				// Let the wrapper decide dynamic availability; Supported() will check for d3d9.dll.
				if ( !CDirectX9::Supported() ) {
					return false;
				}
				// Create with default adapter and no special device string.
				/*m_bUseDx9 = dx9Device.Create( this, "" );

				if ( m_bUseDx9 ) {
					m_upDx9PaletteRender = std::make_unique<CDirectX9NesPresenter>( &dx9Device );
					if ( !m_upDx9PaletteRender.get() ) {
						DestroyDx9();
						return false;
					}
					auto rWinSize = VirtualClientRect( nullptr );
					m_upDx9PaletteRender->SetVertSharpness( 4 );
					if ( !m_upDx9PaletteRender->Init( m_bnEmulator.GetDisplayClient()->DisplayWidth(), m_bnEmulator.GetDisplayClient()->DisplayHeight(), false ) ) {
						DestroyDx9();
						return false;
					}
				}
				return m_bUseDx9;*/
			}

			/**
			 * Destroys the DirectX 9 device and all filters.
			 **/
			void											DestroyDx9() {
				if ( i32RefCnt ) {
					if ( --i32RefCnt == 0 ) {
						/*m_bUseDx9 = false;
						m_upDx9PaletteRender.reset();*/
						dx9Device.Reset();
					}
				}
			}

			/**
			 * \brief Resizes the DX9 backbuffer and reinitializes size-dependent presenter resources.
			 *
			 * Updates cached D3DPRESENT_PARAMETERS with the new client size and resets the device.
			 * Then re-initializes the presenter so its DEFAULT-pool resources (index/LUT targets, VBs)
			 * are recreated against the new device state.
			 *
			 * \param _ui32ClientW New client width in pixels.
			 * \param _ui32ClientH New client height in pixels.
			 * \return True on success; false if the DX9 path is disabled or reset failed.
			 */
			bool											OnSizeDx9( uint32_t /*_ui32ClientW*/, uint32_t /*_ui32ClientH*/ ) {
				if ( !dx9Device.GetDirectX9Device() ) { return false; }

				//if ( !dx9Device.ResetForWindowSize( this ) ) {
				//	// Disable DX9; Paint() will fall back to software.
				//	m_bUseDx9 = false;
				//	return false;
				//}

				//// Rebuild presenter size-dependent resources.
				//if ( m_upDx9PaletteRender ) {
				//	m_upDx9PaletteRender->Init( m_bnEmulator.GetDisplayClient()->DisplayWidth(), m_bnEmulator.GetDisplayClient()->DisplayHeight(), /*use16f=*/false );
				//}
				return true;
			}
		};


		// == Members.
		
	};

}	// namespace lsn


#endif	// #ifdef LSN_DX9
