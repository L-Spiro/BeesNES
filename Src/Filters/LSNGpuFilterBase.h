/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for GPU filters.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../Utilities/LSNUtilities.h"
#include "LSNFilterBase.h"
#include "LSNNesPalette.h"

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
		void												SetPhosphorDecayPeriod( float _fTime = DefaultPhophorDecayDuration() ) {
			m_fPhosphorDecayTime = _fTime;
			m_fPhosphorDecayRateGreen = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime, m_fFps ));
			m_fPhosphorDecayRateRed = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.45f, m_fFps ));
			m_fPhosphorDecayRateBlue = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.25f, m_fFps ));
		}

		/**
		 * Sets the framerate used for calculating the per-frame phosphor decay multipliers.
		 *
		 * \param _fFps The framerate in frames per second.
		 */
		void												SetFps( float _fFps = 60.098812103271484375f ) {
			m_fFps = _fFps;
			m_fPhosphorDecayRateGreen = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime, m_fFps ));
			m_fPhosphorDecayRateRed = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.45f, m_fFps ));
			m_fPhosphorDecayRateBlue = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.25f, m_fFps ));
		}

		/**
		 * Sets the initial decay level applied immediately when a pixel loses intensity.
		 *
		 * \param _fLevel The decay scalar.
		 */
		void												SetPhosphorDecayLevel( float _fLevel = DefaultPhophorDecayIntensity() ) {
			m_fInitPhosphorDecay = _fLevel;
			m_fPhosphorDecayRateGreen = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime, m_fFps ));
			m_fPhosphorDecayRateRed = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.45f, m_fFps ));
			m_fPhosphorDecayRateBlue = static_cast<float>(CUtilities::DecayMultiplier( m_fInitPhosphorDecay, 0.001f, m_fPhosphorDecayTime * 0.25f, m_fFps ));
		}

		/**
		 * Gets the default phosphor decay intensity.
		 *
		 * \return Returns a good default value for phosphor-decay intensity.
		 */
		static inline float consteval						DefaultPhophorDecayIntensity() { return 0.0508760847151279449462890625f * 2.0f; }

		/**
		 * Gets the default phosphor decay duration.
		 *
		 * \return Returns a good default value for phosphor-decay duration.
		 */
		static inline float consteval						DefaultPhophorDecayDuration() { return 1.79113161563873291015625f / 7.0f; }

	protected :
		// == Members.
		CNesPalette::LSN_GAMMA								m_gGamma = CNesPalette::LSN_G_AUTO;					/**< The requested gamma curve. */
		float												m_fFps = 60.098812103271484375f;					/**< The refresh rate used for timing. */
		float												m_fInitPhosphorDecay = 0.25f;						/**< The base decay scalar applied immediately. */
		float												m_fPhosphorDecayRateRed = 0.401767850f;				/**< The frame-to-frame red decay multiplier. */
		float												m_fPhosphorDecayRateGreen = 0.663420439f;			/**< The frame-to-frame green decay multiplier. */
		float												m_fPhosphorDecayRateBlue = 0.193711475f;			/**< The frame-to-frame blue decay multiplier. */
		float												m_fPhosphorDecayTime = 1.79113161563873291015625f;	/**< The decay curve half-life time base. */
		bool												m_bEnablePhosphorDecay = true;						/**< True if the phosphor pass should execute. */
		
	};

}	// namespace lsn
