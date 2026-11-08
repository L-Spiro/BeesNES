/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for GPU filters.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../ColorSpace/LSNColorSpace.h"
#include "../Display/LSNCaptureBuffer.h"
#include "../Utilities/LSNUtilities.h"
#include "LSNFilterBase.h"
#include "LSNNesPalette.h"
#include "LSNResamplerBase.h"

#include <string>
#include <vector>


namespace lsn {

	/**
	 * Class CGpuFilterBase
	 * \brief The base class for GPU filters.
	 *
	 * Description: The base class for GPU filters.
	 */
	class CGpuFilterBase : public CFilterBase {
	public :
		CGpuFilterBase();
		virtual ~CGpuFilterBase();


		// == Enumerations.
		/** The bands of output scales (the ratio of the output size to the console's native size), each with its own convolution filter. */
		enum LSN_CONVOLUTION_BAND : uint32_t {
			LSN_CB_LOW,																							/**< Below 2.5 times. */
			LSN_CB_MEDIUM,																						/**< At least 2.5 and below 3.5 times. */
			LSN_CB_HIGH,																						/**< At least 3.5 and below 4.5 times. */
			LSN_CB_MAX,																							/**< At least 4.5 times. */
			LSN_CB_TOTAL,																						/**< The number of bands. */
		};


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
		 * Gets the PPU output format.
		 *
		 * \return Returns the output format from the PPU/input format for this filter.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			InputFormat() const override { return CDisplayClient::LSN_POF_9BIT_PALETTE; }

		/**
		 * Gets the bits-per-pixel of the final output.  Will be 16, 24, or 32.
		 *
		 * \return Returns the bits-per-pixel of the final output.
		 */
		virtual uint32_t									OutputBits() const override { return 32; }

		/**
		 * Gets a pointer to the output buffer.
		 *
		 * \return Returns a pointer to the output buffer.
		 */
		virtual uint8_t *									OutputBuffer() override { return CurTarget(); }

		/**
		 * If this is a GPU filter, some of the pipeline is different, and extra steps to manage the filter are needed.
		 * 
		 * \return Returns true.
		 **/
		virtual bool										IsGpuFilter() const override { return true; }

		/**
		 * Informs the filter of a window resize.
		 **/
		virtual void										FrameResize() {}

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
		 * \param _i32DispLeft The display area left.
		 * \param _i32DispTop The display area top.
		 * \param _ui32DispWidth The display area width.
		 * \param _ui32DispHeight The display area height.
		 * \return Returns a pointer to the filtered output buffer.
		 */
		virtual uint8_t *									ApplyFilter( uint8_t * /*_pui8Input*/, uint32_t &/*_ui32Width*/, uint32_t &/*_ui32Height*/, uint16_t &/*_ui16BitDepth*/, uint32_t &/*_ui32Stride*/, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/,
			int32_t /*_i32DispLeft*/, int32_t /*_i32DispTop*/, uint32_t /*_ui32DispWidth*/, uint32_t /*_ui32DispHeight*/ ) { return nullptr; }

		/**
		 * Sets the gamma curve to be applied in the pipeline.
		 *
		 * \param _gGamma The gamma preset.
		 */
		inline void											SetGamma( CNesPalette::LSN_GAMMA _gGamma ) { m_gGamma = _gGamma; }
		
		/**
		 * Gets the specified gamma curve for the pipeline.
		 *
		 * \return Returns the currently set gamma preset.
		 */
		inline CNesPalette::LSN_GAMMA						GetGamma() const { return m_gGamma; }
		
		/**
		 * Resolves auto-gamma settings to a concrete gamma curve.
		 *
		 * \return Returns the effective gamma curve to use during rendering.
		 */
		virtual inline CNesPalette::LSN_GAMMA				GetEffectiveGamma() const { return m_gGamma == CNesPalette::LSN_G_AUTO ? CNesPalette::LSN_G_CRT2 : m_gGamma; }

		/**
		 * Sets the color space of the emulated CRT.  The color is converted (in linear light, after the gamma curve) from this color space
		 *	to that of the monitor (see SetDisplaySettings()).
		 *
		 * \param _cspColorSpace The color space of the CRT.
		 */
		inline void											SetColorSpace( CColorSpace::LSN_COLOR_SPACE _cspColorSpace ) {
			m_cspColorSpace = (_cspColorSpace < CColorSpace::LSN_CSP_TOTAL) ? _cspColorSpace : CColorSpace::LSN_CSP_SRGB;
			UpdateColorConversion();
		}

