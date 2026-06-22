#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Direct3D 12 palette filter.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../GPU/DirectX12/LSNDirectX12Texture.h"
#include "../GPU/DirectX12/LSNDirectX12Resource.h"
#include "../GPU/DirectX12/LSNDirectX12DescriptorHeap.h"
#include "../GPU/DirectX12/LSNDirectX12RootSignature.h"
#include "../GPU/DirectX12/LSNDirectX12PipelineState.h"
#include "../GPU/DirectX12/LSNDirectX12GraphicsCommandList.h"
#include "../GPU/DirectX12/LSNDirectX12CommandAllocator.h"
#include "../GPU/DirectX12/LSNDirectX12Phosphor.h"
#include "../GPU/DirectX12/LSNDirectX12Resampler.h"
#include "../GPU/DirectX12/LSNDirectX12TextureGamma.h"
#include "../GPU/DirectX12/LSNDirectX12TexturePixelScaler.h"
#include "../GPU/DirectX12/LSNDirectX12TextureRenderer.h"
#include "LSNDx12FilterBase.h"
#include "../Utilities/LSNUtilities.h"

#include <Helpers/LSWHelpers.h>
#include <d3dcompiler.h>

#include <algorithm>
#include <memory>
#include <vector>

namespace lsn {

	/**
	 * Class CDx12PaletteFilter
	 * \brief A standard 24-bit RGB filter for Direct3D 12.
	 *
	 * Description: A standard 24-bit RGB filter utilizing D3D12 Pipeline State Objects and explicit upload heaps.
	 */
	class CDx12PaletteFilter : public CDx12FilterBase {
	public :
		CDx12PaletteFilter();
		virtual ~CDx12PaletteFilter();


		// == Functions.
		/**
		 * Sets the basic parameters for the filter.
		 *
		 * \param _stBuffers The number of render targets to create.
		 * \param _ui16Width The console screen width.  Typically 256.
		 * \param _ui16Height The console screen height.  Typically 240.
		 * \return Returns the input format requested of the PPU.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) override;

		/**
		 * \brief Updates the vertical sharpness factor (integer scale).
		 *
		 * Changing this triggers size-dependent reallocation on the next Init().
		 *
		 * \param _ui32Factor The vertical sharpness.
		 */
		void												SetVertSharpness( uint32_t _ui32Factor ) { m_ui32VertSharpness = std::max<uint32_t>( 1, _ui32Factor ); }

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
		void												SetHorSharpness( uint32_t _ui32Factor ) { m_ui32HorSharness = std::max<uint32_t>( 1, _ui32Factor ); }

		/**
		 * \brief Gets the current horizontal sharpness factor.
		 * 
		 * \return Returns the horizontal sharpness factor.
		 */
		inline uint32_t										GetHorSharpness() const { return m_ui32HorSharness; }

		/**
		 * Gets the actual vertical sharpness.  Mipmaps are used when the display image is small enough.
		 * 
		 * \return Returns the image scale to use, accounting for mipmaps.
		 **/
		inline uint32_t										GetActualVertSharpness() const {
			return m_ui32SrcH ? std::min<uint32_t>( static_cast<uint32_t>(std::ceil( s_dgsState.rScreenRect.Height() / double( m_ui32SrcH ) )), GetVertSharpness() ) : 1;
		}

