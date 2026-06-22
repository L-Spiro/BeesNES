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
#include "LSNNesPalette.h"
#include "LSNResamplerBase.h"

#include "../GPU/DirectX9/LSNDirectX9.h"
#include "../GPU/DirectX9/LSNDirectX9Device.h"
#include "../GPU/DirectX9/LSNDirectX9NesPresenter.h"

#include "../GPU/DirectX9/LSNDirectX9Phosphor.h"
#include "../GPU/DirectX9/LSNDirectX9RenderTarget.h"
#include "../GPU/DirectX9/LSNDirectX9Resampler.h"
#include "../GPU/DirectX9/LSNDirectX9TextureGamma.h"
#include "../GPU/DirectX9/LSNDirectX9TexturePixelScaler.h"
#include "../GPU/DirectX9/LSNDirectX9TextureRenderer.h"
#include "../Utilities/LSNUtilities.h"

#include <Widget/LSWWidget.h>

#include <algorithm>
#include <cmath>
#include <memory>
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
		 * Which GPU API is being used?
		 * 
		 * \return Returns a LSN_GRAPHICS_API value indicating which GPU API is being used, if any.
		 **/
		virtual LSN_GRAPHICS_API							GpuApi() const { return LSN_GA_DX9; }

		/**
		 * Called when the filter is about to become active.
		 */
		virtual void										Activate() override;

		/**
		 * Called when the filter is about to become inactive.
		 */
		virtual void										DeActivate() override;

		// == Members.
		/** Global: window class name for the DX9 child target. */
		static const wchar_t *								LSN_DX9_TARGET_CLASS;


		// == Functions.
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
		 * \c false if still lost and rendering should be skipped this frame.
		 **/
		static bool											HandleDeviceLoss() {
			if ( !s_dgsState.i32RefCnt || !s_dgsState.hWndDx9Target ) { return false; }
			return s_dgsState.dx9Device.HandleDeviceLoss( s_dgsState.hWndDx9Target );
		}

		/**
		 * Gets a reference to the shared global device.
		 * 
		 * \return Returns a reference to the shared global device.
		 **/
		static CDirectX9Device &							Device() { return s_dgsState.dx9Device; }

		/**
		 * \brief Updates the vertical sharpness factor (integer scale).
		 *
		 * Changing this triggers size-dependent reallocation on the next Init().
		 *
		 * \param _ui32Factor The vertical sharpness.
		 */
		inline void											SetVertSharpness( uint32_t _ui32Factor ) { m_ui32VertSharpness = std::max<uint32_t>( 1, _ui32Factor ); }

		/**
		 * \brief Gets the current vertical sharpness factor.
		 * 
		 * \return Returns the vertical sharpness factor.
		 */
		inline uint32_t										GetVertSharpness() const { return m_ui32VertSharpness; }

		/**
		 * \brief Updates the horizontal sharpness factor (integer scale).
		 *
		 * Changing this triggers size-dependent reallocation on the next Init().
		 *
		 * \param _ui32Factor The horizontal sharpness.
		 */
		inline void											SetHorSharpness( uint32_t _ui32Factor ) { m_ui32HorSharness = std::max<uint32_t>( 1, _ui32Factor ); }

		/**
		 * \brief Gets the current horizontal sharpness factor.
		 * 
		 * \return Returns the horizontal sharpness factor.
		 */
		inline uint32_t										GetHorSharpness() const { return m_ui32HorSharness; }

		/**
		 * Gets the actual vertical sharpness.  Mipmaps are used when the display image is small enough.
		 * 
		 * \param _uiOutH The screen output height.
		 * 
		 * \return Returns the image scale to use, accounting for mipmaps.
		 **/
		inline uint32_t										GetActualVertSharpness( uint32_t _uiOutH ) const {
			return m_ui32RsrcH ? std::min<uint32_t>( static_cast<uint32_t>(std::ceil( _uiOutH / double( m_ui32RsrcH ) )), GetVertSharpness() ) : 1;
		}

		/**
		 * Gets the actual horizontal sharpness.  Mipmaps are used when the display image is small enough.
		 * 
		 * \param _uiOutW The screen output width.
		 * 
		 * \return Returns the image scale to use, accounting for mipmaps.
		 **/
		inline uint32_t										GetActualHorSharpness( uint32_t _uiOutW ) const {
			return m_ui32RsrcW ? std::min<uint32_t>( static_cast<uint32_t>(std::ceil( _uiOutW / double( m_ui32RsrcW ) )), GetHorSharpness() ) : 1;
		}

		/**
		 * Sets whether to use a 16-bit render target for the initial pass.  Must be called before the filter is actually used.
		 * 
		 * \param _bUse16Bit If true, a 16-bit target is used, otherwise a 32-bit target is used.
		 **/
		inline void											Use16Target( bool _bUse16Bit ) { m_bUse16BitInitialTarget = _bUse16Bit; }

		/**
		 * Sets whether the filter should use the high-quality 2-pass resampler for the final composite render.
		 * 
		 * \param _bUse If true, CDirectX9Resampler is used.
		 **/
		inline void											SetUseHighQualityResampler( bool _bUse ) { m_bUseHighQualityResampler = _bUse; }

		/**
		 * Gets whether the filter is configured to use the high-quality resampler.
		 * 
		 * \return Returns true if CDirectX9Resampler is enabled.
		 **/
		inline bool											GetUseHighQualityResampler() const { return m_bUseHighQualityResampler; }

		/**
		 * \brief Sets the gamma curve.
		 * 
		 * \param _gGamma The gamma curve to apply.
		 */
		inline void											SetGamma( CNesPalette::LSN_GAMMA _gGamma ) { m_gGamma = _gGamma; }

		/**
		 * \brief Gets the current gamma setting.
		 * 
		 * \return Returns the gamma setting.
		 */
		inline CNesPalette::LSN_GAMMA						GetGamma() const { return m_gGamma; }

		/**
		 * \brief Gets the effective gamma curve.
		 * 
		 * \return Returns the resolved gamma curve (handling LSN_G_AUTO).
		 */
		virtual inline CNesPalette::LSN_GAMMA				GetEffectiveGamma() const { return m_gGamma == CNesPalette::LSN_G_AUTO ? CNesPalette::LSN_G_CRT2 : m_gGamma; }

		/**
		 * \brief Sets whether phosphor decay is enabled.
		 * 
		 * \param _bEnable If true, phosphor decay is applied.
		 */
		inline void											SetPhosphorDecayEnable( bool _bEnable ) { m_bEnablePhosphorDecay = _bEnable; }

		/**
		 * \brief Gets whether phosphor decay is enabled.
		 * 
		 * \return Returns true if phosphor decay is enabled.
		 */
		inline bool											GetPhosphorDecayEnable() const { return m_bEnablePhosphorDecay; }

		/**
		 * Sets the phospher decay time.
		 *
		 * \param _fTime The time it takes the phosphors to decay to 0.001.
		 **/
		void												SetPhosphorDecayPeriod( float _fTime = 1.79113161563873291015625f ) {
			m_fPhosphorDecayTime = _fTime;
			m_fPhosphorDecayRateGreen = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime, m_fFps ));
			m_fPhosphorDecayRateRed = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.45f, m_fFps ));
			m_fPhosphorDecayRateBlue = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.25f, m_fFps ));
		}

		/**
		 * Sets the FPS of the hardware.
		 *
		 * \param _fFps The FPS to set.
		 **/
		void												SetFps( float _fFps = 60.098812103271484375f ) {
			m_fFps = _fFps;
			m_fPhosphorDecayRateGreen = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime, m_fFps ));
			m_fPhosphorDecayRateRed = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.45f, m_fFps ));
			m_fPhosphorDecayRateBlue = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.25f, m_fFps ));
		}

		/**
		 * Sets the initial phosphor decay level.
		 *
		 * \param _fLevel The strength of the phosphor decay.
		 **/
		void												SetPhosphorDecayLevel( float _fLevel = 0.25f ) {
			m_fInitPhosphorDecay = _fLevel;
			m_fPhosphorDecayRateGreen = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime, m_fFps ));
			m_fPhosphorDecayRateRed = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.45f, m_fFps ));
			m_fPhosphorDecayRateBlue = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.25f, m_fFps ));
		}

		/**
		 * Gets the convolution sampler to use for resampling.
		 * 
		 * \param _ui32Width The target width.
		 * \param _ui32Height The target height.
		 * \return Returns the desired convolution sampler to use.
		 **/
		virtual inline CResamplerBase::LSN_FILTER_FUNCS		GetPreferredConvolutionFilter( uint32_t _ui32Width, uint32_t _ui32Height ) {
			// For low resolutions, use the sharpest-possible filter.
			float fResolutionFactor = std::min( static_cast<float>(_ui32Width) / static_cast<float>(m_ui32RsrcW ? m_ui32RsrcW : 256), static_cast<float>(_ui32Height) / static_cast<float>(m_ui32RsrcH ? m_ui32RsrcH : 240) );
			if ( fResolutionFactor < 2.5 ) { return CResamplerBase::LSN_FF_CARDINALSPLINEUNIFORM; }
			if ( fResolutionFactor < 3.5 ) { return CResamplerBase::LSN_FF_ROBIDOUXSHARP; }
			return CResamplerBase::LSN_FF_LINEAR;
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
#pragma pack( pop )


		// == Members.
		/** The global Direct3D 9 state. */
		static LSN_DX9_GLOBAL_STATE							s_dgsState;

		/** Generically applies gamma to a native-sized texture. */
		CDirectX9TextureGamma								m_tgGamma;
		/** Hardware wrapper for a phosphor-decay post-processing effect. */
		CDirectX9Phosphor									m_pPhosphor;
		/** Generically scales a texture via nearest-neighbor. */
		CDirectX9TexturePixelScaler							m_tpsScaler;
		/** 2-Pass high-quality texture resampler. */
		CDirectX9Resampler									m_rsResampler;
		/** Generically renders a texture to the backbuffer using bilinear sampling. */
		CDirectX9TextureRenderer							m_trRenderer;

		/** Gamma pass render target. */
		std::unique_ptr<CDirectX9RenderTarget>				m_rtGamma;
		/** Phosphor decay target. */
		std::unique_ptr<CDirectX9RenderTarget>				m_rtPhosphorTarget;
		/** Intermediate resample floating-point render target for passing to the screen composite. */
		std::unique_ptr<CDirectX9RenderTarget>				m_rtResampled;

		/** Current selected gamma curve. */
		CNesPalette::LSN_GAMMA								m_gGamma = CNesPalette::LSN_G_AUTO;

		/** The FPS. */
		float												m_fFps = 60.098812103271484375f;
		/** Max phosphor level. */
		float												m_fInitPhosphorDecay = 0.25f;
		/** Red phosphor decay rate. */
		float												m_fPhosphorDecayRateRed = 0.401767850f;
		/** Green phosphor decay rate. */
		float												m_fPhosphorDecayRateGreen = 0.663420439f;
		/** Blue phosphor decay rate. */
		float												m_fPhosphorDecayRateBlue = 0.193711475f;
		/** The time it takes for the phosphors to decay to 0.001. */
		float												m_fPhosphorDecayTime = 1.79113161563873291015625f;
		/** Enable phosphor decay? */
		bool												m_bEnablePhosphorDecay = true;

		/** Created resource width. */
		uint32_t											m_ui32RsrcW = 0;
		/** Created resource height. */
		uint32_t											m_ui32RsrcH = 0;
		/** Resampled target width. */
		uint32_t											m_ui32ResampledTargetW = 0;
		/** Resampled target height. */
		uint32_t											m_ui32ResampledTargetH = 0;
		/** Vertical sharpness factor. */
		uint32_t											m_ui32VertSharpness = 3;
		/** Horizontal sharpness factor. */
		uint32_t											m_ui32HorSharness = 1;
		/** Use a 16-bit initial render target? */
		bool												m_bUse16BitInitialTarget = true;
		/** Toggles whether the high-quality 2-pass CDirectX9Resampler handles final scaling. */
		bool												m_bUseHighQualityResampler = true;


		// == Functions.
		/**
		 * \brief Registers the DX9 child target window class (no background erase).
		 * 
		 * \return Returns true if the class is registered or already existed.
		 */
		static bool LSN_FASTCALL							RegisterDx9TargetClass();

		/**
		 * \brief Resizes the DX9 backbuffer and reinitializes size-dependent presenter resources.
		 *
		 * Updates cached D3DPRESENT_PARAMETERS with the new client size and resets the device.
		 * Then re-initializes the presenter so its DEFAULT-pool resources (index/LUT targets, VBs)
		 * are recreated against the new device state.
		 *
		 * \return True on success; false if the DX9 path is disabled or reset failed.
		 */
		static bool											OnSizeDx9();

		/**
		 * \brief Fills the screen-space quad vertex buffer with an XYZRHW|TEX1 quad.
		 *
		 * Applies a -0.5f XY bias to align texel centers with pixel centers in D3D9 when using XYZRHW.
		 *
		 * \param _dx9vbBuffer The vertex buffer to arrange.
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
		bool												FillQuad( CDirectX9VertexBuffer &_dx9vbBuffer, float _fL, float _fT, float _fR, float _fB, float _fU0, float _fV0, float _fU1, float _fV1 );

		/**
		 * \brief Computes half-texel-correct UVs for a W×H texture in D3D9.
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
		static inline void									HalfTexelUv( uint32_t /*_uiW*/, uint32_t /*_uiH*/,
			float &_fU0, float &_fV0, float &_fU1, float &_fV1 ) {
			/*const float fInvW = 0.5f / static_cast<float>(_uiW);
			const float fInvH = 0.5f / static_cast<float>(_uiH);
			_fU0 = -fInvW;
			_fV0 = -fInvH;
			_fU1 = 1.0f - fInvW;
			_fV1 = 1.0f - fInvH;*/

			_fU0 = 0.0f;
			_fV0 = 0.0f;
			_fU1 = 1.0f;
			_fV1 = 1.0f;
		}

		/**
		 * \brief Ensures base internal size is updated and size-dependent resources are (re)created.
		 *
		 * \param _pdx9dDevice The active Direct3D 9 device wrapper.
		 * \param _ui32NativeW The unscaled native output width.
		 * \param _ui32NativeH The unscaled native output height.
		 * \return Returns true on success.
		 */
		bool												EnsureBaseSizeAndResources( CDirectX9Device * _pdx9dDevice, uint32_t _ui32NativeW, uint32_t _ui32NativeH );

		/**
		 * \brief Releases size-dependent base resources.
		 */
		void												ReleaseBaseSizeDependents();

		/**
		 * Renders the common D3D9 pipeline components.
		 *
		 * \param _pdx9dDevice The active Direct3D 9 device wrapper.
		 * \param _ptSrc The starting RGB source texture generated by the subclass.
		 * \param _ui32NativeW The base resolution width (prior to integer scaling).
		 * \param _ui32NativeH The base resolution height (prior to integer scaling).
		 * \param _rOutput The destination rectangle in client pixels.
		 * \param _bFlipY Determines whether the integer scaler pass should flip the Y axis.
		 * \return Returns true if rendering succeeded.
		 */
		bool												RenderBase( CDirectX9Device * _pdx9dDevice, IDirect3DTexture9 * _ptSrc, uint32_t _ui32NativeW, uint32_t _ui32NativeH, const lsw::LSW_RECT &_rOutput, bool _bFlipY = false );
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