		/**
		 * Gets the color space of the emulated CRT.
		 *
		 * \return Returns the color space of the emulated CRT.
		 */
		inline CColorSpace::LSN_COLOR_SPACE					GetColorSpace() const { return m_cspColorSpace; }

		/**
		 * Sets the settings of the host display (the monitor's color space, gamma curve, white-point adaptation, gamut mapping, and
		 *	output range).
		 *
		 * \param _dsSettings The display settings.
		 */
		inline void											SetDisplaySettings( const CColorSpace::LSN_DISPLAY_SETTINGS &_dsSettings ) {
			m_dsDisplay = _dsSettings;
			UpdateColorConversion();
		}

		/**
		 * Gets the settings of the host display.
		 *
		 * \return Returns the settings of the host display.
		 */
		inline const CColorSpace::LSN_DISPLAY_SETTINGS &	GetDisplaySettings() const { return m_dsDisplay; }

		/**
		 * Gets the color-space conversion matrix applied after the gamma curve (from the CRT's color space to the monitor's).
		 *
		 * \return Returns the color-space conversion matrix.
		 */
		inline const CColorSpace::LSN_COLOR_MATRIX &		GetColorMatrix() const { return m_cmColorSpace; }

		/**
		 * Sets the parameters of the LSN_G_CRT1 gamma curve.
		 *
		 * \param _fWhite The white level (Lw).
		 * \param _fBrightness The brightness (black lift, B).
		 */
		inline void											SetCrt1Parms( float _fWhite, float _fBrightness ) {
			m_fCrt1White = _fWhite;
			m_fCrt1Brightness = _fBrightness;
		}

		/**
		 * Gets the white level (Lw) of the LSN_G_CRT1 gamma curve.
		 *
		 * \return Returns the white level of the LSN_G_CRT1 gamma curve.
		 */
		inline float										GetCrt1White() const { return m_fCrt1White; }

		/**
		 * Gets the brightness (black lift, B) of the LSN_G_CRT1 gamma curve.
		 *
		 * \return Returns the brightness of the LSN_G_CRT1 gamma curve.
		 */
		inline float										GetCrt1Brightness() const { return m_fCrt1Brightness; }

		/**
		 * Gets the parameters of the gamma pass for a given (effective) gamma curve.
		 *
		 * \param _gEffGamma The effective gamma curve (from GetEffectiveGamma()).
		 * \return Returns the parameters of the gamma pass.
		 */
		inline CColorSpace::LSN_GAMMA_PASS					GammaPass( CNesPalette::LSN_GAMMA _gEffGamma ) const {
			CColorSpace::LSN_GAMMA_PASS gpRet;
			gpRet.gGamma = _gEffGamma;
			gpRet.fCrtWhite = m_fCrt1White;
			gpRet.fCrtBrightness = m_fCrt1Brightness;
			gpRet.cmMatrix = m_cmColorSpace;
			gpRet.fLuma[0] = m_fDisplayLuma[0];
			gpRet.fLuma[1] = m_fDisplayLuma[1];
			gpRet.fLuma[2] = m_fDisplayLuma[2];
			gpRet.gmGamutMapping = m_dsDisplay.gmGamutMapping;
			return gpRet;
		}

		/**
		 * Gets the conversion from linear light to the values sent to the monitor (applied by the final render).
		 *
		 * \return Returns the output transform.
		 */
		inline CColorSpace::LSN_OUTPUT_TRANSFORM			OutputTransform() const {
			CColorSpace::LSN_OUTPUT_TRANSFORM otRet;
			otRet.gGamma = m_dsDisplay.gGamma;
			otRet.orOutputRange = m_dsDisplay.orOutputRange;
			return otRet;
		}

		/**
		 * Determines whether the gamma pass (gamma curve and/or color-space conversion) must run.
		 *
		 * \param _gEffGamma The effective gamma curve (from GetEffectiveGamma()).
		 * \return Returns true if a gamma curve or a non-identity color-space conversion is to be applied.
		 */
		inline bool											NeedsGammaPass( CNesPalette::LSN_GAMMA _gEffGamma ) const { return _gEffGamma != CNesPalette::LSN_G_NONE || !m_cmColorSpace.IsIdentity(); }

		/**
		 * Sets a hue offset that is added to the filter's own hue (normally filled from a TV preset; see CColorSpace::HueOffset()).  Filters
		 *	that decode a composite signal (the L. Spiro, Blargg, and NTSC CRT Full filters) override this; the rest ignore it.
		 *
		 * \param _fHueOffset The hue offset, in radians.  Positive values rotate the demodulation axes forward.
		 */
		virtual void										SetHueOffset( float /*_fHueOffset*/ ) {}

