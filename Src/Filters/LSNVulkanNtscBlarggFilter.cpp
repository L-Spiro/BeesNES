#ifdef LSN_VULKAN1

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Blargg’s implementation of an NTSC filter for Vulkan.
 */

#include "LSNVulkanNtscBlarggFilter.h"
#include "../Utilities/LSNScopedNoSubnormals.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#include <numbers>

namespace lsn {

	// == Members.
	CVulkanNtscBlarggFilter::CVulkanNtscBlarggFilter() :
		m_ui32FinalStride( 0 ) {
		nes_ntsc_setup_t nsTmp = nes_ntsc_composite;
		nsTmp.artifacts = 0.62;
		nsTmp.bleed = 0.0;
		nsTmp.fringing = 0.05;
		nsTmp.sharpness = 0.78;
		nsTmp.merge_fields = 0;
		nsTmp.saturation = -0.250;
		nsTmp.brightness = -0.084;
		nsTmp.gamma = 0.0;
		nsTmp.hue = 7.89 / 180.0;
		
		::nes_ntsc_init( &m_nnBlarggNtsc, &nsTmp );

		m_rsResampler.SetFilter( CResamplerBase::LSN_FF_ROBIDOUX );
		SetPhosphorDecayLevel( 0.15f );
		SetPhosphorDecayPeriod( 1.79113161563873291015625f / 7.0f );
	}

