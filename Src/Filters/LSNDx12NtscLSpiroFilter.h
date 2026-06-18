#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of an NTSC filter.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../GPU/DirectX12/LSNDirectX12CommandAllocator.h"
#include "../GPU/DirectX12/LSNDirectX12DescriptorHeap.h"
#include "../GPU/DirectX12/LSNDirectX12GraphicsCommandList.h"
#include "../GPU/DirectX12/LSNDirectX12Resampler.h"
#include "../GPU/DirectX12/LSNDirectX12Resource.h"
#include "../GPU/DirectX12/LSNDirectX12TexturePixelScaler.h"
#include "../GPU/DirectX12/LSNDirectX12TextureRenderer.h"
#include "../GPU/DirectX12/LSNDirectX12TextureUploader.h"
#include "LSNDx12FilterBase.h"
#include "LSNLSpiroNtscFilterBase.h"

#include <mutex>
#include <vector>


namespace lsn {

	/**
	 * Class CDx12NtscLSpiroFilter
	 * \brief My own implementation of an NTSC filter.
	 *
	 * Description: My own implementation of an NTSC filter.
	 */
	class CDx12NtscLSpiroFilter : public CLSpiroNtscFilterBase, public CDx12FilterBase {
	public :
		CDx12NtscLSpiroFilter();
		virtual ~CDx12NtscLSpiroFilter();
		
		
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
		 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.
		 *
		 * \param _pui8Input The buffer to be filtered, which will be a pointer to one of the buffers returned by OutputBuffer() previously.
		 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width, in pixels, of the result.
		 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height, in pixels, of the result.
		 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth of the result.
		 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes, of the result.
		 * \param _ui64PpuFrame The PPU frame associated with the input data.
		 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
		 * \param _i32DispLeft The display area left.
		 * \param _i32DispTop The display area top.
		 * \param _ui32DispWidth The display area width.
		 * \param _ui32DispHeight The display area height
		 * \return Returns a pointer to the filtered output buffer.
		 */
		virtual uint8_t *									ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle,
			int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) override;

		/**
		 * Gets the PPU output format.
		 *
		 * \return Returns the output format from the PPU/input format for this filter.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			InputFormat() const override { return CDisplayClient::LSN_POF_9BIT_PALETTE; }

		/**
		 * If true, the PPU is requested to provide a frame that has been flipped vertically.
		 *
		 * \return Returns true to receive a vertically flipped image from the PPU, false to receive an unflipped image.
		 */
		virtual bool										FlipInput() const override { return false; }

		/**
		 * Gets a pointer to the output buffer.
		 *
		 * \return Returns a pointer to the output buffer.
		 */
		virtual uint8_t *									OutputBuffer() override { return CurTarget(); }

		/**
		 * Gets the bits-per-pixel of the final output.  Will be 16, 24, or 32.
		 *
		 * \return Returns the bits-per-pixel of the final output.
		 */
		virtual uint32_t									OutputBits() const override { return 32; }

		/**
		 * Called when the filter is about to become active.
		 */
		virtual void										Activate() override;

		/**
		 * Called when the filter is about to become inactive.
		 */
		virtual void										DeActivate() override;

		/**
		 * Informs the filter of a window resize.
		 **/
		virtual void										FrameResize() override;

		/**
		 * Sets the number of worker threads used by the filter.
		 *
		 * \param _stThreads Number of worker threads to use.  0 disables worker threads.
		 */
		void												SetWorkerThreadCount( size_t _stThreads );

		/**
		 * Gets the number of worker threads used by the filter.
		 *
		 * \return Returns the total number of worker threads used by the filter.
		 */
		inline size_t										WorkerThreadCount() const { return m_stWorkerThreadCount; }


	protected :
		// == Types.
		/** A per-frame work package shared by all threads. */
		struct LSN_JOB {
			const uint8_t *									pui8Pixels = nullptr;								/**< The input 9-bit pixel array. */
			uint64_t										ui64RenderStartCycle = 0;							/**< The render cycle at the start of the frame. */
			size_t											stThreads = 1;										/**< Total number of threads for the job, including the calling thread. */
		};


		// == Members.
		/** The DirectX 12 device wrapper (non-owning). */
		CDirectX12Device *									m_pdx12dDevice = nullptr;
		