		/**
		 * Sets whether the internal scaling (the sharpness factors) is dynamic.  If dynamic, each factor is one level above the ratio of
		 *	the output size to the native size, capped at the set sharpness factor; otherwise the set factors are always used.
		 *
		 * \param _bDynamic If true, the internal scaling is dynamic.
		 */
		inline void											SetDynamicScaling( bool _bDynamic ) { m_bDynamicScaling = _bDynamic; }

		/**
		 * Gets whether the internal scaling is dynamic.
		 *
		 * \return Returns true if the internal scaling is dynamic.
		 */
		inline bool											GetDynamicScaling() const { return m_bDynamicScaling; }

		/**
		 * Sets the convolution filter used for one band of output scales.
		 *
		 * \param _cbBand The band of output scales.
		 * \param _ffFilter The filter, or CResamplerBase::LSN_FF_TOTAL for none (hardware bilinear filtering).
		 */
		inline void											SetConvolutionFilter( LSN_CONVOLUTION_BAND _cbBand, CResamplerBase::LSN_FILTER_FUNCS _ffFilter ) {
			if ( _cbBand < LSN_CB_TOTAL ) { m_ffConvolution[_cbBand] = (_ffFilter < CResamplerBase::LSN_FF_TOTAL) ? _ffFilter : CResamplerBase::LSN_FF_TOTAL; }
		}

		/**
		 * Gets the convolution filter used for one band of output scales.
		 *
		 * \param _cbBand The band of output scales.
		 * \return Returns the filter, or CResamplerBase::LSN_FF_TOTAL for none.
		 */
		inline CResamplerBase::LSN_FILTER_FUNCS				GetConvolutionFilter( LSN_CONVOLUTION_BAND _cbBand ) const { return (_cbBand < LSN_CB_TOTAL) ? m_ffConvolution[_cbBand] : CResamplerBase::LSN_FF_TOTAL; }

		/**
		 * Gets the convolution filter to use for a given ratio of the output size to the console's native size:  below 2.5 (low),
		 *	below 3.5 (medium), below 4.5 (high), otherwise (max).
		 *
		 * \param _fResolutionFactor The ratio of the output size to the native size.
		 * \return Returns the filter, or CResamplerBase::LSN_FF_TOTAL for none.
		 */
		inline CResamplerBase::LSN_FILTER_FUNCS				ConvolutionFilterForScale( float _fResolutionFactor ) const {
			if ( _fResolutionFactor < 2.5f ) { return m_ffConvolution[LSN_CB_LOW]; }
			if ( _fResolutionFactor < 3.5f ) { return m_ffConvolution[LSN_CB_MEDIUM]; }
			if ( _fResolutionFactor < 4.5f ) { return m_ffConvolution[LSN_CB_HIGH]; }
			return m_ffConvolution[LSN_CB_MAX];
		}

		/**
		 * Sets the vertical sharpness factor (how much the image is scaled vertically before the final resampling).
		 *
		 * \param _ui32Factor The vertical sharpness (at least 1).
		 */
		virtual void										SetVertSharpness( uint32_t _ui32Factor ) = 0;

		/**
		 * Gets the vertical sharpness factor.
		 *
		 * \return Returns the vertical sharpness factor.
		 */
		virtual uint32_t									GetVertSharpness() const = 0;

		/**
		 * Sets the horizontal sharpness factor (how much the image is scaled horizontally before the final resampling).
		 *
		 * \param _ui32Factor The horizontal sharpness (at least 1).
		 */
		virtual void										SetHorSharpness( uint32_t _ui32Factor ) = 0;

		/**
		 * Gets the horizontal sharpness factor.
		 *
		 * \return Returns the horizontal sharpness factor.
		 */
		virtual uint32_t									GetHorSharpness() const = 0;

		/**
		 * Sets whether the final composite render uses the high-quality 2-pass resampler.
		 *
		 * \param _bUse If true, the high-quality resampler is used.
		 */
		virtual void										SetUseHighQualityResampler( bool _bUse ) = 0;

		/**
		 * Gets whether the final composite render uses the high-quality 2-pass resampler.
		 *
		 * \return Returns true if the high-quality resampler is used.
		 */
		virtual bool										GetUseHighQualityResampler() const = 0;
		