		/**
		 * Gets the actual horizontal sharpness.  Mipmaps are used when the display image is small enough.
		 * 
		 * \return Returns the image scale to use, accounting for mipmaps.
		 **/
		inline uint32_t										GetActualHorSharpness() const {
			return m_ui32SrcW ? std::min<uint32_t>( static_cast<uint32_t>(std::ceil( s_dgsState.rScreenRect.Width() / double( m_ui32SrcW ) )), GetHorSharpness() ) : 1;
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
		 * \param _bUse If true, CDirectX12Resampler is used.
		 **/
		inline void											SetUseHighQualityResampler( bool _bUse ) { m_bUseHighQualityResampler = _bUse; }

		/**
		 * Gets whether the filter is configured to use the high-quality resampler.
		 * 
		 * \return Returns true if CDirectX12Resampler is enabled.
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
		inline CNesPalette::LSN_GAMMA						GetEffectiveGamma() const { return m_gGamma == CNesPalette::LSN_G_AUTO ? CNesPalette::LSN_G_sRGB : m_gGamma; }

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
		inline CResamplerBase::LSN_FILTER_FUNCS				GetPreferredConvolutionFilter( uint32_t _ui32Width, uint32_t _ui32Height ) {
			// For low resolutions, use the sharpest-possible filter.
			float fResolutionFactor = std::min( static_cast<float>(_ui32Width) / static_cast<float>(m_ui32SrcW), static_cast<float>(_ui32Height) / static_cast<float>(m_ui32SrcH) );
			if ( fResolutionFactor < 2.5 ) { return CResamplerBase::LSN_FF_CARDINALSPLINEUNIFORM; }
			if ( fResolutionFactor < 3.5 ) { return CResamplerBase::LSN_FF_ROBIDOUXSHARP; }
			return CResamplerBase::LSN_FF_LINEAR;
		}

		/**
		 * Sets the palette.
		 * 
		 * \param _pfRgba512 Pointer to 2048 floats (512 * RGBA).
		 * \return Returns true if the memory for the palette copy was able to be allocated and _pfRgba512 is not nullptr.
		 **/
		bool												SetLut( const float * _pfRgba512 );

		/**
		 * Called when the filter is about to become active.
		 */
		virtual void										Activate() override;

		/**
		 * Called when the filter is about to become inactive.
		 */
		virtual void										DeActivate() override;

		/**
		 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.
		 *
		 * \param _pui8Input The buffer to be filtered.
		 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width.
		 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height.
		 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth.
		 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes.
		 * \param _ui64PpuFrame The PPU frame associated with the input data.
		 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
		 * \param _i32DispLeft The display area left.
		 * \param _i32DispTop The display area top.
		 * \param _ui32DispWidth The display area width.
		 * \param _ui32DispHeight The display area height
		 * \return Returns a pointer to the filtered output buffer.
		 */
		virtual uint8_t *									ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/,
			int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) override;

		/**
		 * Informs the filter of a window resize.
		 **/
		virtual void										FrameResize() override;


	protected :
		// == Members.
		/** The DirectX 12 device wrapper (non-owning). */
		CDirectX12Device *									m_pdx12dDevice = nullptr;
		
		/** Generically applies gamma to a native-sized texture. */
		CDirectX12TextureGamma								m_tgGamma;
		/** Hardware wrapper for a phosphor-decay post-processing effect. */
		CDirectX12Phosphor									m_pPhosphor;
		/** Generically scales a texture via nearest-neighbor. */
		CDirectX12TexturePixelScaler						m_tpsScaler;
		/** 2-Pass high-quality texture resampler. */
		CDirectX12Resampler									m_rsResampler;
		/** Generically renders a texture to the backbuffer using bilinear sampling. */
		CDirectX12TextureRenderer							m_trRenderer;

		/** Index texture (R16_UNORM, DEFAULT heap). */
		std::unique_ptr<CDirectX12Texture>					m_tIndex;
		/** 512x1 LUT texture (R32G32B32A32_FLOAT, DEFAULT heap). */
		std::unique_ptr<CDirectX12Texture>					m_tLut;
		/** Initial floating-point render target. */
		std::unique_ptr<CDirectX12Resource>					m_rtInitial;
		/** Gamma pass render target. */
		std::unique_ptr<CDirectX12Resource>					m_rtGamma;
		/** Phosphor decay target. */
		std::unique_ptr<CDirectX12Resource>					m_rtPhosphorTarget;
		/** Intermediate resample floating-point render target for passing to the screen composite. */
		std::unique_ptr<CDirectX12Resource>					m_rtResampled;
		
		/** Screen-space quad vertex buffer. */
		std::unique_ptr<CDirectX12Resource>					m_vbPass1;
		D3D12_VERTEX_BUFFER_VIEW							m_vbView1;

		// Upload Heaps for GPU copies.
		std::unique_ptr<CDirectX12Resource>					m_rIndexUpload;
		std::unique_ptr<CDirectX12Resource>					m_rLutUpload;

		// Command execution per-frame.
		std::unique_ptr<CDirectX12CommandAllocator>			m_caAllocator;
		std::unique_ptr<CDirectX12GraphicsCommandList>		m_gclCommandList;

		// Pipeline state objects and signatures.
		std::unique_ptr<CDirectX12RootSignature>			m_rsRootSignature;
		std::unique_ptr<CDirectX12PipelineState>			m_psoIdxToColor;