		/** Generically uploads CPU texel arrays to GPU textures. */
		CDirectX12TextureUploader							m_tuUploader;
		/** Generically scales a texture via nearest-neighbor and applies gamma. */
		CDirectX12TexturePixelScaler						m_tpsScaler;
		/** 2-Pass high-quality texture resampler. */
		CDirectX12Resampler									m_rsResampler;
		/** Generically renders a texture to the backbuffer using bilinear sampling. */
		CDirectX12TextureRenderer							m_trRenderer;

		/** Intermediate resample floating-point render target for passing to the screen composite. */
		std::unique_ptr<CDirectX12Resource>					m_rtResampled;

		/** Command allocator for frame execution. */
		std::unique_ptr<CDirectX12CommandAllocator>			m_caAllocator;
		/** Command list for frame execution. */
		std::unique_ptr<CDirectX12GraphicsCommandList>		m_gclCommandList;
		/** Descriptor Heap for RTVs. */
		std::unique_ptr<CDirectX12DescriptorHeap>			m_dhRtvHeap;
		/** Descriptor size for RTVs. */
		UINT												m_uiRtvDescriptorSize = 0;

		/** Source width in pixels. */
		uint32_t											m_ui32SrcW = 0;
		/** Source height in pixels. */
		uint32_t											m_ui32SrcH = 0;
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
		/** Toggles whether the high-quality 2-pass CDirectX12Resampler handles final scaling. */
		bool												m_bUseHighQualityResampler = true;
		/** Are we in a valid state? */
		bool												m_bValidState = false;

		std::vector<std::thread>							m_vThreads;											/**< Worker threads. */
		std::mutex											m_mThreadMutex;										/**< Mutex protecting thread state. */
		std::condition_variable								m_cvGo;												/**< Signal to tell worker threads to start a job. */
		std::condition_variable								m_cvDone;											/**< Signal to tell the main thread workers have finished. */
		std::atomic<uint32_t>								m_ui32WorkersRemaining = 0;							/**< Number of workers still running the current job. */
		size_t												m_stWorkerThreadCount = 2;							/**< Total number of worker threads. */
		bool												m_bThreadsStarted = false;							/**< True if the worker threads have been created. */
		bool												m_bStopThreads = false;								/**< True if worker threads should exit. */
		uint64_t											m_ui64JobId = 0;									/**< Incremented to start a new job. */
		LSN_JOB												m_jJob;												/**< The current job. */
		std::vector<uint8_t>								m_vRgbBuffer;										/**< The output created by calling FilterFrame(). */


		// == Functions.
		/**
		 * Renders a full frame of PPU 9-bit (stored in uint16_t's) palette indices to a given 32-bit RGBX buffer.
		 * 
		 * \param _pui8Pixels The input array of 9-bit PPU outputs.
		 * \param _ui64RenderStartCycle The PPU cycle at the start of the block being rendered.
		 **/
		void												FilterFrame( const uint8_t * _pui8Pixels, uint64_t _ui64RenderStartCycle );
		
		/**
		 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
		 * 
		 * \return Returns true on success.
		 */
		bool												EnsureSizeAndResources();

		/**
		 * \brief Releases size-dependent resources.
		 */
		void												ReleaseSizeDependents();

		/**
		 * Renders the final output to the backbuffer.
		 *
		 * \param _rOutput The destination rectangle.
		 * \return Returns true if rendering succeeded.
		 */
		bool												Render( const lsw::LSW_RECT &_rOutput );

		/**
		 * \brief Starts the worker threads.
		 *
		 * Creates m_vThreads based on m_stWorkerThreadCount and resets the thread-control state.
		 * Safe to call multiple times; if threads are already started, this function does nothing.
		 */
		void												StartThreads();

		/**
		 * \brief Stops the worker threads.
		 *
		 * Signals all worker threads to exit, wakes them, joins them, clears m_vThreads, and
		 * resets thread-control state.  Safe to call multiple times; if threads are not started,
		 * this function does nothing.
		 */
		void												StopThreads();

		/**
		 * \brief The worker thread entry point.
		 *
		 * Waits for jobs signaled via m_cvGo, renders the scanline range assigned to this worker,
		 * then decrements m_ui32WorkersRemaining and notifies m_cvDone when the final worker
		 * finishes the job.
		 *
		 * \param _stThreadIdx The worker thread index in the range [1, stThreads - 1].
		 *	Index 0 is reserved for the calling thread.
		 */
		void												WorkerThread( size_t _stThreadIdx );

	private :
		typedef CDx12FilterBase								CParent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12