		/**
		 * Enables or disables the phosphor decay effect.
		 *
		 * \param _bEnable If true, phosphor decay is simulated.
		 */
		inline void											SetPhosphorDecayEnable( bool _bEnable ) { m_bEnablePhosphorDecay = _bEnable; }
		
		/**
		 * Gets whether the phosphor decay effect is enabled.
		 *
		 * \return Returns true if phosphor decay is enabled.
		 */
		inline bool											GetPhosphorDecayEnable() const { return m_bEnablePhosphorDecay; }

		/**
		 * Sets the base time period for phosphor decay calculations.
		 *
		 * \param _fTime The decay period in seconds.
		 */
		void												SetPhosphorDecayPeriod( float _fTime = DefaultPhosphorDecayDuration() ) {
			m_fPhosphorDecayTime = _fTime;
			UpdatePhosphorRates();
		}

		/**
		 * Sets the framerate used for calculating the per-frame phosphor decay multipliers.
		 *
		 * \param _fFps The framerate in frames per second.
		 */
		void												SetFps( float _fFps = 60.098812103271484375f ) {
			m_fFps = _fFps;
			UpdatePhosphorRates();
		}

		/**
		 * Sets the initial decay level applied immediately when a pixel loses intensity.
		 *
		 * \param _fLevel The decay scalar.
		 */
		void												SetPhosphorDecayLevel( float _fLevel = DefaultPhosphorDecayIntensity() ) {
			m_fInitPhosphorDecay = _fLevel;
			UpdatePhosphorRates();
		}

		/**
		 * Sets the decay times of the red and blue phosphors relative to that of the green phosphor.
		 *
		 * \param _fRed The red multiplier (typically around 0.45).
		 * \param _fBlue The blue multiplier (typically around 0.25).
		 */
		void												SetPhosphorDecayMultipliers( float _fRed, float _fBlue ) {
			m_fPhosphorRedMultiplier = _fRed;
			m_fPhosphorBlueMultiplier = _fBlue;
			UpdatePhosphorRates();
		}

		/**
		 * Gets the decay time of the red phosphor relative to that of the green phosphor.
		 *
		 * \return Returns the red multiplier.
		 */
		inline float										GetPhosphorDecayRedMultiplier() const { return m_fPhosphorRedMultiplier; }

		/**
		 * Gets the decay time of the blue phosphor relative to that of the green phosphor.
		 *
		 * \return Returns the blue multiplier.
		 */
		inline float										GetPhosphorDecayBlueMultiplier() const { return m_fPhosphorBlueMultiplier; }

		/**
		 * Gets the phosphor decay time.
		 *
		 * \return Returns the time it takes the phosphors to decay to 0.001.
		 */
		inline float										GetPhosphorDecayPeriod() const { return m_fPhosphorDecayTime; }

		/**
		 * Gets the initial phosphor decay level.
		 *
		 * \return Returns the strength of the phosphor decay.
		 */
		inline float										GetPhosphorDecayLevel() const { return m_fInitPhosphorDecay; }

		/**
		 * Gets the default phosphor decay intensity.
		 *
		 * \return Returns a good default value for phosphor-decay intensity.
		 */
		static inline float constexpr						DefaultPhosphorDecayIntensity() { return 0.0508760847151279449462890625f * 2.0f; }

		/**
		 * Gets the default phosphor decay duration.
		 *
		 * \return Returns a good default value for phosphor-decay duration.
		 */
		static inline float constexpr						DefaultPhosphorDecayDuration() { return 1.79113161563873291015625f / 7.0f; }

		/**
		 * Sets the buffer into which the next rendered frames are captured.  The swap chain (the area of it showing the console screen) is
		 *	captured.  GPU captures are read back after the GPU finishes with them, which can be a frame later.
		 *
		 * \param _pcbTarget The buffer to fill with each rendered frame, or nullptr to stop capturing.
		 * \param _ui64PpuFrame The PPU frame about to be rendered.
		 */
		inline void											SetCaptureTarget( LSN_CAPTURE_BUFFER * _pcbTarget, uint64_t _ui64PpuFrame ) {
			m_pcbCapture = _pcbTarget;
			m_ui64CaptureFrame = _ui64PpuFrame;
		}

		/**
		 * Gets the buffer into which rendered frames are being captured.
		 *
		 * \return Returns the buffer into which rendered frames are being captured or nullptr if not capturing.
		 */
		inline LSN_CAPTURE_BUFFER *							CaptureTarget() const { return m_pcbCapture; }

