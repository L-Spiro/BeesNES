#ifdef LSN_VULKAN1

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: My own implementation of an NTSC filter for Vulkan 1.0.
 */

#include "LSNVulkanNtscLSpiroFilter.h"
#include "../Utilities/LSNScopedNoSubnormals.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>

namespace lsn {

	// == Members.
	CVulkanNtscLSpiroFilter::CVulkanNtscLSpiroFilter() {
		SetMonitorGammaApply( false );

		m_bPreProcessNormalization = false;
		GenNormalizedSignals();
	}
	CVulkanNtscLSpiroFilter::~CVulkanNtscLSpiroFilter() {
		StopThreads();
		DeActivate();
	}

	// == Functions.
	/**
	 * Sets the basic parameters for the filter.
	 *
	 * \param _stBuffers The number of render targets to create.
	 * \param _ui16Width The console screen width.  Typically 256.
	 * \param _ui16Height The console screen height.  Typically 240.
	 * \return Returns the input format requested of the PPU.
	 */
	CDisplayClient::LSN_PPU_OUT_FORMAT CVulkanNtscLSpiroFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		StopThreads();
		m_ui32SrcW = _ui16Width;
		m_ui32SrcH = _ui16Height;

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		AllocYiqBuffers( _ui16Width, _ui16Height, m_ui16WidthScale );

		ReleaseBaseSizeDependents();
		ReleaseSizeDependents();
		
		auto pofOut = CParent::Init( _stBuffers, _ui16Width, _ui16Height );
		m_stStride = size_t( m_ui32OutputWidth * sizeof( uint16_t ) );

