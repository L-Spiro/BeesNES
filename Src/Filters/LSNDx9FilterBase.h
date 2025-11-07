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

#include <Widget/LSWWidget.h>

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


		// == Types.
		static constexpr DWORD								LSN_FVF_XYZRHWTEX1 = D3DFVF_XYZRHW | D3DFVF_TEX1;


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
		 * \brief Registers the DX9 child target window class (no background erase).
		 * 
		 * \return Returns true if the class is registered or already existed.
		 */
		static bool LSN_FASTCALL							RegisterDx9TargetClass();

		// == Members.
		/** Global: window class name for the DX9 child target. */
		static const wchar_t *								LSN_DX9_TARGET_CLASS;

		/**
		 * Sets the parent window handle.  Must be set only when there is a 0 reference count.  Typically it will only ever be set once; nothing should ever destroy the main window except shut-down.
		 * 
		 * \param _pwParent The parent window for the render window.
		 * \return Returns false if the global reference count is not 0.
		 **/
		static bool											SetRenderWindowParent( lsw::CWidget * _pwParent ) {
			if ( s_dgsState.i32RefCnt != 0 ) { return false; }
			s_dgsState.pwParent = _pwParent;
			return true;
		}

		/**
		 * \brief Check cooperative level and recover from device loss when possible.
		 *
		 * When \c TestCooperativeLevel() returns \c D3DERR_DEVICENOTRESET, this attempts a \c Reset()
		 * using the current client-area size (via \c ResetForWindowSize()).
		 *
		 * \return Returns \c true if the device is usable for this frame (OK or reset succeeded);
		 *         \c false if still lost and rendering should be skipped this frame.
		 **/
		bool												HandleDeviceLoss() {
			if ( !s_dgsState.i32RefCnt || !s_dgsState.hWndDx9Target ) { return false; }
			return s_dgsState.dx9Device.HandleDeviceLoss( s_dgsState.hWndDx9Target );
		}

	protected :
		// == Types.
		/** The shared base data for all Direct3D 9 filters.  The device (and consequently the loaded DLL) is shared to avoid having to recreate it when switching between filters and ROM files. */
		struct LSN_DX9_GLOBAL_STATE {
			lsw::CWidget *									pwParent = nullptr;										/**< The parent window.  Can only be set while there is a 0 reference count. */
			HWND											hWndDx9Target = NULL;									/**< Render window. */
			CDirectX9Device									dx9Device;												/**< DirectX 9 device (created on demand). */
			int32_t											i32RefCnt = 0;											/**< The reference count. */
			lsw::LSW_RECT									rScreenRect;											/**< The output rectangle. */
			bool											bValidState = false;									/**< Is the device in a valid state? */


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
				bValidState = false;
				// Let the wrapper decide dynamic availability; Supported() will check for d3d9.dll.
				if ( !pwParent || !CDirectX9::Supported() ) { return false; }
				if ( !CDx9FilterBase::RegisterDx9TargetClass() ) { return false; }
				if ( !CreateDx9TargetChild() ) { return false; }
				LayoutDx9TargetChild();

				// Create with default adapter and no special device string.
				if ( dx9Device.Create( hWndDx9Target, "" ) ) {
					++i32RefCnt;
					bValidState = true;
					return true;
				}
				
				return false;
			}

			/**
			 * \brief Creates the child target window used for DX9 presentation.
			 * 
			 * \return Returns true if created or already exists.
			 */
			bool											CreateDx9TargetChild() {
				if ( hWndDx9Target && ::IsWindow( hWndDx9Target ) ) { return true; }
				if ( !pwParent ) { return false; }
				constexpr DWORD dwStyle   = WS_CHILD | WS_VISIBLE;
				constexpr DWORD dwExStyle = WS_EX_NOPARENTNOTIFY;
				rScreenRect = pwParent->VirtualClientRect( nullptr );
				hWndDx9Target = ::CreateWindowExW(
					dwExStyle, CDx9FilterBase::LSN_DX9_TARGET_CLASS, L"", dwStyle,
					rScreenRect.left, rScreenRect.top, rScreenRect.Width(), rScreenRect.Height(),
					pwParent->Wnd(), NULL, ::GetModuleHandleW( NULL ), NULL );

				::SetClassLongPtrW( hWndDx9Target, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(nullptr) );
				return hWndDx9Target != nullptr;
			}

			/**
			 * Destroys the DirectX 9 device and all filters.
			 **/
			void											DestroyDx9() {
				if ( i32RefCnt ) {
					if ( --i32RefCnt == 0 ) {
						bValidState = false;
						dx9Device.Reset();
						if ( hWndDx9Target ) {
							::DestroyWindow( hWndDx9Target );
							hWndDx9Target = NULL;
						}
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
			 * \return True on success; false if the DX9 path is disabled or reset failed.
			 */
			bool											OnSizeDx9() {
				if ( !pwParent || !dx9Device.GetDirectX9Device() ) { return false; }

				LayoutDx9TargetChild();		// Updates rScreenRect.
				if ( bValidState ) {
					if ( !dx9Device.ResetForWindowSize( hWndDx9Target ) ) {
						// Disable DX9; Paint() will fall back to software.
						bValidState = false;
						return false;
					}
				}
				return true;
			}

			/**
			 * \brief Lays out the DX9 child target to cover the drawable client region.
			 *
			 * \return Returns true on success.
			 */
			bool											LayoutDx9TargetChild() {
				if ( !hWndDx9Target || !::IsWindow( hWndDx9Target ) ) { return false; }
				rScreenRect = pwParent->VirtualClientRect( nullptr );
				const int iW = int( std::max<LONG>( 1, rScreenRect.Width() ) );
				const int iH = int( std::max<LONG>( 1, rScreenRect.Height() ) );
				::SetWindowPos( hWndDx9Target, nullptr, rScreenRect.left, rScreenRect.top, iW, iH,
					SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS );
				return true;
			}


			~LSN_DX9_GLOBAL_STATE() {
				// Check for proper use.
				if ( i32RefCnt ) {
					::OutputDebugStringA( "\r\n* * * * * LSN_DX9_GLOBAL_STATE::~LSN_DX9_GLOBAL_STATE():i32RefCnt NOT 0 * * * * *\r\n\r\n" );
				}
				if ( hWndDx9Target ) {
					::OutputDebugStringA( "\r\n* * * * * LSN_DX9_GLOBAL_STATE::~LSN_DX9_GLOBAL_STATE():hWndDx9Target NOT DESTROYED * * * * *\r\n\r\n" );
					::DestroyWindow( hWndDx9Target );
					hWndDx9Target = NULL;
				}
			}
		};

#pragma pack( push, 1 )
		struct LSN_XYZRHWTEX1 {
			float fX, fY, fZ, fRhw;
			float fU, fV;
		};
#pragma pack(pop)


		// == Members.
		/** The global Direct3D 9 state. */
		static LSN_DX9_GLOBAL_STATE							s_dgsState;


		// == Functions.
		/**
		 * \brief Fills the screen-space quad vertex buffer with an XYZRHW|TEX1 quad.
		 *
		 * Applies a -0.5f XY bias to align texel centers with pixel centers in D3D9 when using XYZRHW.
		 *
		 * \param _fL Left X in pixels.
		 * \param _fT Top Y in pixels.
		 * \param _fR Right X in pixels.
		 * \param _fB Bottom Y in pixels.
		 * \param _fU0 Left U coordinate.
		 * \param _fV0 Top V coordinate.
		 * \param _fU1 Right U coordinate.
		 * \param _fV1 Bottom V coordinate.
		 * \return Returns true on success.
		 */
		bool												FillQuad( float _fL, float _fT, float _fR, float _fB, float _fU0, float _fV0, float _fU1, float _fV1 );

		/**
		 * \brief Computes half-texel-correct UVs for a WÅ~H texture in D3D9.
		 *
		 * Centers sampling on texels by offsetting UV edges by 0.5/W and 0.5/H.
		 * Required when using POINT sampling with pre-transformed XYZRHW quads,
		 * otherwise you'll hit the gaps between texels (black columns/rows).
		 *
		 * \param _uiW Texture width in texels.
		 * \param _uiH Texture height in texels.
		 * \param _fU0 Receives the left U (with half-texel offset applied).
		 * \param _fV0 Receives the top V (with half-texel offset applied).
		 * \param _fU1 Receives the right U (with half-texel offset applied).
		 * \param _fV1 Receives the bottom V (with half-texel offset applied).
		 */
		static inline void									HalfTexelUv( uint32_t _uiW, uint32_t _uiH,
			float &_fU0, float &_fV0, float &_fU1, float &_fV1 ) {
			const float fInvW = 1.0f / static_cast<float>(_uiW);
			const float fInvH = 1.0f / static_cast<float>(_uiH);
			_fU0 = 0.5f * fInvW;
			_fV0 = 0.5f * fInvH;
			_fU1 = 1.0f - _fU0;
			_fV1 = 1.0f - _fV0;
		}
	};

}	// namespace lsn


#endif	// #ifdef LSN_DX9