	CVulkanNtscBlarggFilter::~CVulkanNtscBlarggFilter() {
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CVulkanNtscBlarggFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_ui32SrcW = _ui16Width;
		m_ui32SrcH = _ui16Height;

		ReleaseBaseSizeDependents();
		ReleaseSizeDependents();
		
		auto pofOut = CParent::Init( _stBuffers, _ui16Width, _ui16Height );

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( _ui16Width * sizeof( uint16_t ) );

		m_ui32FinalStride = RowStride( NES_NTSC_OUT_WIDTH( _ui16Width ), OutputBits() );
		m_vRgbBuffer.resize( m_ui32FinalStride * _ui16Height );

		return pofOut;
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
	uint8_t * CVulkanNtscBlarggFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t _ui64RenderStartCycle,
		int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) {
		
		if LSN_UNLIKELY( !m_pvkDevice ) {
			if ( !s_vgsState.CreateVulkan() ) { return m_vBasicRenderTarget[0].data(); }
			m_pvkDevice = &s_vgsState.vkDevice;
		}
		if ( m_pvkDevice ) {
			if LSN_UNLIKELY( _ui32Width != m_ui32SrcW || _ui32Height != m_ui32SrcH ) {
				m_ui32SrcW = _ui32Width;
				m_ui32SrcH = _ui32Height;
				if ( !EnsureSizeAndResources() ) { m_bValidState = false; return m_vOutputBuffer.data(); }
				m_vOutputBuffer.resize( _ui32Width * _ui32Height * sizeof( uint32_t ) );
			}

			lsw::LSW_RECT rRect;
			rRect.left = LONG( _i32DispLeft );
			rRect.top = LONG( _i32DispTop );
			rRect.right = rRect.left + LONG( _ui32DispWidth );
			rRect.bottom = rRect.top + LONG( _ui32DispHeight );

			const uint32_t ui32Pitch = m_ui32FinalStride;
			m_vRgbBuffer.resize( ui32Pitch * m_ui32SrcH );

			{
				CScopedNoSubnormals snsScope;
				::nes_ntsc_blit( &m_nnBlarggNtsc,
					reinterpret_cast<NES_NTSC_IN_T *>(_pui8Input), _ui32Width, _ui64RenderStartCycle % 3, 3,
					_ui32Width, _ui32Height,
					m_vRgbBuffer.data(), m_ui32FinalStride );
			}
			
			_ui32Width = NES_NTSC_OUT_WIDTH( _ui32Width );
			_ui32Stride = m_ui32FinalStride;

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
					bicCopy.imageExtent = { _ui32Width, m_ui32SrcH, 1 };
					CVulkan::m_pfCmdCopyBufferToImage( cbCmd, m_pbTextureUpload->Get(), m_piTexture->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bicCopy );

					imbUpload.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					imbUpload.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imbUpload.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					imbUpload.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
					CVulkan::m_pfCmdPipelineBarrier( cbCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imbUpload );

					RenderBase( m_pvkDevice, &m_cbCommandBuffer, m_ivTextureView.ivImageView, m_sPointSampler.sSampler, _ui32Width, m_ui32SrcH, rRect, m_ui32ImageIndex, false );

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
	void CVulkanNtscBlarggFilter::Activate() {
		CParent::Activate();
		EnsureSizeAndResources();
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CVulkanNtscBlarggFilter::DeActivate() {
		if ( m_pvkDevice && m_pvkDevice->GetDevice() ) {
			CVulkan::m_pfDeviceWaitIdle( m_pvkDevice->GetDevice() );
		}

		ReleaseSizeDependents();
		CParent::DeActivate();
	}

	/**
	 * Informs the filter of a window resize.
	 **/
	void CVulkanNtscBlarggFilter::FrameResize() {
		if ( s_vgsState.vkDevice.GetDevice() ) {
			CVulkan::m_pfDeviceWaitIdle( s_vgsState.vkDevice.GetDevice() );
		}

		ReleaseBaseSizeDependents();
		ReleaseSwapchainResources();
		OnSizeVulkan();
		EnsureSizeAndResources();
	}

	/**
	 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
	 * 
	 * \return Returns true on success.
	 */
	bool CVulkanNtscBlarggFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		if LSN_UNLIKELY( !m_ui32SrcW || !m_ui32SrcH ) { return false; }

		if ( !m_pvkDevice ) {
			if ( !s_vgsState.CreateVulkan() ) { return false; }
			m_pvkDevice = &s_vgsState.vkDevice;
		}

		VkDevice dDevice = m_pvkDevice->GetDevice();
		if ( !dDevice ) { return false; }

		uint32_t ui32NativeW = NES_NTSC_OUT_WIDTH( m_ui32SrcW );
		if ( !CParent::EnsureBaseSizeAndResources( m_pvkDevice, ui32NativeW, m_ui32SrcH ) ) { return false; }

		if ( m_piTexture.get() && m_ui32OutputWidth == m_ui32SrcW && m_ui32OutputHeight == m_ui32SrcH ) { m_bValidState = true; return true; }

		ReleaseSizeDependents();

		m_piTexture = std::make_unique<CVulkanImage>();
		VkImageCreateInfo iciInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		iciInfo.imageType = VK_IMAGE_TYPE_2D;
		iciInfo.extent = { ui32NativeW, m_ui32SrcH, 1 };
		iciInfo.mipLevels = 1;
		iciInfo.arrayLayers = 1;
		iciInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
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
		ivciView.format = VK_FORMAT_B8G8R8A8_UNORM;
		ivciView.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		m_ivTextureView.Create( dDevice, &ivciView );

		m_pbTextureUpload = std::make_unique<CVulkanBuffer>();
		VkBufferCreateInfo bciBuf = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bciBuf.size = static_cast<VkDeviceSize>(ui32NativeW) * m_ui32SrcH * sizeof( uint32_t );
		bciBuf.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		m_pbTextureUpload->CreateBuffer( dDevice, &bciBuf );

		CVulkan::m_pfGetBufferMemoryRequirements( dDevice, m_pbTextureUpload->Get(), &mrReq );
		maiAlloc.allocationSize = mrReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( m_pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

		m_pdmTextureUploadMemory = std::make_unique<CVulkanDeviceMemory>();
		m_pdmTextureUploadMemory->AllocateMemory( dDevice, &maiAlloc );
		CVulkan::m_pfBindBufferMemory( dDevice, m_pbTextureUpload->Get(), m_pdmTextureUploadMemory->Get(), 0 );

		if ( !CParent::CreateSamplers( dDevice ) ) { return false; }

		/*m_ui32OutputWidth = m_ui32SrcW;
		m_ui32OutputHeight = m_ui32SrcH;*/
		m_bValidState = true;

		return true;
	}

	/**
	 * \brief Releases size-dependent resources.
	 */
	void CVulkanNtscBlarggFilter::ReleaseSizeDependents() {
		m_ivTextureView.Reset();
		if ( m_pdmTextureMemory.get() ) { m_pdmTextureMemory->Reset(); }
		if ( m_piTexture.get() ) { m_piTexture->Reset(); }
		if ( m_pdmTextureUploadMemory.get() ) { m_pdmTextureUploadMemory->Reset(); }
		if ( m_pbTextureUpload.get() ) { m_pbTextureUpload->Reset(); }

		m_piTexture.reset();
		m_pdmTextureMemory.reset();
		m_pbTextureUpload.reset();
		m_pdmTextureUploadMemory.reset();

		//m_ui32OutputWidth = m_ui32OutputHeight = 0;
	}

	/**
	 * \brief Uploads the CPU-side NTSC image to the staging upload buffer.
	 * 
	 * \return Returns true on success.
	 */
	bool CVulkanNtscBlarggFilter::UploadTexture() {
		uint32_t ui32NativeW = NES_NTSC_OUT_WIDTH( m_ui32SrcW );
		if LSN_UNLIKELY( !ui32NativeW || !m_ui32SrcH || !m_pvkDevice ||
			!m_piTexture.get() || !m_piTexture->Get() ||
			!m_pbTextureUpload.get() || !m_pbTextureUpload->Get() ||
			!m_pdmTextureUploadMemory.get() || !m_pdmTextureUploadMemory->Get() ) { return false; }

		void * pvData = nullptr;
		VkDeviceSize stBytes = static_cast<VkDeviceSize>(ui32NativeW) * m_ui32SrcH * sizeof( uint32_t );

		if ( CVulkan::m_pfMapMemory( m_pvkDevice->GetDevice(), m_pdmTextureUploadMemory->Get(), 0, stBytes, 0, &pvData ) == VK_SUCCESS ) {
			uint32_t ui32Pitch = ui32NativeW * sizeof( uint32_t );
			if ( ui32Pitch == m_ui32FinalStride ) {
				std::memcpy( pvData, m_vRgbBuffer.data(), stBytes );
			}
			else {
				for ( uint32_t Y = 0; Y < m_ui32SrcH; ++Y ) {
					std::memcpy( reinterpret_cast<uint8_t *>(pvData) + Y * ui32Pitch, m_vRgbBuffer.data() + Y * m_ui32FinalStride, ui32Pitch );
				}
			}
			CVulkan::m_pfUnmapMemory( m_pvkDevice->GetDevice(), m_pdmTextureUploadMemory->Get() );
			return true;
		}
		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
