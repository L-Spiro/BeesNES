#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for Direct3D 12 filters.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNGpuFilterBase.h"
#include "LSNNesPalette.h"
#include "LSNResamplerBase.h"

#include "../GPU/DirectX12/LSNDirectX12.h"
#include "../GPU/DirectX12/LSNDirectX12Device.h"
#include "../GPU/DirectX12/LSNDirectX12CommandQueue.h"
#include "../GPU/DirectX12/LSNDirectX12GraphicsCommandList.h"
#include "../GPU/DirectX12/LSNDirectX12DescriptorHeap.h"
#include "../GPU/DirectX12/LSNDirectX12Fence.h"
#include "../GPU/DirectX12/LSNDirectX12Resource.h"

#include "../GPU/DirectX12/LSNDirectX12CommandAllocator.h"
#include "../GPU/DirectX12/LSNDirectX12Phosphor.h"
#include "../GPU/DirectX12/LSNDirectX12Resampler.h"
#include "../GPU/DirectX12/LSNDirectX12TextureGamma.h"
#include "../GPU/DirectX12/LSNDirectX12TexturePixelScaler.h"
#include "../GPU/DirectX12/LSNDirectX12TextureRenderer.h"
#include "../Utilities/LSNUtilities.h"

#include <Widget/LSWWidget.h>

#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>


namespace lsn {

	/**
	 * Class CDx12FilterBase
	 * \brief The base class for Direct3D 12 filters.
	 *
	 * Description: Manages a shared D3D12 device and swap-chain context for all D3D12-based
	 *	filters. Derived classes build their pipelines (root signatures, PSOs, resources)
	 *	using this common device.
	 */
	class CDx12FilterBase : public CGpuFilterBase {
	public :
		CDx12FilterBase();
		virtual ~CDx12FilterBase();


		// == Functions.
		/**
		 * Sets the basic parameters for the filter.
		 *
		 * \param _stBuffers The number of render targets to create.
		 * \param _ui16Width The console screen width.  Typically 256.
		 * \param _ui16Height The console screen height.  Typically 240.
		 * \return Returns the input format requested of the PPU.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT				Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) override;

		/**
		 * Which GPU API is being used?
		 * 
		 * \return Returns a LSN_GRAPHICS_API value indicating which GPU API is being used, if any.
		 **/
		virtual LSN_GRAPHICS_API								GpuApi() const override { return LSN_GA_DX12; }

		/**
		 * Called when the filter is about to become active.
		 */
		virtual void											Activate() override;

		/**
		 * Called when the filter is about to become inactive.
		 */
		virtual void											DeActivate() override;

		/**
		 * Sets the parent window handle.  Must be set only when there is a 0 reference count.
		 * Typically it will only ever be set once; nothing should ever destroy the main window except shut-down.
		 * 
		 * \param _pwParent The parent window for the render window.
		 * \return Returns false if the global reference count is not 0.
		 **/
		static bool												SetRenderWindowParent( lsw::CWidget * _pwParent ) {
			if ( s_dgsState.i32RefCnt != 0 ) { return false; }
			s_dgsState.pwParent = _pwParent;
			return true;
		}

		/**
		 * \brief Check cooperative level and recover from device loss when possible.
		 *
		 * \return Returns true if the device is usable for this frame; false if lost.
		 **/
		static bool												HandleDeviceLoss() {
			if ( !s_dgsState.i32RefCnt || !s_dgsState.hWndTarget || !s_dgsState.dx12Device.GetDevice() ) { return false; }
			HRESULT hRes = s_dgsState.dx12Device.GetDevice()->GetDeviceRemovedReason();
			if ( FAILED( hRes ) ) {
				// D3D12 device is completely removed. Requires full teardown and recreation.
				return false; 
			}
			return true;
		}

		/**
		 * Gets a reference to the shared global device.
		 * 
		 * \return Returns a reference to the shared global device.
		 **/
		static CDirectX12Device &								Device() { return s_dgsState.dx12Device; }

		// == Common Pipeline Accessors ==
		/**
		 * \brief Updates the vertical sharpness factor (integer scale).
		 *
		 * Changing this triggers size-dependent reallocation on the next Init().
		 *
		 * \param _ui32Factor The vertical sharpness.
		 */
		inline void												SetVertSharpness( uint32_t _ui32Factor ) { m_ui32VertSharpness = std::max<uint32_t>( 1, _ui32Factor ); }

		/**
		 * \brief Gets the current vertical sharpness factor.
		 * \return Returns the vertical sharpness factor.
		 */
		inline uint32_t											GetVertSharpness() const { return m_ui32VertSharpness; }

