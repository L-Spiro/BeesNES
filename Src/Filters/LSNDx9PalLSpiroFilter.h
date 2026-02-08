#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of an NTSC filter.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "../GPU/DirectX9/LSNDirectX9PixelShader.h"
#include "../GPU/DirectX9/LSNDirectX9RenderTarget.h"
#include "../GPU/DirectX9/LSNDirectX9Texture.h"
#include "../GPU/DirectX9/LSNDirectX9VertexBuffer.h"
#include "LSNDx9FilterBase.h"
#include "LSNLSpiroPalFilterBase.h"

#include <mutex>


namespace lsn {

	/**
	 * Class CDx9PalLSpiroFilter
	 * \brief My own implementation of an NTSC filter.
	 *
	 * Description: My own implementation of an NTSC filter.
	 */
	class CDx9PalLSpiroFilter : public CLSpiroPalFilterBase, public CDx9FilterBase {
	public :
		CDx9PalLSpiroFilter();
		virtual ~CDx9PalLSpiroFilter();
		
		
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
			return m_ui32SrcH ? std::min<uint32_t>( s_dgsState.rScreenRect.Height() / m_ui32SrcH, GetVertSharpness() ) : 1;
		}

		/**
		 * Gets the actual horizontal sharpness.  Mipmaps are used when the display image is small enough.
		 * 
		 * \return Returns the image scale to use, accounting for mipmaps.
		 **/
		inline uint32_t										GetActualHorSharpness() const {
			return m_ui32SrcW ? std::min<uint32_t>( s_dgsState.rScreenRect.Width() / m_ui32SrcW, GetHorSharpness() ) : 1;
		}

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
		 * \param _ui32DispHeight The display area height
		 * \return Returns a pointer to the filtered output buffer.
		 */
		virtual uint8_t *									ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/,
			int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) override;

		/**
		 * Gets the PPU output format.
		 *
		 * \return Returns the output format from the PPU/input format for this filter.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT			InputFormat() const { return CDisplayClient::LSN_POF_9BIT_PALETTE; }

		/**
		 * If true, the PPU is requested to provide a frame that has been flipped vertically.
		 *
		 * \return Returns true to receive a vertically flipped image from the PPU, false to receive an unflipped image.
		 */
		virtual bool										FlipInput() const { return false; }

		/**
		 * Gets a pointer to the output buffer.
		 *
		 * \return Returns a pointer to the output buffer.
		 */
		virtual uint8_t *									OutputBuffer() { return CurTarget(); }

		/**
		 * Gets the bits-per-pixel of the final output.  Will be 16, 24, or 32.
		 *
		 * \return Returns the bits-per-pixel of the final output.
		 */
		virtual uint32_t									OutputBits() const { return 32; }

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
		/** The DirectX 9 device wrapper (non-owning). */
		CDirectX9Device *									m_pdx9dDevice = nullptr;
		/** Initial floating-point render target (same size as indices). */
		//std::unique_ptr<CDirectX9RenderTarget>				m_rtInitial;
		/** Software target texture (A32B32G32R32F, MANAGED), source for GPU up-scale. */
		std::unique_ptr<CDirectX9Texture>					m_tSrc;
		/** Scanlined floating-point render target (height = source height Å~ factor). */
		std::unique_ptr<CDirectX9RenderTarget>				m_rtScanlined;
		/** Dynamic screen-space quad vertex buffer (XYZRHW|TEX1, 4 vertices). */
		std::unique_ptr<CDirectX9VertexBuffer>				m_vbQuad;
		/** Pixel shader: vertical nearest-neighbor upscale (pass 2). */
		std::unique_ptr<CDirectX9PixelShader>				m_psVerticalNN;
		/** Pixel shader: copy pass (pass 3). */
		std::unique_ptr<CDirectX9PixelShader>				m_psCopy;
		/** Source width in pixels. */
		uint32_t											m_ui32SrcW = 0;
		/** Source height in pixels. */
		uint32_t											m_ui32SrcH = 0;
		/** Created resource width. */
		uint32_t											m_ui32RsrcW = 0;
		/** Created resource height. */
		uint32_t											m_ui32RsrcH = 0;
		/** Vertical sharpness factor. */
		uint32_t											m_ui32VertSharpness = 3;
		/** Horizontal sharpness factor. */
		uint32_t											m_ui32HorSharness = 1;
		/** The locked rectangle into which the frame is drawn during ths osftware phase. */
		D3DLOCKED_RECT										m_lrRect{};
		/** Use a 16-bit initial render target? */
		bool												m_bUse16BitInitialTarget = true;
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
		 * Releases/creates the index texture, both FP render targets, and quad vertex buffer as needed.
		 *
		 * \return Returns true on success.
		 */
		bool												EnsureSizeAndResources();

		/**
		 * Creates the upload texture.
		 * 
		 * \return Returns true on success.
		 **/
		bool												PrepaerSrcTexture();

		/**
		 * \brief Ensures pixel shaders (indexÅ®color, vertical NN, copy) are created.
		 *
		 * Compiles the HLSL entry points with D3DX at runtime and creates pixel shaders from bytecode.
		 * If D3DX cannot be loaded, this function returns false.
		 *
		 * \return Returns true if all shaders are ready.
		 */
		bool												EnsureShaders();

		/**
		 * \brief Compiles an HLSL pixel shader using dynamically loaded D3DX.
		 *
		 * \param _pcszSource Null-terminated HLSL source code.
		 * \param _pcszEntry Null-terminated entry-point function name (e.g., "main").
		 * \param _pcszProfile Null-terminated profile (e.g., "ps_2_0").
		 * \param _vOutByteCode Output vector to receive the compiled bytecode (DWORD stream).
		 * \param _piInclude Optional #include handler.
		 * \return Returns true if compilation succeeded and bytecode was produced.
		 */
		bool												CompileHlslPs( const char * _pcszSource, const char * _pcszEntry, const char * _pcszProfile, std::vector<DWORD> &_vOutByteCode, ID3DXInclude * _piInclude );

		/**
		 * \brief Releases size-dependent resources (index texture, FP RTs, quad VB).
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
		typedef CDx9FilterBase								CParent;
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.

}	// namespace lsn

#endif	// #ifdef LSN_DX9