		/**
		 * Gets the reason the last capture failed.
		 *
		 * \return Returns a description of why the last attempt to capture a frame failed, or an empty string if it did not fail.
		 */
		inline const std::wstring &							CaptureError() const { return m_wsCaptureError; }

		/**
		 * Reads any capture that is still on the GPU into the capture target now.  GPU captures can arrive a frame late; call this when
		 *	capturing stops so that the last rendered frame is not lost.
		 */
		virtual void										FlushCapture() {}

	protected :
		// == Members.
		CNesPalette::LSN_GAMMA								m_gGamma = CNesPalette::LSN_G_AUTO;					/**< The requested gamma curve. */
		CColorSpace::LSN_COLOR_MATRIX						m_cmColorSpace;										/**< The color-space conversion applied after the gamma curve (identity = none). */
		CColorSpace::LSN_COLOR_SPACE						m_cspColorSpace = CColorSpace::LSN_CSP_SRGB;		/**< The color space of the emulated CRT. */
		CColorSpace::LSN_DISPLAY_SETTINGS					m_dsDisplay;										/**< The settings of the host display. */
		float												m_fDisplayLuma[3] = { 2.12639006e-01f, 7.15168679e-01f, 7.21923187e-02f };	/**< The luminance coefficients of the monitor's color space. */
		float												m_fCrt1White = 1.0f;								/**< The white level (Lw) of the LSN_G_CRT1 curve. */
		float												m_fCrt1Brightness = 0.0181f * 8.0f;					/**< The brightness (black lift, B) of the LSN_G_CRT1 curve. */
		float												m_fPhosphorRedMultiplier = 0.45f;					/**< The red phosphor's decay time relative to the green phosphor's. */
		float												m_fPhosphorBlueMultiplier = 0.25f;					/**< The blue phosphor's decay time relative to the green phosphor's. */
		CResamplerBase::LSN_FILTER_FUNCS					m_ffConvolution[LSN_CB_TOTAL] = {					/**< The convolution filter of each band of output scales (LSN_FF_TOTAL = none). */
			CResamplerBase::LSN_FF_CARDINALSPLINEUNIFORM,
			CResamplerBase::LSN_FF_ROBIDOUXSHARP,
			CResamplerBase::LSN_FF_LINEAR,
			CResamplerBase::LSN_FF_TOTAL,
		};
		bool												m_bDynamicScaling = true;							/**< If true, the internal scaling follows the output size (capped at the sharpness factors). */
		float												m_fFps = 60.098812103271484375f;					/**< The refresh rate used for timing. */
		float												m_fInitPhosphorDecay = 0.25f;						/**< The base decay scalar applied immediately. */
		float												m_fPhosphorDecayRateRed = 0.401767850f;				/**< The frame-to-frame red decay multiplier. */
		float												m_fPhosphorDecayRateGreen = 0.663420439f;			/**< The frame-to-frame green decay multiplier. */
		float												m_fPhosphorDecayRateBlue = 0.193711475f;			/**< The frame-to-frame blue decay multiplier. */
		float												m_fPhosphorDecayTime = 1.79113161563873291015625f;	/**< The decay curve half-life time base. */
		bool												m_bEnablePhosphorDecay = true;						/**< True if the phosphor pass should execute. */
		LSN_CAPTURE_BUFFER *								m_pcbCapture = nullptr;								/**< The buffer into which rendered frames are captured, or nullptr. */
		uint64_t											m_ui64CaptureFrame = 0;								/**< The PPU frame being rendered, for the capture. */
		std::wstring										m_wsCaptureError;									/**< Why the last capture failed, or empty. */


		// == Functions.
		/**
		 * Recomputes the per-frame phosphor decay multipliers of each channel.
		 */
		void												UpdatePhosphorRates() {
			m_fPhosphorDecayRateGreen = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime, m_fFps ));
			m_fPhosphorDecayRateRed = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * m_fPhosphorRedMultiplier, m_fFps ));
			m_fPhosphorDecayRateBlue = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * m_fPhosphorBlueMultiplier, m_fFps ));
		}

		/**
		 * Recomputes the color-space conversion (from the CRT's color space to the monitor's) and the monitor's luminance coefficients.
		 */
		void												UpdateColorConversion() {
			m_cmColorSpace = CColorSpace::ConversionMatrix( m_cspColorSpace, m_dsDisplay.cspColorSpace, m_dsDisplay.waWhiteAdaptation );
			CColorSpace::LuminanceCoefficients( m_dsDisplay.cspColorSpace, m_fDisplayLuma );
		}

		
	};

}	// namespace lsn