		/**
		 * \brief Updates the horizontal sharpness factor (integer scale).
		 *
		 * Changing this triggers size-dependent reallocation on the next Init().
		 *
		 * \param _ui32Factor The horizontal sharpness.
		 */
		inline void												SetHorSharpness( uint32_t _ui32Factor ) { m_ui32HorSharness = std::max<uint32_t>( 1, _ui32Factor ); }

		/**
		 * \brief Gets the current horizontal sharpness factor.
		 * \return Returns the horizontal sharpness factor.
		 */
		inline uint32_t											GetHorSharpness() const { return m_ui32HorSharness; }

		/**
		 * Gets the actual vertical sharpness.  Mipmaps are used when the display image is small enough.
		 * \param _uiOutH The screen output height.
		 * \return Returns the image scale to use, accounting for mipmaps.
		 **/
		inline uint32_t											GetActualVertSharpness( uint32_t _uiOutH ) const {
			return m_ui32RsrcH ? std::min<uint32_t>( static_cast<uint32_t>(std::ceil( _uiOutH / double( m_ui32RsrcH ) )), GetVertSharpness() ) : 1;
		}

		/**
		 * Gets the actual horizontal sharpness.  Mipmaps are used when the display image is small enough.
		 * \param _uiOutW The screen output width.
		 * \return Returns the image scale to use, accounting for mipmaps.
		 **/
		inline uint32_t											GetActualHorSharpness( uint32_t _uiOutW ) const {
			return m_ui32RsrcW ? std::min<uint32_t>( static_cast<uint32_t>(std::ceil( _uiOutW / double( m_ui32RsrcW ) )), GetHorSharpness() ) : 1;
		}

		/**
		 * Sets whether to use a 16-bit render target for the initial pass.  Must be called before the filter is actually used.
		 * \param _bUse16Bit If true, a 16-bit target is used, otherwise a 32-bit target is used.
		 **/
		inline void												Use16Target( bool _bUse16Bit ) { m_bUse16BitInitialTarget = _bUse16Bit; }

		/**
		 * Sets whether the filter should use the high-quality 2-pass resampler for the final composite render.
		 * \param _bUse If true, CDirectX12Resampler is used.
		 **/
		inline void												SetUseHighQualityResampler( bool _bUse ) { m_bUseHighQualityResampler = _bUse; }

		/**
		 * Gets whether the filter is configured to use the high-quality resampler.
		 * \return Returns true if CDirectX12Resampler is enabled.
		 **/
		inline bool												GetUseHighQualityResampler() const { return m_bUseHighQualityResampler; }

		/**
		 * \brief Sets the gamma curve.
		 * \param _gGamma The gamma curve to apply.
		 */
		inline void												SetGamma( CNesPalette::LSN_GAMMA _gGamma ) { m_gGamma = _gGamma; }

		/**
		 * \brief Gets the current gamma setting.
		 * \return Returns the gamma setting.
		 */
		inline CNesPalette::LSN_GAMMA							GetGamma() const { return m_gGamma; }

		/**
		 * \brief Gets the effective gamma curve.
		 * \return Returns the resolved gamma curve (handling LSN_G_AUTO).
		 */
		virtual inline CNesPalette::LSN_GAMMA					GetEffectiveGamma() const { return m_gGamma == CNesPalette::LSN_G_AUTO ? CNesPalette::LSN_G_CRT2 : m_gGamma; }

		/**
		 * \brief Sets whether phosphor decay is enabled.
		 * \param _bEnable If true, phosphor decay is applied.
		 */
		inline void												SetPhosphorDecayEnable( bool _bEnable ) { m_bEnablePhosphorDecay = _bEnable; }

		/**
		 * \brief Gets whether phosphor decay is enabled.
		 * \return Returns true if phosphor decay is enabled.
		 */
		inline bool												GetPhosphorDecayEnable() const { return m_bEnablePhosphorDecay; }