		// Descriptor Heaps.
		std::unique_ptr<CDirectX12DescriptorHeap>			m_dhSrvHeap;
		std::unique_ptr<CDirectX12DescriptorHeap>			m_dhGammaRtv;
		std::unique_ptr<CDirectX12DescriptorHeap>			m_dhPhosphorRtv;
		std::unique_ptr<CDirectX12DescriptorHeap>			m_dhRtvHeap;
		std::unique_ptr<CDirectX12DescriptorHeap>			m_dhSamplerHeap;
		UINT												m_uiSrvDescriptorSize = 0;
		UINT												m_uiRtvDescriptorSize = 0;
		UINT												m_uiSamplerDescriptorSize = 0;

		/** The palette look-up table. */
		std::vector<float>									m_vLut;
		/** The output buffer. */
		std::vector<uint8_t>								m_vOutputBuffer;
		
		/** Current selected gamma curve. */
		CNesPalette::LSN_GAMMA								m_gGamma = CNesPalette::LSN_G_NONE;

		/** Phosphor variables. */
		float												m_fFps = 60.098812103271484375f;					/**< The FPS. */
		float												m_fInitPhosphorDecay = 0.25f;						/**< Max phosphor level. */
		float												m_fPhosphorDecayRateRed = 0.401767850f;				/**< Red phosphor decay rate. */
		float												m_fPhosphorDecayRateGreen = 0.663420439f;			/**< Green phosphor decay rate. */
		float												m_fPhosphorDecayRateBlue = 0.193711475f;			/**< Blue phosphor decay rate. */
		float												m_fPhosphorDecayTime = 1.79113161563873291015625f;	/**< The time it takes for the phosphors to decay to 0.001. */
		bool												m_bEnablePhosphorDecay = true;						/**< Enable phosphor decay? */

		uint32_t											m_ui32SrcW = 0;
		uint32_t											m_ui32SrcH = 0;
		uint32_t											m_ui32RsrcW = 0;
		uint32_t											m_ui32RsrcH = 0;
		uint32_t											m_ui32ResampledTargetW = 0;
		uint32_t											m_ui32ResampledTargetH = 0;
		uint32_t											m_ui32VertSharpness = 2;
		uint32_t											m_ui32HorSharness = 2;
		bool												m_bUse16BitInitialTarget = true;
		bool												m_bUseHighQualityResampler = true;
		bool												m_bValidState = false;
		bool												m_bUpdatePalette = true;
			

		// == Functions.
		/**
		 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
		 * 
		 * \return Returns true on success.
		 */
		bool												EnsureSizeAndResources();

		/**
		 * \brief Updates the 512-entry float RGBA LUT in the upload heap.
		 * 
		 * \return Returns true on success.
		 */
		bool												UpdateLut();

		/**
		 * \brief Uploads the 16-bit PPU indices to the upload heap.
		 * 
		 * \param _pui16Idx Source pointer to the index image (row-major).
		 * \param _ui32W Image width in pixels.
		 * \param _ui32H Image height in pixels.
		 * \param _ui32SrcPitch Source pitch in bytes; pass 0 for tightly packed.
		 * \return Returns true on success.
		 */
		bool												UploadIndices( const uint16_t * _pui16Idx, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32SrcPitch = 0 );

		/**
		 * \brief Ensures pixel shaders, vertex shaders, and PSOs are created.
		 * 
		 * \return Returns true if all shaders are ready.
		 */
		bool												EnsureShaders();

		/**
		 * \brief Compiles an HLSL shader using dynamically loaded d3dcompiler_47.dll.
		 *
		 * \param _pcszSource Null-terminated HLSL source code.
		 * \param _pcszEntry Null-terminated entry-point function name (e.g., "main").
		 * \param _pcszProfile Null-terminated profile (e.g., "ps_5_0").
		 * \param _vOutByteCode Output vector to receive the compiled bytecode (DWORD stream).
		 * \return Returns true if compilation succeeded and bytecode was produced.
		 */
		bool												CompileHlsl( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<uint8_t> &_vOutByteCode );

		/**
		 * \brief Releases size-dependent resources.
		 */
		void												ReleaseSizeDependents();

		/**
		 * \brief Renders the three-pass pipeline to the backbuffer with a black border outside _rOutput.
		 * 
		 * \param _rOutput The destination rectangle in client pixels where the NES image should appear.
		 * \return Returns true if the draw succeeded; false on failure.
		 */
		bool												Render( const lsw::LSW_RECT &_rOutput );

	private :
		typedef CDx12FilterBase								CParent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12
