#ifdef LSN_VULKAN1

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: LMP88959’s implementation of a PAL filter for Vulkan 1.0.
 */

#include "LSNVulkanPalCrtFullFilter.h"
#include "../Utilities/LSNScopedNoSubnormals.h"
#include "PAL-CRT-Full/pal_core.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>

#define m_nsSettings				(*reinterpret_cast<PAL_SETTINGS *>(m_vSettings.data()))
#define m_nnCrtPal					(*reinterpret_cast<PAL_CRT *>(m_vCrtPal.data()))

namespace lsn {

	// == Members.
	CVulkanPalCrtFullFilter::CVulkanPalCrtFullFilter() :
		m_ui32FinalStride( 0 ) {
		int iPhases[4] = { 0, 16, 0, -16 };
		std::memcpy( m_iPhaseRef, iPhases, sizeof( iPhases ) );

		m_vSettings.resize( sizeof( PAL_SETTINGS ) );
		m_vCrtPal.resize( sizeof( PAL_CRT ) );

		m_rsResampler.SetFilter( CResamplerBase::LSN_FF_LINEAR );
		m_gGamma = CNesPalette::LSN_G_CRT1;
	}
	CVulkanPalCrtFullFilter::~CVulkanPalCrtFullFilter() {
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CVulkanPalCrtFullFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_ui32SrcW = _ui16Width;
		m_ui32SrcH = _ui16Height;

		ReleaseBaseSizeDependents();
		ReleaseSizeDependents();
		
		auto pofOut = CParent::Init( _stBuffers, _ui16Width, _ui16Height );

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( _ui16Width * sizeof( uint16_t ) );

		constexpr uint32_t ui32Scale = 1;
		m_ui32FinalWidth = PAL_HRES * ui32Scale;
		m_ui32FinalHeight = _ui16Height * ui32Scale;
		m_ui32FinalStride = RowStride( m_ui32FinalWidth, OutputBits() );
		m_vRgbBuffer.resize( m_ui32FinalStride * m_ui32FinalHeight );

		::pal_init( &m_nnCrtPal, m_ui32FinalWidth, m_ui32FinalHeight, PAL_PIX_FORMAT_BGRA, m_vRgbBuffer.data() );
		m_nnCrtPal.brightness = 4;
		m_nnCrtPal.contrast = 180;
		m_nnCrtPal.saturation = 17;
		m_nnCrtPal.black_point = 4;
		m_nnCrtPal.white_point = 75;
		m_nnCrtPal.blend = 0;

		m_nnCrtPal.chroma_correction = 1;
		m_nsSettings.yoffset = 7;

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
	uint8_t * CVulkanPalCrtFullFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/,
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
				m_vOutputBuffer.resize( _ui32Width * _ui32Height * sizeof( uint32_t ) );
			}

			lsw::LSW_RECT rRect;
			rRect.left = LONG( _i32DispLeft );
			rRect.top = LONG( _i32DispTop );
			rRect.right = rRect.left + LONG( _ui32DispWidth );
			rRect.bottom = rRect.top + LONG( _ui32DispHeight );

			const uint32_t ui32Pitch = m_ui32FinalStride;
			m_vRgbBuffer.resize( ui32Pitch * m_ui32SrcH );

			m_nsSettings.data = reinterpret_cast<unsigned short *>(_pui8Input);
			m_nsSettings.w = int( m_ui32OutputWidth );
			m_nsSettings.h = int( m_ui32OutputHeight );

			{
				CScopedNoSubnormals snsScope;
				::pal_modulate( &m_nnCrtPal, &m_nsSettings );
				::pal_demodulate( &m_nnCrtPal, 3 );
			}

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
					bicCopy.imageExtent = { m_ui32FinalWidth, m_ui32FinalHeight, 1 };
					CVulkan::m_pfCmdCopyBufferToImage( cbCmd, m_pbTextureUpload->Get(), m_piTexture->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bicCopy );

					imbUpload.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					imbUpload.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imbUpload.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					imbUpload.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
					CVulkan::m_pfCmdPipelineBarrier( cbCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imbUpload );

					RenderBase( m_pvkDevice, &m_cbCommandBuffer, m_ivTextureView.ivImageView, m_sPointSampler.sSampler, m_ui32FinalWidth, m_ui32FinalHeight, rRect, m_ui32ImageIndex, false );

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
	void CVulkanPalCrtFullFilter::Activate() {
		CParent::Activate();
		EnsureSizeAndResources();
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CVulkanPalCrtFullFilter::DeActivate() {
		if ( m_pvkDevice && m_pvkDevice->GetDevice() ) {
			CVulkan::m_pfDeviceWaitIdle( m_pvkDevice->GetDevice() );
		}

		ReleaseSizeDependents();
		
		CParent::DeActivate();
	}

	/**
	 * Informs the filter of a window resize.
	 **/
	void CVulkanPalCrtFullFilter::FrameResize() {
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
	bool CVulkanPalCrtFullFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		
		if LSN_UNLIKELY( !m_ui32SrcW || !m_ui32SrcH ) { return false; }

		if ( !m_pvkDevice ) {
			if ( !s_vgsState.CreateVulkan() ) { return false; }
			m_pvkDevice = &s_vgsState.vkDevice;
		}

		VkDevice dDevice = m_pvkDevice->GetDevice();
		if ( !dDevice ) { return false; }

		if ( !CParent::EnsureBaseSizeAndResources( m_pvkDevice, m_ui32FinalWidth, m_ui32FinalHeight ) ) { return false; }

		if ( m_piTexture.get() && m_ui32OutputWidth == m_ui32SrcW && m_ui32OutputHeight == m_ui32SrcH ) { m_bValidState = true; return true; }

		ReleaseSizeDependents();

		m_piTexture = std::make_unique<CVulkanImage>();
		VkImageCreateInfo iciInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		iciInfo.imageType = VK_IMAGE_TYPE_2D;
		iciInfo.extent = { m_ui32FinalWidth, m_ui32FinalHeight, 1 };
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
		bciBuf.size = static_cast<VkDeviceSize>(m_ui32FinalWidth) * m_ui32FinalHeight * sizeof( uint32_t );
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
	void CVulkanPalCrtFullFilter::ReleaseSizeDependents() {
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
	 * \brief Uploads the CPU-side PAL image to the staging upload buffer.
	 * 
	 * \return Returns true on success.
	 */
	bool CVulkanPalCrtFullFilter::UploadTexture() {
		if LSN_UNLIKELY( !m_ui32FinalWidth || !m_ui32FinalHeight || !m_pvkDevice ||
			!m_piTexture.get() || !m_piTexture->Get() ||
			!m_pbTextureUpload.get() || !m_pbTextureUpload->Get() ||
			!m_pdmTextureUploadMemory.get() || !m_pdmTextureUploadMemory->Get() ) { return false; }

		void * pvData = nullptr;
		VkDeviceSize stBytes = static_cast<VkDeviceSize>(m_ui32FinalWidth) * m_ui32FinalHeight * sizeof( uint32_t );

		if ( CVulkan::m_pfMapMemory( m_pvkDevice->GetDevice(), m_pdmTextureUploadMemory->Get(), 0, stBytes, 0, &pvData ) == VK_SUCCESS ) {
			uint32_t ui32Pitch = m_ui32FinalWidth * sizeof( uint32_t );
			if ( ui32Pitch == m_ui32FinalStride ) {
				std::memcpy( pvData, m_vRgbBuffer.data(), stBytes );
			}
			else {
				for ( uint32_t Y = 0; Y < m_ui32FinalHeight; ++Y ) {
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

#undef m_nsSettings
#undef m_nnCrtPal