		/**
		 * Sets the phospher decay time.
		 *
		 * \param _fTime The time it takes the phosphors to decay to 0.001.
		 **/
		void													SetPhosphorDecayPeriod( float _fTime = 1.79113161563873291015625f ) {
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
		void													SetFps( float _fFps = 60.098812103271484375f ) {
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
		void													SetPhosphorDecayLevel( float _fLevel = 0.25f ) {
			m_fInitPhosphorDecay = _fLevel;
			m_fPhosphorDecayRateGreen = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime, m_fFps ));
			m_fPhosphorDecayRateRed = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.45f, m_fFps ));
			m_fPhosphorDecayRateBlue = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.25f, m_fFps ));
		}

		/**
		 * Gets the convolution sampler to use for resampling.
		 * \param _ui32Width The target width.
		 * \param _ui32Height The target height.
		 * \return Returns the desired convolution sampler to use.
		 **/
		virtual inline CResamplerBase::LSN_FILTER_FUNCS			GetPreferredConvolutionFilter( uint32_t _ui32Width, uint32_t _ui32Height ) {
			// For low resolutions, use the sharpest-possible filter.
			float fResolutionFactor = std::min( static_cast<float>(_ui32Width) / static_cast<float>(m_ui32RsrcW ? m_ui32RsrcW : 256), static_cast<float>(_ui32Height) / static_cast<float>(m_ui32RsrcH ? m_ui32RsrcH : 240) );
			if ( fResolutionFactor < 2.5 ) { return CResamplerBase::LSN_FF_CARDINALSPLINEUNIFORM; }
			if ( fResolutionFactor < 3.5 ) { return CResamplerBase::LSN_FF_ROBIDOUXSHARP; }
			return CResamplerBase::LSN_FF_LINEAR;
		}

	protected :
		// == Types.
		// TMP.
#pragma pack( push, 1 )
		struct LSN_XYZRHWTEX1 {
			float fX, fY, fZ, fW;
			float fU, fV;
		};
