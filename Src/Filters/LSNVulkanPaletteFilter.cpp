#ifdef LSN_VULKAN1

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Vulkan 1.0 palette filter.
 */

#include "LSNVulkanPaletteFilter.h"
#include <cstring>

namespace lsn {

	CVulkanPaletteFilter::CVulkanPaletteFilter() {
		SetPhosphorDecayLevel( 0.15f );
		SetPhosphorDecayPeriod( 1.79113161563873291015625f / 7.0f );
		m_gGamma = CNesPalette::LSN_G_NONE;
	}

	CVulkanPaletteFilter::~CVulkanPaletteFilter() {
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
	CDisplayClient::LSN_PPU_OUT_FORMAT CVulkanPaletteFilter::Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) {
		m_ui32SrcW = _ui16Width;
		m_ui32SrcH = _ui16Height;

		m_ui32OutputWidth = _ui16Width;
		m_ui32OutputHeight = _ui16Height;
		m_stStride = size_t( m_ui32OutputWidth * sizeof( uint32_t ) );

		ReleaseBaseSizeDependents();
		ReleaseSizeDependents();
		
		return CParent::Init( _stBuffers, _ui16Width, _ui16Height );
	}

	/**
	 * Sets the palette.
	 * 
	 * \param _pfRgba512 Pointer to 2048 floats (512 * RGBA).
	 * \return Returns true if the memory for the palette copy was able to be allocated and _pfRgba512 is not nullptr.
	 **/
	bool CVulkanPaletteFilter::SetLut( const float * _pfRgba512 ) {
		if ( !_pfRgba512 ) { return false; }
		try {
			m_vLut.resize( 4 * 512 );
			std::memcpy( m_vLut.data(), _pfRgba512, sizeof( float ) * 4 * 512 );
			m_bUpdatePalette = true;
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Called when the filter is about to become active.
	 */
	void CVulkanPaletteFilter::Activate() {
		CParent::Activate();

		EnsureSizeAndResources();
		EnsureShaders();
	}

	/**
	 * Called when the filter is about to become inactive.
	 */
	void CVulkanPaletteFilter::DeActivate() {
		m_rpInitialPass.Reset();

		ReleaseSizeDependents();

		m_ivPaletteView.Reset();
		if ( m_pdmPaletteMemory.get() ) { m_pdmPaletteMemory->Reset(); }
		if ( m_piPalette.get() ) { m_piPalette->Reset(); }
		if ( m_pdmPaletteUploadMemory.get() ) { m_pdmPaletteUploadMemory->Reset(); }
		if ( m_pbPaletteUpload.get() ) { m_pbPaletteUpload->Reset(); }
		
		if ( m_pdmVbQuadMemory.get() ) { m_pdmVbQuadMemory->Reset(); }
		if ( m_pbVbQuad.get() ) { m_pbVbQuad->Reset(); }

		m_dsTextureSet.Reset();
		if ( m_pdpDescriptorPool.get() ) { m_pdpDescriptorPool->Reset(); }
		if ( m_pdslDescriptorSetLayout.get() ) { m_pdslDescriptorSetLayout->Reset(); }
		if ( m_pplPipelineLayout.get() ) { m_pplPipelineLayout->Reset(); }
		if ( m_ppShader.get() ) { m_ppShader->Reset(); }

		m_piPalette.reset();
		m_pdmPaletteMemory.reset();
		m_pbPaletteUpload.reset();
		m_pdmPaletteUploadMemory.reset();
		m_pbVbQuad.reset();
		m_pdmVbQuadMemory.reset();
		m_pdpDescriptorPool.reset();
		m_pdslDescriptorSetLayout.reset();
		m_pplPipelineLayout.reset();
		m_ppShader.reset();

		m_bUpdatePalette = true;

		CParent::DeActivate();
	}

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
	uint8_t * CVulkanPaletteFilter::ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &/*_ui16BitDepth*/, uint32_t &_ui32Stride, uint64_t /*_ui64PpuFrame*/, uint64_t /*_ui64RenderStartCycle*/,
		int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) {
		
		if LSN_UNLIKELY( !m_pvkDevice ) {
			if ( !s_vgsState.CreateVulkan() ) { return m_vBasicRenderTarget[0].data(); }
			m_pvkDevice = &s_vgsState.vkDevice;
			m_bUpdatePalette = true;
		}
		if ( m_pvkDevice ) {
			if LSN_UNLIKELY( _ui32Width != m_ui32SrcW || _ui32Height != m_ui32SrcH ) {
				m_ui32SrcW = _ui32Width;
				m_ui32SrcH = _ui32Height;
				if ( !EnsureSizeAndResources() || !EnsureShaders() ) { m_bValidState = false; return m_vOutputBuffer.data(); }
				m_vOutputBuffer.resize( _ui32Width * _ui32Height * sizeof( uint32_t ) );
			}
			if LSN_UNLIKELY( m_bUpdatePalette ) {
				if ( !UpdateLut() ) { m_bValidState = false; return m_vOutputBuffer.data(); }
			}
			if LSN_UNLIKELY( !UploadIndices( reinterpret_cast<const uint16_t *>(_pui8Input), _ui32Width, _ui32Height, _ui32Stride ) ) {
				m_bValidState = false;
			}

			lsw::LSW_RECT rRect;
			rRect.left = LONG( _i32DispLeft );
			rRect.top = LONG( _i32DispTop );
			rRect.right = rRect.left + LONG( _ui32DispWidth );
			rRect.bottom = rRect.top + LONG( _ui32DispHeight );

			if ( m_bValidState ) {
				VkDevice dDevice = m_pvkDevice->GetDevice();
				VkQueue qQueue = m_pvkDevice->GetCommandQueue();


				m_fRenderFence.Wait( UINT64_MAX );

				m_bCanPresent = false;

				VkResult rRes = CVulkan::m_pfAcquireNextImageKHR( dDevice, m_pvkDevice->GetSwapChain(), UINT64_MAX, m_sImageAvailable.Get(), VK_NULL_HANDLE, &m_ui32ImageIndex );
				if ( rRes == VK_ERROR_OUT_OF_DATE_KHR || rRes == VK_SUBOPTIMAL_KHR ) {
					m_pvkDevice->ResizeSwapChain();
				} else if ( rRes == VK_SUCCESS ) {
					m_fRenderFence.ResetFence();
				

					Render( rRect, m_ui32ImageIndex );


					VkPipelineStageFlags psfWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
					VkSemaphore sWaits[] = { m_sImageAvailable.Get() };
					VkSemaphore sSignals[] = { m_sRenderFinished.Get() };

					VkCommandBuffer cbCmd = m_cbCommandBuffer.Get();
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
	 * Informs the filter of a window resize.
	 **/
	void CVulkanPaletteFilter::FrameResize() {
		if ( s_vgsState.vkDevice.GetDevice() ) {
			CVulkan::m_pfDeviceWaitIdle( s_vgsState.vkDevice.GetDevice() );
		}

		ReleaseBaseSizeDependents();
		ReleaseSwapchainResources();		
		OnSizeVulkan();
		EnsureSizeAndResources();
		EnsureShaders();
	}

	/**
	 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
	 * 
	 * \return Returns true on success.
	 */
	bool CVulkanPaletteFilter::EnsureSizeAndResources() {
		m_bValidState = false;
		if ( !m_pvkDevice ) {
			if ( !s_vgsState.CreateVulkan() ) { return false; }
			m_pvkDevice = &s_vgsState.vkDevice;
			m_bUpdatePalette = true;
		}

		VkDevice dDevice = m_pvkDevice->GetDevice();
		if ( !dDevice ) { return false; }

		if ( !CParent::EnsureBaseSizeAndResources( m_pvkDevice, m_ui32SrcW, m_ui32SrcH ) ) { return false; }

		if ( !m_piPalette.get() ) {
			m_piPalette = std::make_unique<CVulkanImage>();
			VkImageCreateInfo iciPalette = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			iciPalette.imageType = VK_IMAGE_TYPE_2D;
			iciPalette.extent = { 512, 1, 1 };
			iciPalette.mipLevels = 1;
			iciPalette.arrayLayers = 1;
			iciPalette.format = VK_FORMAT_R32G32B32A32_SFLOAT;
			iciPalette.tiling = VK_IMAGE_TILING_OPTIMAL;
			iciPalette.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			iciPalette.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			iciPalette.samples = VK_SAMPLE_COUNT_1_BIT;
			m_piPalette->CreateImage( dDevice, &iciPalette );

			VkMemoryRequirements mrReq;
			CVulkan::m_pfGetImageMemoryRequirements( dDevice, m_piPalette->Get(), &mrReq );

			VkMemoryAllocateInfo maiAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			maiAlloc.allocationSize = mrReq.size;
			maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( m_pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

			m_pdmPaletteMemory = std::make_unique<CVulkanDeviceMemory>();
			m_pdmPaletteMemory->AllocateMemory( dDevice, &maiAlloc );
			CVulkan::m_pfBindImageMemory( dDevice, m_piPalette->Get(), m_pdmPaletteMemory->Get(), 0 );

			VkImageViewCreateInfo ivciView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			ivciView.image = m_piPalette->Get();
			ivciView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ivciView.format = VK_FORMAT_R32G32B32A32_SFLOAT;
			ivciView.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			m_ivPaletteView.Create( dDevice, &ivciView );

			m_pbPaletteUpload = std::make_unique<CVulkanBuffer>();
			VkBufferCreateInfo bciBuf = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			bciBuf.size = 512 * 4 * sizeof( float );
			bciBuf.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			m_pbPaletteUpload->CreateBuffer( dDevice, &bciBuf );

			CVulkan::m_pfGetBufferMemoryRequirements( dDevice, m_pbPaletteUpload->Get(), &mrReq );
			maiAlloc.allocationSize = mrReq.size;
			maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( m_pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
			
			m_pdmPaletteUploadMemory = std::make_unique<CVulkanDeviceMemory>();
			m_pdmPaletteUploadMemory->AllocateMemory( dDevice, &maiAlloc );
			CVulkan::m_pfBindBufferMemory( dDevice, m_pbPaletteUpload->Get(), m_pdmPaletteUploadMemory->Get(), 0 );


			LSN_XYZRHWTEX1 Quad[] = {
				{ -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f },
				{  1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f },
				{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
				{  1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f },
			};
			m_pbVbQuad = std::make_unique<CVulkanBuffer>();
			bciBuf.size = sizeof( Quad );
			bciBuf.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			m_pbVbQuad->CreateBuffer( dDevice, &bciBuf );

			CVulkan::m_pfGetBufferMemoryRequirements( dDevice, m_pbVbQuad->Get(), &mrReq );
			maiAlloc.allocationSize = mrReq.size;
			maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( m_pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
			
			m_pdmVbQuadMemory = std::make_unique<CVulkanDeviceMemory>();
			m_pdmVbQuadMemory->AllocateMemory( dDevice, &maiAlloc );
			CVulkan::m_pfBindBufferMemory( dDevice, m_pbVbQuad->Get(), m_pdmVbQuadMemory->Get(), 0 );

			void* pvData = nullptr;
			if ( CVulkan::m_pfMapMemory( dDevice, m_pdmVbQuadMemory->Get(), 0, sizeof( Quad ), 0, &pvData ) == VK_SUCCESS ) {
				std::memcpy( pvData, Quad, sizeof( Quad ) );
				CVulkan::m_pfUnmapMemory( dDevice, m_pdmVbQuadMemory->Get() );
			}

			if ( !CParent::CreateSamplers( dDevice ) ) { return false; }
		}

		if ( m_piIndex.get() && m_ui32OutputWidth == m_ui32SrcW && m_ui32OutputHeight == m_ui32SrcH ) { m_bValidState = true; return true; }


		ReleaseSizeDependents();

		VkFormat fmtRt = m_bUse16BitInitialTarget ? VK_FORMAT_R16G16B16A16_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
		if ( !m_rpInitialPass.Valid() ) { CreateRenderPass( fmtRt, m_rpInitialPass ); }


		m_piInitial = std::make_unique<CVulkanImage>();
		VkImageCreateInfo iciInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		iciInfo.imageType = VK_IMAGE_TYPE_2D;
		iciInfo.extent = { m_ui32SrcW, m_ui32SrcH, 1 };
		iciInfo.mipLevels = 1;
		iciInfo.arrayLayers = 1;
		iciInfo.format = fmtRt;
		iciInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		iciInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		iciInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		iciInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		m_piInitial->CreateImage( dDevice, &iciInfo );

		VkMemoryRequirements mrReq;
		CVulkan::m_pfGetImageMemoryRequirements( dDevice, m_piInitial->Get(), &mrReq );

		VkMemoryAllocateInfo maiAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		maiAlloc.allocationSize = mrReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( m_pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		m_pdmInitialMemory = std::make_unique<CVulkanDeviceMemory>();
		m_pdmInitialMemory->AllocateMemory( dDevice, &maiAlloc );
		CVulkan::m_pfBindImageMemory( dDevice, m_piInitial->Get(), m_pdmInitialMemory->Get(), 0 );

		VkImageViewCreateInfo ivciView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		ivciView.image = m_piInitial->Get();
		ivciView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivciView.format = fmtRt;
		ivciView.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		m_ivInitialView.Create( dDevice, &ivciView );

		VkFramebufferCreateInfo fbciFrame = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		fbciFrame.renderPass = m_rpInitialPass.rpRenderPass;
		fbciFrame.attachmentCount = 1;
		fbciFrame.pAttachments = &m_ivInitialView.ivImageView;
		fbciFrame.width = m_ui32SrcW;
		fbciFrame.height = m_ui32SrcH;
		fbciFrame.layers = 1;
		m_fbInitial.Create( dDevice, &fbciFrame );


		m_piIndex = std::make_unique<CVulkanImage>();
		iciInfo.format = VK_FORMAT_R16_UINT;
		iciInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		m_piIndex->CreateImage( dDevice, &iciInfo );

		CVulkan::m_pfGetImageMemoryRequirements( dDevice, m_piIndex->Get(), &mrReq );
		maiAlloc.allocationSize = mrReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( m_pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		
		m_pdmIndexMemory = std::make_unique<CVulkanDeviceMemory>();
		m_pdmIndexMemory->AllocateMemory( dDevice, &maiAlloc );
		CVulkan::m_pfBindImageMemory( dDevice, m_piIndex->Get(), m_pdmIndexMemory->Get(), 0 );

		ivciView.image = m_piIndex->Get();
		ivciView.format = VK_FORMAT_R16_UINT;
		m_ivIndexView.Create( dDevice, &ivciView );

		m_pbIndexUpload = std::make_unique<CVulkanBuffer>();
		VkBufferCreateInfo bciBuf = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bciBuf.size = static_cast<VkDeviceSize>(m_ui32SrcW) * m_ui32SrcH * sizeof( uint16_t );
		bciBuf.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		m_pbIndexUpload->CreateBuffer( dDevice, &bciBuf );

		CVulkan::m_pfGetBufferMemoryRequirements( dDevice, m_pbIndexUpload->Get(), &mrReq );
		maiAlloc.allocationSize = mrReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( m_pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
		
		m_pdmIndexUploadMemory = std::make_unique<CVulkanDeviceMemory>();
		m_pdmIndexUploadMemory->AllocateMemory( dDevice, &maiAlloc );
		CVulkan::m_pfBindBufferMemory( dDevice, m_pbIndexUpload->Get(), m_pdmIndexUploadMemory->Get(), 0 );

		m_ui32OutputWidth = m_ui32SrcW;
		m_ui32OutputHeight = m_ui32SrcH;
		m_bValidState = true;

		return true;
	}

	/**
	 * \brief Updates the 512-entry float RGBA LUT in the upload buffer.
	 * 
	 * \return Returns true on success.
	 */
	bool CVulkanPaletteFilter::UpdateLut() {
		if LSN_UNLIKELY( !m_pvkDevice ) { return false; }
		if LSN_UNLIKELY( m_vLut.size() && m_bUpdatePalette && m_pbPaletteUpload.get() && m_pbPaletteUpload->Get() ) {
			void* pvData = nullptr;
			if ( CVulkan::m_pfMapMemory( m_pvkDevice->GetDevice(), m_pdmPaletteUploadMemory->Get(), 0, 512 * 4 * sizeof( float ), 0, &pvData ) == VK_SUCCESS ) {
				std::memcpy( pvData, m_vLut.data(), 512 * 4 * sizeof( float ) );
				CVulkan::m_pfUnmapMemory( m_pvkDevice->GetDevice(), m_pdmPaletteUploadMemory->Get() );
			}
		}
		return true;
	}

	/**
	 * \brief Uploads the 16-bit PPU indices to the upload buffer.
	 * 
	 * \param _pui16Idx Source pointer to the index image (row-major).
	 * \param _ui32W Image width in pixels.
	 * \param _ui32H Image height in pixels.
	 * \param _ui32SrcPitch Source pitch in bytes; pass 0 for tightly packed.
	 * \return Returns true on success.
	 */
	bool CVulkanPaletteFilter::UploadIndices( const uint16_t * _pui16Idx, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32SrcPitch ) {
		if LSN_UNLIKELY( !m_pvkDevice || _ui32W != m_ui32SrcW || _ui32H != m_ui32SrcH || !m_piIndex->Get() || !m_pbIndexUpload->Get() ) { return false; }
		if ( !_ui32SrcPitch ) { _ui32SrcPitch = _ui32W * sizeof( uint16_t ); }

		void* pvData = nullptr;
		VkDeviceSize stBytes = static_cast<VkDeviceSize>(_ui32W) * _ui32H * sizeof( uint16_t );

		if ( CVulkan::m_pfMapMemory( m_pvkDevice->GetDevice(), m_pdmIndexUploadMemory->Get(), 0, stBytes, 0, &pvData ) == VK_SUCCESS ) {
			uint32_t ui32Pitch = _ui32W * sizeof( uint16_t );
			if ( ui32Pitch == _ui32SrcPitch ) {
				std::memcpy( pvData, _pui16Idx, stBytes );
			} else {
				for ( uint32_t Y = 0; Y < _ui32H; ++Y ) {
					std::memcpy( reinterpret_cast<uint8_t *>(pvData) + Y * ui32Pitch, reinterpret_cast<const uint8_t *>(_pui16Idx) + Y * _ui32SrcPitch, ui32Pitch );
				}
			}
			CVulkan::m_pfUnmapMemory( m_pvkDevice->GetDevice(), m_pdmIndexUploadMemory->Get() );
			m_bIndicesDirty = true;
			return true;
		}
		return false;
	}

	/**
	 * \brief Ensures pixel shaders, vertex shaders, and PSOs are created.
	 * 
	 * \return Returns true if all shaders are ready.
	 */
	bool CVulkanPaletteFilter::EnsureShaders() {
		if ( !m_pvkDevice ) { return false; }
		VkDevice dDevice = m_pvkDevice->GetDevice();
		if ( m_ppShader.get() && m_ppShader->Get() ) {
			VkDescriptorImageInfo diiIndex = { m_sPointSampler.sSampler, m_ivIndexView.ivImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
			VkDescriptorImageInfo diiPalette = { m_sPointSampler.sSampler, m_ivPaletteView.ivImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

			VkWriteDescriptorSet wdsWrites[2] = {};
			wdsWrites[0] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, m_dsTextureSet.dsDescriptorSet, 0, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &diiIndex, nullptr, nullptr };
			wdsWrites[1] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, m_dsTextureSet.dsDescriptorSet, 1, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &diiPalette, nullptr, nullptr };
			CVulkan::m_pfUpdateDescriptorSets( dDevice, 2, wdsWrites, 0, nullptr );

			return true; 
		}

		// Descriptor Set Layout (Binding 0: Index Image, Binding 1: Palette LUT)
		VkDescriptorSetLayoutBinding dslbBindings[2] = {};
		dslbBindings[0].binding = 0;
		dslbBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dslbBindings[0].descriptorCount = 1;
		dslbBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		dslbBindings[1].binding = 1;
		dslbBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dslbBindings[1].descriptorCount = 1;
		dslbBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo dslciLayoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		dslciLayoutInfo.bindingCount = 2;
		dslciLayoutInfo.pBindings = dslbBindings;

		m_pdslDescriptorSetLayout = std::make_unique<CVulkanDescriptorSetLayout>();
		if ( !m_pdslDescriptorSetLayout->CreateDescriptorSetLayout( dDevice, &dslciLayoutInfo ) ) { return false; }

		// Pool and Sets
		VkDescriptorPoolSize dpsSize = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2 };
		VkDescriptorPoolCreateInfo dpciPoolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		dpciPoolInfo.poolSizeCount = 1;
		dpciPoolInfo.pPoolSizes = &dpsSize;
		dpciPoolInfo.maxSets = 1;

		m_pdpDescriptorPool = std::make_unique<CVulkanDescriptorPool>();
		if ( !m_pdpDescriptorPool->CreateDescriptorPool( dDevice, &dpciPoolInfo ) ) { return false; }

		VkDescriptorSetLayout layouts[] = { m_pdslDescriptorSetLayout->Get() };
		VkDescriptorSetAllocateInfo dsaiAllocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		dsaiAllocInfo.descriptorPool = m_pdpDescriptorPool->Get();
		dsaiAllocInfo.descriptorSetCount = 1;
		dsaiAllocInfo.pSetLayouts = layouts;

		VkDescriptorSet dsSet;
		if ( CVulkan::m_pfAllocateDescriptorSets( dDevice, &dsaiAllocInfo, &dsSet ) != VK_SUCCESS ) { return false; }
		m_dsTextureSet.dsDescriptorSet = dsSet;

		// Bind initial descriptors
		VkDescriptorImageInfo diiIndex = { m_sPointSampler.sSampler, m_ivIndexView.ivImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkDescriptorImageInfo diiPalette = { m_sPointSampler.sSampler, m_ivPaletteView.ivImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		VkWriteDescriptorSet wdsWrites[2] = {};
		wdsWrites[0] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, m_dsTextureSet.dsDescriptorSet, 0, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &diiIndex, nullptr, nullptr };
		wdsWrites[1] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, m_dsTextureSet.dsDescriptorSet, 1, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &diiPalette, nullptr, nullptr };
		CVulkan::m_pfUpdateDescriptorSets( dDevice, 2, wdsWrites, 0, nullptr );

		// Pipeline Layout
		VkPipelineLayoutCreateInfo plciLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		plciLayoutInfo.setLayoutCount = 1;
		plciLayoutInfo.pSetLayouts = layouts;

		m_pplPipelineLayout = std::make_unique<CVulkanPipelineLayout>();
		if ( !m_pplPipelineLayout->CreatePipelineLayout( dDevice, &plciLayoutInfo ) ) { return false; }

		static const char * kVsGlsl =
			"#version 450\n"
			"layout(location = 0) in vec4 inPos;\n"
			"layout(location = 1) in vec2 inTex;\n"
			"layout(location = 0) out vec2 outTex;\n"
			"void main() {\n"
			"    gl_Position = vec4(inPos.x, -inPos.y, inPos.z, inPos.w);\n"	// Vulkan Y-flip.
			"    outTex = inTex;\n"
			"}\n";

		static const char * kPsIdxToColorGlsl =
			"#version 450\n"
			"layout(binding = 0) uniform usampler2D tIdx;\n"
			"layout(binding = 1) uniform sampler2D tLut;\n"
			"layout(location = 0) in vec2 inTex;\n"
			"layout(location = 0) out vec4 outColor;\n"
			"void main() {\n"
			"    uint raw = texture(tIdx, inTex).r;\n"
			"    float idx = clamp(float(raw), 0.0, 511.0);\n"
			"    float u = (idx + 0.5) / 512.0;\n"
			"    outColor = texture(tLut, vec2(u, 0.5));\n"
			"}\n";

		std::vector<uint32_t> vSpirvVert, vSpirvFrag; 
		if ( !CVulkan::CompileGlslToSpirv( kVsGlsl, "vertex", vSpirvVert ) || 
			 !CVulkan::CompileGlslToSpirv( kPsIdxToColorGlsl, "fragment", vSpirvFrag ) ) {
			return false;
		}

		CVulkan::LSN_SHADER_MODULE smVert, smFrag;
		if ( !CVulkan::LoadSpirv( m_pvkDevice, vSpirvVert, smVert ) || !CVulkan::LoadSpirv( m_pvkDevice, vSpirvFrag, smFrag ) ) { return false; }

		VkPipelineShaderStageCreateInfo pssciShaderStages[2] = {};
		pssciShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pssciShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		pssciShaderStages[0].module = smVert.smShaderModule;
		pssciShaderStages[0].pName = "main";

		pssciShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pssciShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		pssciShaderStages[1].module = smFrag.smShaderModule;
		pssciShaderStages[1].pName = "main";

		VkVertexInputBindingDescription vibdBindingDesc = {};
		vibdBindingDesc.binding = 0;
		vibdBindingDesc.stride = sizeof( LSN_XYZRHWTEX1 );
		vibdBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription viadAttributes[2] = {};
		viadAttributes[0].binding = 0;
		viadAttributes[0].location = 0; 
		viadAttributes[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		viadAttributes[0].offset = offsetof( LSN_XYZRHWTEX1, fX );

		viadAttributes[1].binding = 0;
		viadAttributes[1].location = 1; 
		viadAttributes[1].format = VK_FORMAT_R32G32_SFLOAT;
		viadAttributes[1].offset = offsetof( LSN_XYZRHWTEX1, fU );

		VkPipelineVertexInputStateCreateInfo pvisciVertexInput = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		pvisciVertexInput.vertexBindingDescriptionCount = 1;
		pvisciVertexInput.pVertexBindingDescriptions = &vibdBindingDesc;
		pvisciVertexInput.vertexAttributeDescriptionCount = 2;
		pvisciVertexInput.pVertexAttributeDescriptions = viadAttributes;

		VkPipelineInputAssemblyStateCreateInfo piasciInputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		piasciInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

		VkPipelineViewportStateCreateInfo pvsciViewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		pvsciViewportState.viewportCount = 1;
		pvsciViewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo prsciRasterizer = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		prsciRasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		prsciRasterizer.lineWidth = 1.0f;
		prsciRasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

		VkPipelineMultisampleStateCreateInfo pmsciMultisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		pmsciMultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState pcbasColorBlendAttachment = {};
		pcbasColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo pcbsciColorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		pcbsciColorBlending.attachmentCount = 1;
		pcbsciColorBlending.pAttachments = &pcbasColorBlendAttachment;

		VkDynamicState dsDynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo pdsciDynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		pdsciDynamicState.dynamicStateCount = 2;
		pdsciDynamicState.pDynamicStates = dsDynamicStates;

		VkGraphicsPipelineCreateInfo gpciPipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		gpciPipelineInfo.stageCount = 2;
		gpciPipelineInfo.pStages = pssciShaderStages;
		gpciPipelineInfo.pVertexInputState = &pvisciVertexInput;
		gpciPipelineInfo.pInputAssemblyState = &piasciInputAssembly;
		gpciPipelineInfo.pViewportState = &pvsciViewportState;
		gpciPipelineInfo.pRasterizationState = &prsciRasterizer;
		gpciPipelineInfo.pMultisampleState = &pmsciMultisampling;
		gpciPipelineInfo.pColorBlendState = &pcbsciColorBlending;
		gpciPipelineInfo.pDynamicState = &pdsciDynamicState;
		gpciPipelineInfo.layout = m_pplPipelineLayout->Get();
		gpciPipelineInfo.renderPass = m_rpInitialPass.rpRenderPass;

		m_ppShader = std::make_unique<CVulkanPipeline>();
		if ( !m_ppShader->CreateGraphicsPipeline( dDevice, &gpciPipelineInfo ) ) { return false; }

		return true;
	}

	/**
	 * \brief Releases size-dependent resources.
	 */
	void CVulkanPaletteFilter::ReleaseSizeDependents() {
		m_fbInitial.Reset();
		m_ivInitialView.Reset();
		if ( m_pdmInitialMemory.get() ) { m_pdmInitialMemory->Reset(); }
		if ( m_piInitial.get() ) { m_piInitial->Reset(); }

		m_ivIndexView.Reset();
		if ( m_pdmIndexMemory.get() ) { m_pdmIndexMemory->Reset(); }
		if ( m_piIndex.get() ) { m_piIndex->Reset(); }
		if ( m_pdmIndexUploadMemory.get() ) { m_pdmIndexUploadMemory->Reset(); }
		if ( m_pbIndexUpload.get() ) { m_pbIndexUpload->Reset(); }

		m_piInitial.reset();
		m_pdmInitialMemory.reset();
		m_piIndex.reset();
		m_pdmIndexMemory.reset();
		m_pbIndexUpload.reset();
		m_pdmIndexUploadMemory.reset();

		m_ui32OutputWidth = m_ui32OutputHeight = 0;
	}

	/**
	 * \brief Renders the pipeline to the backbuffer.
	 * 
	 * \param _rOutput The destination rectangle in client pixels where the NES image should appear.
	 * \param _ui32ImageIndex The swapchain image index.
	 * \return Returns true if the draw succeeded; false on failure.
	 */
	bool CVulkanPaletteFilter::Render( const lsw::LSW_RECT &_rOutput, uint32_t _ui32ImageIndex ) {
		VkCommandBuffer cbCmd = m_cbCommandBuffer.Get();

		VkCommandBufferBeginInfo cbbiInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		cbbiInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		CVulkan::m_pfBeginCommandBuffer( cbCmd, &cbbiInfo );

		if ( m_bIndicesDirty || m_bUpdatePalette ) {
			VkImageMemoryBarrier imbBarriers[2] = {};
			uint32_t ui32BarrierCount = 0;

			if ( m_bIndicesDirty ) {
				imbBarriers[ui32BarrierCount] = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
				imbBarriers[ui32BarrierCount].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imbBarriers[ui32BarrierCount].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imbBarriers[ui32BarrierCount].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imbBarriers[ui32BarrierCount].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imbBarriers[ui32BarrierCount].image = m_piIndex->Get();
				imbBarriers[ui32BarrierCount].subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
				imbBarriers[ui32BarrierCount].srcAccessMask = 0;
				imbBarriers[ui32BarrierCount].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				ui32BarrierCount++;
			}
			if ( m_bUpdatePalette ) {
				imbBarriers[ui32BarrierCount] = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
				imbBarriers[ui32BarrierCount].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imbBarriers[ui32BarrierCount].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imbBarriers[ui32BarrierCount].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imbBarriers[ui32BarrierCount].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imbBarriers[ui32BarrierCount].image = m_piPalette->Get();
				imbBarriers[ui32BarrierCount].subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
				imbBarriers[ui32BarrierCount].srcAccessMask = 0;
				imbBarriers[ui32BarrierCount].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				ui32BarrierCount++;
			}

			CVulkan::m_pfCmdPipelineBarrier( cbCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, ui32BarrierCount, imbBarriers );

			if ( m_bIndicesDirty ) {
				VkBufferImageCopy bicCopy = {};
				bicCopy.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
				bicCopy.imageExtent = { m_ui32SrcW, m_ui32SrcH, 1 };
				CVulkan::m_pfCmdCopyBufferToImage( cbCmd, m_pbIndexUpload->Get(), m_piIndex->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bicCopy );
			}
			if ( m_bUpdatePalette ) {
				VkBufferImageCopy bicCopy = {};
				bicCopy.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
				bicCopy.imageExtent = { 512, 1, 1 }; 
				CVulkan::m_pfCmdCopyBufferToImage( cbCmd, m_pbPaletteUpload->Get(), m_piPalette->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bicCopy );
			}

			for ( uint32_t I = 0; I < ui32BarrierCount; ++I ) {
				imbBarriers[I].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imbBarriers[I].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imbBarriers[I].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				imbBarriers[I].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			}
			CVulkan::m_pfCmdPipelineBarrier( cbCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, ui32BarrierCount, imbBarriers );

			m_bIndicesDirty = false;
			m_bUpdatePalette = false;
		}

		VkRenderPassBeginInfo rpbiInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		rpbiInfo.renderPass = m_rpInitialPass.rpRenderPass;
		rpbiInfo.framebuffer = m_fbInitial.fbFramebuffer;
		rpbiInfo.renderArea.extent = { m_ui32SrcW, m_ui32SrcH };
		
		CVulkan::m_pfCmdBeginRenderPass( cbCmd, &rpbiInfo, VK_SUBPASS_CONTENTS_INLINE );

		VkViewport vViewport = { 0.0f, 0.0f, static_cast<float>(m_ui32SrcW), static_cast<float>(m_ui32SrcH), 0.0f, 1.0f };
		CVulkan::m_pfCmdSetViewport( cbCmd, 0, 1, &vViewport );
		VkRect2D rScissor = { { 0, 0 }, { m_ui32SrcW, m_ui32SrcH } };
		CVulkan::m_pfCmdSetScissor( cbCmd, 0, 1, &rScissor );

		CVulkan::m_pfCmdBindPipeline( cbCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ppShader->Get() );
		VkBuffer buffers[] = { m_pbVbQuad->Get() };
		VkDeviceSize offsets[] = { 0 };
		CVulkan::m_pfCmdBindVertexBuffers( cbCmd, 0, 1, buffers, offsets );
		CVulkan::m_pfCmdBindDescriptorSets( cbCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pplPipelineLayout->Get(), 0, 1, &m_dsTextureSet.dsDescriptorSet, 0, nullptr );
		
		CVulkan::m_pfCmdDraw( cbCmd, 4, 1, 0, 0 );
		CVulkan::m_pfCmdEndRenderPass( cbCmd );

		// Transition Initial target from color attachment back to shader read-only for the common pipeline
		VkImageMemoryBarrier imbInitial = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		imbInitial.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // Automatically transitioned back by the render pass end
		imbInitial.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imbInitial.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imbInitial.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		imbInitial.image = m_piInitial->Get();
		imbInitial.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		CVulkan::m_pfCmdPipelineBarrier( cbCmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imbInitial );

		RenderBase( m_pvkDevice, &m_cbCommandBuffer, m_ivInitialView.ivImageView, m_sPointSampler.sSampler, m_ui32SrcW, m_ui32SrcH, _rOutput, _ui32ImageIndex, true );

		CVulkan::m_pfEndCommandBuffer( cbCmd );
		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