		StartThreads();
		return pofOut;
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
	uint8_t * CVulkanNtscLSpiroFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle,
		int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) {
		
		if LSN_UNLIKELY( !m_pvkDevice ) {
			/*if ( !s_vgsState.CreateVulkan() ) { return m_vOutputBuffer.data(); }
			m_pvkDevice = &s_vgsState.vkDevice;*/
			return m_vOutputBuffer.data();
		}
		if ( m_pvkDevice ) {
			if LSN_UNLIKELY( !m_bValidState || _ui32Width != m_ui32SrcW || _ui32Height != m_ui32SrcH ) {
				m_ui32SrcW = _ui32Width;
				m_ui32SrcH = _ui32Height;
				if ( !EnsureSizeAndResources() ) { m_bValidState = false; return m_vOutputBuffer.data(); }
				AllocYiqBuffers( uint16_t( m_ui32SrcW ), uint16_t( m_ui32SrcH ), m_ui16WidthScale );
				m_vOutputBuffer.resize( _ui32Width * _ui32Height * sizeof( uint32_t ) );
			}
			
			lsw::LSW_RECT rRect;
			rRect.left = LONG( _i32DispLeft );
			rRect.top = LONG( _i32DispTop );
			rRect.right = rRect.left + LONG( _ui32DispWidth );
			rRect.bottom = rRect.top + LONG( _ui32DispHeight );

			m_vRgbBuffer.resize( m_ui16ScaledWidth * m_ui32SrcH * 4 * sizeof( float ) );

			FilterFrame( _pui8Input, _ui64RenderStartCycle + 2 );

			if ( m_bValidState && UploadTexture() ) {
				VkDevice dDevice = m_pvkDevice->GetDevice();
				VkQueue qQueue = m_pvkDevice->GetCommandQueue();

				m_fRenderFence.Wait( UINT64_MAX );
				m_bCanPresent = false;

				VkResult rRes = CVulkan::m_pfAcquireNextImageKHR( dDevice, m_pvkDevice->GetSwapChain(), UINT64_MAX, m_sImageAvailable.Get(), VK_NULL_HANDLE, &m_ui32ImageIndex );
				if ( rRes == VK_ERROR_OUT_OF_DATE_KHR || rRes == VK_SUBOPTIMAL_KHR ) {
					m_pvkDevice->ResizeSwapChain();
				}
				else if ( rRes == VK_SUCCESS ) {
					m_fRenderFence.ResetFence();

					VkCommandBuffer cbCmd = m_cbCommandBuffer.Get();

					VkCommandBufferBeginInfo cbbiInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
					cbbiInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
					CVulkan::m_pfBeginCommandBuffer( cbCmd, &cbbiInfo );

					VkImageMemoryBarrier imbUpload = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
					imbUpload.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					imbUpload.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					imbUpload.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imbUpload.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imbUpload.image = m_piTexture->Get();
					imbUpload.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
					imbUpload.srcAccessMask = 0;
					imbUpload.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

					CVulkan::m_pfCmdPipelineBarrier( cbCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imbUpload );

					VkBufferImageCopy bicCopy = {};
					bicCopy.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
					bicCopy.imageExtent = { m_ui16ScaledWidth, m_ui32SrcH, 1 };
					CVulkan::m_pfCmdCopyBufferToImage( cbCmd, m_pbTextureUpload->Get(), m_piTexture->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bicCopy );

					imbUpload.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					imbUpload.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imbUpload.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					imbUpload.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
					CVulkan::m_pfCmdPipelineBarrier( cbCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imbUpload );

					RenderBase( m_pvkDevice, &m_cbCommandBuffer, m_ivTextureView.ivImageView, m_sPointSampler.sSampler, m_ui16ScaledWidth, m_ui32SrcH, rRect, m_ui32ImageIndex, false );

					CVulkan::m_pfEndCommandBuffer( cbCmd );

					VkPipelineStageFlags psfWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
					VkSemaphore sWaits[] = { m_sImageAvailable.Get() };
					VkSemaphore sSignals[] = { m_sRenderFinished.Get() };

					VkSubmitInfo siSubmit = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
					siSubmit.waitSemaphoreCount = 1;
					siSubmit.pWaitSemaphores = sWaits;
					siSubmit.pWaitDstStageMask = &psfWaitStage;
					siSubmit.commandBufferCount = 1;
					siSubmit.pCommandBuffers = &cbCmd;
					siSubmit.signalSemaphoreCount = 1;
					siSubmit.pSignalSemaphores = sSignals;

					CVulkan::m_pfQueueSubmit( qQueue, 1, &siSubmit, m_fRenderFence.Get() );
					m_bCanPresent = true;
				}
			}
		}

		_ui32Width = uint32_t( s_vgsState.rScreenRect.Width() );
		_ui32Height = uint32_t( s_vgsState.rScreenRect.Height() );
		_ui32Stride = _ui32Width * sizeof( uint32_t );
		return m_vOutputBuffer.data();
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CVulkanNtscLSpiroFilter::Activate() {
		CParent::Activate();

		EnsureSizeAndResources();
		AllocYiqBuffers( uint16_t( m_ui32SrcW ), uint16_t( m_ui32SrcH ), m_ui16WidthScale );
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CVulkanNtscLSpiroFilter::DeActivate() {
		if ( m_pvkDevice && m_pvkDevice->GetDevice() ) {
			CVulkan::m_pfDeviceWaitIdle( m_pvkDevice->GetDevice() );
		}

		ReleaseSizeDependents();

		CParent::DeActivate();
	}

	/**
	 * Informs the filter of a window resize.
	 **/
	void CVulkanNtscLSpiroFilter::FrameResize() {
		if ( s_vgsState.vkDevice.GetDevice() ) {
			CVulkan::m_pfDeviceWaitIdle( s_vgsState.vkDevice.GetDevice() );
		}

		ReleaseBaseSizeDependents();
		ReleaseSwapchainResources();
		OnSizeVulkan();
		EnsureSizeAndResources();
	}

	/**
	 * Sets the number of worker threads used by the filter.
	 *
	 * \param _stThreads Number of worker threads to use.  0 disables worker threads.
	 */
	void CVulkanNtscLSpiroFilter::SetWorkerThreadCount( size_t _stThreads ) {
		if ( _stThreads == m_stWorkerThreadCount ) { return; }

		const bool bRestart = m_bThreadsStarted;
		if ( bRestart ) { StopThreads(); }
		m_stWorkerThreadCount = _stThreads;
		if ( bRestart ) { StartThreads(); }
	}

	/**
	 * Renders a full frame of PPU 9-bit (stored in uint16_t's) palette indices to a given 32-bit RGBX buffer.
	 * 
	 * \param _pui8Pixels The input array of 9-bit PPU outputs.
	 * \param _ui64RenderStartCycle The PPU cycle at the start of the block being rendered.
	 **/
	void CVulkanNtscLSpiroFilter::FilterFrame( const uint8_t * _pui8Pixels, uint64_t _ui64RenderStartCycle ) {
		const uint32_t ui32Pitch = m_ui16ScaledWidth * 4 * sizeof( float );
		if LSN_UNLIKELY( !m_vThreads.size() ) {
			RenderScanlineRange<false, false>( _pui8Pixels, 0, m_ui16Height, _ui64RenderStartCycle, m_vRgbBuffer.data(), ui32Pitch );
			return;
		}

		const size_t stThreads = m_vThreads.size() + 1;
		{
			std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
			m_jJob.pui8Pixels = _pui8Pixels;
			m_jJob.ui64RenderStartCycle = _ui64RenderStartCycle;
			m_jJob.stThreads = stThreads;
			++m_ui64JobId;
			m_ui32WorkersRemaining.store( uint32_t( m_vThreads.size() ) );
		}
		m_cvGo.notify_all();

		const uint16_t ui16Lines = m_ui16Height;
		const uint16_t ui16Start = 0;
		const uint16_t ui16End = uint16_t( (uint32_t( ui16Lines ) * 1U) / uint32_t( stThreads ) );
		RenderScanlineRange<false, false>( _pui8Pixels, ui16Start, ui16End, _ui64RenderStartCycle, m_vRgbBuffer.data(), ui32Pitch );

		std::unique_lock<std::mutex> ulLock( m_mThreadMutex );
		m_cvDone.wait( ulLock, [&]() { return m_ui32WorkersRemaining.load() == 0; } );
	}

	/**
	 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
	 * 
	 * \return Returns true on success.
	 */
	bool CVulkanNtscLSpiroFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		
		if LSN_UNLIKELY( !m_ui32SrcW || !m_ui32SrcH ) { return false; }

		if ( !m_pvkDevice ) {
			if ( !s_vgsState.CreateVulkan() ) { return false; }
			m_pvkDevice = &s_vgsState.vkDevice;
		}

		VkDevice dDevice = m_pvkDevice->GetDevice();
		if ( !dDevice ) { return false; }

		if ( !CParent::EnsureBaseSizeAndResources( m_pvkDevice, m_ui16ScaledWidth, m_ui32SrcH ) ) { return false; }

		if ( m_piTexture.get() && m_ui32OutputWidth == m_ui32SrcW && m_ui32OutputHeight == m_ui32SrcH ) { m_bValidState = true; return true; }

		ReleaseSizeDependents();

		m_piTexture = std::make_unique<CVulkanImage>();
		VkImageCreateInfo iciInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		iciInfo.imageType = VK_IMAGE_TYPE_2D;
		iciInfo.extent = { m_ui16ScaledWidth, m_ui32SrcH, 1 };
		iciInfo.mipLevels = 1;
		iciInfo.arrayLayers = 1;
		iciInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		iciInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		iciInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		iciInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		iciInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		m_piTexture->CreateImage( dDevice, &iciInfo );

		VkMemoryRequirements mrReq;
		CVulkan::m_pfGetImageMemoryRequirements( dDevice, m_piTexture->Get(), &mrReq );

		VkMemoryAllocateInfo maiAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		maiAlloc.allocationSize = mrReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( m_pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		m_pdmTextureMemory = std::make_unique<CVulkanDeviceMemory>();
		m_pdmTextureMemory->AllocateMemory( dDevice, &maiAlloc );
		CVulkan::m_pfBindImageMemory( dDevice, m_piTexture->Get(), m_pdmTextureMemory->Get(), 0 );

		VkImageViewCreateInfo ivciView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		ivciView.image = m_piTexture->Get();
		ivciView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivciView.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		ivciView.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		m_ivTextureView.Create( dDevice, &ivciView );

		m_pbTextureUpload = std::make_unique<CVulkanBuffer>();
		VkBufferCreateInfo bciBuf = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bciBuf.size = static_cast<VkDeviceSize>(m_ui16ScaledWidth) * m_ui32SrcH * 4 * sizeof( float );
		bciBuf.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		m_pbTextureUpload->CreateBuffer( dDevice, &bciBuf );

		CVulkan::m_pfGetBufferMemoryRequirements( dDevice, m_pbTextureUpload->Get(), &mrReq );
		maiAlloc.allocationSize = mrReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( m_pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

		m_pdmTextureUploadMemory = std::make_unique<CVulkanDeviceMemory>();
		m_pdmTextureUploadMemory->AllocateMemory( dDevice, &maiAlloc );
		CVulkan::m_pfBindBufferMemory( dDevice, m_pbTextureUpload->Get(), m_pdmTextureUploadMemory->Get(), 0 );

		if ( !CParent::CreateSamplers( dDevice ) ) { return false; }

		m_bValidState = true;

		return true;
	}

	/**
	 * \brief Releases size-dependent resources.
	 */
	void CVulkanNtscLSpiroFilter::ReleaseSizeDependents() {
		m_ivTextureView.Reset();
		if ( m_pdmTextureMemory.get() ) { m_pdmTextureMemory->Reset(); }
		if ( m_piTexture.get() ) { m_piTexture->Reset(); }
		if ( m_pdmTextureUploadMemory.get() ) { m_pdmTextureUploadMemory->Reset(); }
		if ( m_pbTextureUpload.get() ) { m_pbTextureUpload->Reset(); }

		m_piTexture.reset();
		m_pdmTextureMemory.reset();
		m_pbTextureUpload.reset();
		m_pdmTextureUploadMemory.reset();
	}

	/**
	 * \brief Uploads the CPU-side NTSC image to the staging upload buffer.
	 * \return Returns true on success.
	 */
	bool CVulkanNtscLSpiroFilter::UploadTexture() {
		if LSN_UNLIKELY( !m_ui16ScaledWidth || !m_ui32SrcH || !m_pvkDevice ||
			!m_piTexture.get() || !m_piTexture->Get() ||
			!m_pbTextureUpload.get() || !m_pbTextureUpload->Get() ||
			!m_pdmTextureUploadMemory.get() || !m_pdmTextureUploadMemory->Get() ) { return false; }

		void* pvData = nullptr;
		VkDeviceSize stBytes = static_cast<VkDeviceSize>(m_ui16ScaledWidth) * m_ui32SrcH * 4 * sizeof( float );

		if ( CVulkan::m_pfMapMemory( m_pvkDevice->GetDevice(), m_pdmTextureUploadMemory->Get(), 0, stBytes, 0, &pvData ) == VK_SUCCESS ) {
			std::memcpy( pvData, m_vRgbBuffer.data(), stBytes );
			CVulkan::m_pfUnmapMemory( m_pvkDevice->GetDevice(), m_pdmTextureUploadMemory->Get() );
			return true;
		}
		return false;
	}

	/**
	 * \brief Starts the worker threads.
	 *
	 * Creates m_vThreads based on m_stWorkerThreadCount and resets the thread-control state.
	 * Safe to call multiple times; if threads are already started, this function does nothing.
	 */
	void CVulkanNtscLSpiroFilter::StartThreads() {
		if ( m_bThreadsStarted ) { return; }

		const size_t stWorkers = m_stWorkerThreadCount;
		m_bStopThreads = false;
		m_ui64JobId = 0;
		m_ui32WorkersRemaining.store( 0 );
		m_vThreads.clear();

		if ( stWorkers ) {
			m_vThreads.reserve( stWorkers );
			for ( size_t I = 0; I < stWorkers; ++I ) {
				m_vThreads.emplace_back( &CVulkanNtscLSpiroFilter::WorkerThread, this, I + 1 );
			}
		}

		m_bThreadsStarted = true;
	}

	/**
	 * \brief Stops the worker threads.
	 *
	 * Signals all worker threads to exit, wakes them, joins them, clears m_vThreads, and
	 * resets thread-control state.  Safe to call multiple times; if threads are not started,
	 * this function does nothing.
	 */
	void CVulkanNtscLSpiroFilter::StopThreads() {
		if ( !m_bThreadsStarted ) { return; }

		{
			std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
			m_bStopThreads = true;
		}
		m_cvGo.notify_all();

		for ( auto & T : m_vThreads ) {
			if ( T.joinable() ) {
				T.join();
			}
		}
		m_vThreads.clear();

		{
			std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
			m_bStopThreads = false;
		}
		m_ui32WorkersRemaining.store( 0 );
		m_bThreadsStarted = false;
	}

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
	void CVulkanNtscLSpiroFilter::WorkerThread( size_t _stThreadIdx ) {
		::SetThreadHighPriority();
		lsn::CScopedNoSubnormals snsNoSubnormals;

		uint64_t ui64LastJobId = 0;

		for ( ;; ) {
			LSN_JOB jJob;
			{
				std::unique_lock<std::mutex> ulLock( m_mThreadMutex );
				m_cvGo.wait( ulLock, [&]() { return m_bStopThreads || m_ui64JobId != ui64LastJobId; } );
				if ( m_bStopThreads ) { break; }

				ui64LastJobId = m_ui64JobId;
				jJob = m_jJob;
			}

			const uint16_t ui16Lines = m_ui16Height;
			const uint16_t ui16Start = uint16_t( (uint32_t( ui16Lines ) * uint32_t( _stThreadIdx )) / uint32_t( jJob.stThreads ) );
			const uint16_t ui16End = uint16_t( (uint32_t( ui16Lines ) * uint32_t( _stThreadIdx + 1 )) / uint32_t( jJob.stThreads ) );
			
			if ( ui16End > ui16Start ) {
				const uint32_t ui32Pitch = m_ui16ScaledWidth * 4 * sizeof( float );
				RenderScanlineRange<false, false>( jJob.pui8Pixels, ui16Start, ui16End, jJob.ui64RenderStartCycle, m_vRgbBuffer.data(), ui32Pitch );
			}

			if ( m_ui32WorkersRemaining.fetch_sub( 1 ) == 1 ) {
				std::lock_guard<std::mutex> lgLock( m_mThreadMutex );
				m_cvDone.notify_one();
			}
		}
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