#pragma pack( pop )

		/** The shared base data for all Direct3D 12 filters.  The device is shared to avoid having to recreate it when switching between filters and ROM files. */
		struct LSN_DX12_GLOBAL_STATE {
			lsw::CWidget *										pwParent = nullptr;										/**< The parent window.  Can only be set while there is a 0 reference count. */
			HWND												hWndTarget = NULL;										/**< Render window. */
			CDirectX12Device									dx12Device;												/**< DirectX 12 device (created on demand). */
			int32_t												i32RefCnt = 0;											/**< The reference count. */
			lsw::LSW_RECT										rScreenRect;											/**< The output rectangle. */
			bool												bValidState = false;									/**< Is the device in a valid state? */

			// == Functions.
			/**
			 * \brief Initializes the DirectX 12 device for this window.
			 * 
			 * Dynamically loads d3d12.dll via the DX12 wrapper and creates a device bound to this HWND.
			 * Call once (e.g., after the window is created) and, if successful, the Paint() path will
			 * render using DirectX 12 instead of the software blitter.
			 *
			 * \return Returns true if the DX12 device was created and is ready.
			 **/
			bool												CreateDx12();

			/**
			 * \brief Creates the child target window used for DX12 presentation.
			 * 
			 * \return Returns true if created or already exists.
			 */
			bool												CreateTargetChild();

			/**
			 * Destroys the DirectX 12 device and swap chain when the ref-count reaches 0.
			 **/
			void												DestroyDx12();

			/**
			 * \brief Lays out and caches the drawable client region for the swap chain.
			 *
			 * \return Returns true on success.
			 */
			bool												LayoutTargetChild();


			~LSN_DX12_GLOBAL_STATE() {
				// Check for proper use.
				if ( i32RefCnt ) {
					::OutputDebugStringA( "\r\n* * * * * LSN_DX12_GLOBAL_STATE::~LSN_DX12_GLOBAL_STATE():i32RefCnt NOT 0 * * * * *\r\n\r\n" );
				}
				if ( hWndTarget ) {
					::OutputDebugStringA( "\r\n* * * * * LSN_DX12_GLOBAL_STATE::~LSN_DX12_GLOBAL_STATE():hWndTarget NOT DESTROYED * * * * *\r\n\r\n" );
					::DestroyWindow( hWndTarget );
					hWndTarget = NULL;
				}
			}
		};


		// == Members.
		/** Global: window class name for the DX12 child target. */
		static const wchar_t *									LSN_DX12_TARGET_CLASS;

		/** The global Direct3D 12 state. */
		static LSN_DX12_GLOBAL_STATE							s_dgsState;

		/** The DirectX 12 device wrapper (non-owning). */
		CDirectX12Device *										m_pdx12dDevice = nullptr;


		// == Common Pipeline Components ==
		/** Generically applies gamma to a native-sized texture. */
		CDirectX12TextureGamma									m_tgGamma;
		/** Hardware wrapper for a phosphor-decay post-processing effect. */
		CDirectX12Phosphor										m_pPhosphor;
		/** Generically scales a texture via nearest-neighbor. */
		CDirectX12TexturePixelScaler							m_tpsScaler;
		/** 2-Pass high-quality texture resampler. */
		CDirectX12Resampler										m_rsResampler;
		/** Generically renders a texture to the backbuffer using bilinear sampling. */
		CDirectX12TextureRenderer								m_trRenderer;

		/** Gamma pass render target. */
		std::unique_ptr<CDirectX12Resource>						m_rtGamma;
		/** Phosphor decay target. */
		std::unique_ptr<CDirectX12Resource>						m_rtPhosphorTarget;
		/** Intermediate resample floating-point render target for passing to the screen composite. */
		std::unique_ptr<CDirectX12Resource>						m_rtResampled;

		/** Descriptor Heap for Gamma RTV. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhGammaRtv;
		/** Descriptor Heap for Phosphor RTV. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhPhosphorRtv;
		/** Descriptor Heap for general RTVs. */
		std::unique_ptr<CDirectX12DescriptorHeap>				m_dhRtvHeap;
		/** Descriptor size for RTVs. */
		UINT													m_uiRtvDescriptorSize = 0;

		/** Current selected gamma curve. */
		CNesPalette::LSN_GAMMA									m_gGamma = CNesPalette::LSN_G_AUTO;

		/** Phosphor variables. */
		float													m_fFps = 60.098812103271484375f;
		float													m_fInitPhosphorDecay = 0.25f;
		float													m_fPhosphorDecayRateRed = 0.401767850f;
		float													m_fPhosphorDecayRateGreen = 0.663420439f;
		float													m_fPhosphorDecayRateBlue = 0.193711475f;
		float													m_fPhosphorDecayTime = 1.79113161563873291015625f;
		bool													m_bEnablePhosphorDecay = true;

		/** Created resource width. */
		uint32_t												m_ui32RsrcW = 0;
		/** Created resource height. */
		uint32_t												m_ui32RsrcH = 0;
		/** Resampled target width. */
		uint32_t												m_ui32ResampledTargetW = 0;
		/** Resampled target height. */
		uint32_t												m_ui32ResampledTargetH = 0;
		/** Vertical sharpness factor. */
		uint32_t												m_ui32VertSharpness = 3;
		/** Horizontal sharpness factor. */
		uint32_t												m_ui32HorSharness = 1;
		/** Use a 16-bit initial render target? */
		bool													m_bUse16BitInitialTarget = true;
		/** Toggles whether the high-quality 2-pass CDirectX12Resampler handles final scaling. */
		bool													m_bUseHighQualityResampler = true;


		// == Functions.
		/**
		 * \brief Registers the DX12 child target window class (no background erase).
		 * \return Returns true if the class is registered or already existed.
		 */
		static bool LSN_FASTCALL								RegisterDx12TargetClass();

		/**
		 * \brief Handles resizing the DX12 buffers and targets.
		 * \return Returns true if resizing was handled correctly.
		 */
		static bool												OnSizeDx12();

		/**
		 * \brief Fills a buffer with an XYZRHW|TEX1 quad.
		 *
		 * Note: Direct3D 12 does not require the -0.5f offset used in Direct3D 9.
		 *
		 * \param _dx12rBuffer The vertex buffer resource to arrange.
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
		bool													FillQuad( CDirectX12Resource &_dx12rBuffer, float _fL, float _fT, float _fR, float _fB, float _fU0, float _fV0, float _fU1, float _fV1 );

		/**
		 * \brief Ensures base internal size is updated and size-dependent resources are (re)created.
		 *
		 * \param _pdx12dDevice The active Direct3D 12 device wrapper.
		 * \param _pgclCommandList The command list used to initialize resource state barriers on creation.
		 * \param _ui32NativeW The unscaled native output width.
		 * \param _ui32NativeH The unscaled native output height.
		 * \return Returns true on success.
		 */
		bool													EnsureBaseSizeAndResources( CDirectX12Device * _pdx12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, uint32_t _ui32NativeW, uint32_t _ui32NativeH );

		/**
		 * \brief Releases size-dependent base resources.
		 */
		void													ReleaseBaseSizeDependents();

		/**
		 * Renders the common D3D12 pipeline components.
		 *
		 * \param _pdx12dDevice The active Direct3D 12 device wrapper.
		 * \param _pgclCommandList The command list in which to record draw commands. (Caller must close and execute).
		 * \param _prSrc The starting RGB source texture resource generated by the subclass.
		 * \param _ui32NativeW The base resolution width (prior to integer scaling).
		 * \param _ui32NativeH The base resolution height (prior to integer scaling).
		 * \param _rOutput The destination rectangle in client pixels.
		 * \param _bFlipY Determines whether the integer scaler pass should flip the Y axis.
		 * \return Returns true if rendering succeeded.
		 */
		bool													RenderBase( CDirectX12Device * _pdx12dDevice, CDirectX12GraphicsCommandList * _pgclCommandList, CDirectX12Resource * _prSrc, uint32_t _ui32NativeW, uint32_t _ui32NativeH, const lsw::LSW_RECT &_rOutput, bool _bFlipY = false );
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12
