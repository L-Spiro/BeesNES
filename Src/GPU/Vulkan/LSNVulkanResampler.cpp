/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 hardware wrapper for 2-pass GPU resampling.
 */

#ifdef LSN_VULKAN1

#include "LSNVulkanResampler.h"
#include <cstring>

namespace lsn {

	CVulkanResampler::CVulkanResampler() {
	}
	CVulkanResampler::~CVulkanResampler() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the resources and internal states.
	 **/
	void CVulkanResampler::Reset() {
		m_dsPass1.Reset();
		m_dsPass2.Reset();

		m_fbIntermediate.Reset();
		m_ivIntermediateView.Reset();
		if LSN_LIKELY( m_pdmIntermediateMemory.get() ) { m_pdmIntermediateMemory->Reset(); }
		if LSN_LIKELY( m_piIntermediate.get() ) { m_piIntermediate->Reset(); }

		m_ivLutXView.Reset();
		if LSN_LIKELY( m_pdmLutXMemory.get() ) { m_pdmLutXMemory->Reset(); }
		if LSN_LIKELY( m_piLutX.get() ) { m_piLutX->Reset(); }
		if LSN_LIKELY( m_pdmLutXUploadMemory.get() ) { m_pdmLutXUploadMemory->Reset(); }
		if LSN_LIKELY( m_pbLutXUpload.get() ) { m_pbLutXUpload->Reset(); }

		m_ivLutYView.Reset();
		if LSN_LIKELY( m_pdmLutYMemory.get() ) { m_pdmLutYMemory->Reset(); }
		if LSN_LIKELY( m_piLutY.get() ) { m_piLutY->Reset(); }
		if LSN_LIKELY( m_pdmLutYUploadMemory.get() ) { m_pdmLutYUploadMemory->Reset(); }
		if LSN_LIKELY( m_pbLutYUpload.get() ) { m_pbLutYUpload->Reset(); }

		if LSN_LIKELY( m_pdmVbQuadMemory.get() ) { m_pdmVbQuadMemory->Reset(); }
		if LSN_LIKELY( m_pbVbQuad.get() ) { m_pbVbQuad->Reset(); }

		if LSN_LIKELY( m_ppPass1.get() ) { m_ppPass1->Reset(); }
		if LSN_LIKELY( m_ppPass2.get() ) { m_ppPass2->Reset(); }
		if LSN_LIKELY( m_pplPipelineLayout.get() ) { m_pplPipelineLayout->Reset(); }
		if LSN_LIKELY( m_pdslDescriptorSetLayout.get() ) { m_pdslDescriptorSetLayout->Reset(); }
		if LSN_LIKELY( m_pdpDescriptorPool.get() ) { m_pdpDescriptorPool->Reset(); }

		m_piIntermediate.reset();
		m_pdmIntermediateMemory.reset();
		m_piLutX.reset();
		m_pdmLutXMemory.reset();
		m_pbLutXUpload.reset();
		m_pdmLutXUploadMemory.reset();
		m_piLutY.reset();
		m_pdmLutYMemory.reset();
		m_pbLutYUpload.reset();
		m_pdmLutYUploadMemory.reset();
		m_pbVbQuad.reset();
		m_pdmVbQuadMemory.reset();
		m_ppPass1.reset();
		m_ppPass2.reset();
		m_pplPipelineLayout.reset();
		m_pdslDescriptorSetLayout.reset();
		m_pdpDescriptorPool.reset();

		m_rpPass1.Reset();
		m_rpPass2.Reset();
		m_sSampler.Reset();

		m_ui32SrcW = m_ui32SrcH = m_ui32DstW = m_ui32DstH = 0;
		m_ui32MaxTapsX = m_ui32MaxTapsY = 0;
		m_fIntermediateFormat = m_fTargetFormat = VK_FORMAT_UNDEFINED;
	}

	/**
	 * Ensures all render targets, intermediate buffers, and LUTs are created.
	 *
	 * \param _pvkDevice The Vulkan device.
	 * \param _pcbCommandList The command list used to record barriers and copy operations.
	 * \param _ui32SrcW The source width.
	 * \param _ui32SrcH The source height.
	 * \param _ui32DstW The destination width.
	 * \param _ui32DstH The destination height.
	 * \param _fIntermediateFormat The format for the intermediate target.
	 * \param _fTargetFormat The format for the final target.
	 * \return Returns true if resources are ready.
	 **/
	bool CVulkanResampler::EnsureResources( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, uint32_t _ui32SrcW, uint32_t _ui32SrcH, uint32_t _ui32DstW, uint32_t _ui32DstH, VkFormat _fIntermediateFormat, VkFormat _fTargetFormat ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pcbCommandList ) { return false; }
		
		if ( m_piIntermediate.get() && m_ui32SrcW == _ui32SrcW && m_ui32SrcH == _ui32SrcH && m_ui32DstW == _ui32DstW && m_ui32DstH == _ui32DstH && m_fIntermediateFormat == _fIntermediateFormat && m_fTargetFormat == _fTargetFormat ) {
			return true;
		}

		Reset();


		VkAttachmentDescription adPass1 = {};
		adPass1.format = _fIntermediateFormat;
		adPass1.samples = VK_SAMPLE_COUNT_1_BIT;
		adPass1.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		adPass1.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		adPass1.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		adPass1.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		adPass1.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		adPass1.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference arRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		VkSubpassDescription sdSubpass = {};
		sdSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		sdSubpass.colorAttachmentCount = 1;
		sdSubpass.pColorAttachments = &arRef;

		VkRenderPassCreateInfo rpciInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		rpciInfo.attachmentCount = 1;
		rpciInfo.pAttachments = &adPass1;
		rpciInfo.subpassCount = 1;
		rpciInfo.pSubpasses = &sdSubpass;
		if ( !m_rpPass1.Create( _pvkDevice->GetDevice(), &rpciInfo ) ) { return false; }

		VkAttachmentDescription adPass2 = adPass1;
		adPass2.format = _fTargetFormat;		// Render pass 2 writes to the target format.
		rpciInfo.pAttachments = &adPass2;
		if ( !m_rpPass2.Create( _pvkDevice->GetDevice(), &rpciInfo ) ) { return false; }


		m_piIntermediate = std::make_unique<CVulkanImage>();
		VkImageCreateInfo iciImageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		iciImageInfo.imageType = VK_IMAGE_TYPE_2D;
		iciImageInfo.extent.width = _ui32DstW;
		iciImageInfo.extent.height = _ui32SrcH;
		iciImageInfo.extent.depth = 1;
		iciImageInfo.mipLevels = 1;
		iciImageInfo.arrayLayers = 1;
		iciImageInfo.format = _fIntermediateFormat;
		iciImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		iciImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		iciImageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		iciImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		iciImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		if ( !m_piIntermediate->CreateImage( _pvkDevice->GetDevice(), &iciImageInfo ) ) { return false; }

		VkMemoryRequirements mrMemReq;
		CVulkan::m_pfGetImageMemoryRequirements( _pvkDevice->GetDevice(), m_piIntermediate->Get(), &mrMemReq );

		VkMemoryAllocateInfo maiAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		maiAlloc.allocationSize = mrMemReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		m_pdmIntermediateMemory = std::make_unique<CVulkanDeviceMemory>();
		if ( !m_pdmIntermediateMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiAlloc ) ) { return false; }
		CVulkan::m_pfBindImageMemory( _pvkDevice->GetDevice(), m_piIntermediate->Get(), m_pdmIntermediateMemory->Get(), 0 );

		VkImageViewCreateInfo ivciView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		ivciView.image = m_piIntermediate->Get();
		ivciView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivciView.format = _fIntermediateFormat;
		ivciView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ivciView.subresourceRange.levelCount = 1;
		ivciView.subresourceRange.layerCount = 1;
		if ( !m_ivIntermediateView.Create( _pvkDevice->GetDevice(), &ivciView ) ) { return false; }

		VkFramebufferCreateInfo fbciInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		fbciInfo.renderPass = m_rpPass1.rpRenderPass;
		fbciInfo.attachmentCount = 1;
		fbciInfo.pAttachments = &m_ivIntermediateView.ivImageView;
		fbciInfo.width = _ui32DstW;
		fbciInfo.height = _ui32SrcH;
		fbciInfo.layers = 1;
		if ( !m_fbIntermediate.Create( _pvkDevice->GetDevice(), &fbciInfo ) ) { return false; }


		if ( !BuildLUT( _pvkDevice, _pcbCommandList, _ui32SrcW, _ui32DstW, m_piLutX, m_pdmLutXMemory, m_ivLutXView, m_pbLutXUpload, m_pdmLutXUploadMemory, m_ui32MaxTapsX ) ) { return false; }
		if ( !BuildLUT( _pvkDevice, _pcbCommandList, _ui32SrcH, _ui32DstH, m_piLutY, m_pdmLutYMemory, m_ivLutYView, m_pbLutYUpload, m_pdmLutYUploadMemory, m_ui32MaxTapsY ) ) { return false; }


		LSN_XYZRHWTEX1 Quad[] = {
			{ -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f },
			{  1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f },
			{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{  1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f },
		};

		m_pbVbQuad = std::make_unique<CVulkanBuffer>();
		VkBufferCreateInfo bciQuad = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bciQuad.size = sizeof( Quad );
		bciQuad.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bciQuad.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if ( !m_pbVbQuad->CreateBuffer( _pvkDevice->GetDevice(), &bciQuad ) ) { return false; }

		CVulkan::m_pfGetBufferMemoryRequirements( _pvkDevice->GetDevice(), m_pbVbQuad->Get(), &mrMemReq );
		maiAlloc.allocationSize = mrMemReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
		
		m_pdmVbQuadMemory = std::make_unique<CVulkanDeviceMemory>();
		if ( !m_pdmVbQuadMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiAlloc ) ) { return false; }
		CVulkan::m_pfBindBufferMemory( _pvkDevice->GetDevice(), m_pbVbQuad->Get(), m_pdmVbQuadMemory->Get(), 0 );

		void* pvData = nullptr;
		if ( CVulkan::m_pfMapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get(), 0, sizeof( Quad ), 0, &pvData ) == VK_SUCCESS ) {
			std::memcpy( pvData, Quad, sizeof( Quad ) );
			CVulkan::m_pfUnmapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get() );
		}


		VkSamplerCreateInfo sciSamplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		sciSamplerInfo.magFilter = VK_FILTER_NEAREST;
		sciSamplerInfo.minFilter = VK_FILTER_NEAREST;
		sciSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		sciSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sciSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sciSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sciSamplerInfo.maxAnisotropy = 1.0f;
		if ( !m_sSampler.Create( _pvkDevice->GetDevice(), &sciSamplerInfo ) ) { return false; }

		m_ui32SrcW = _ui32SrcW; m_ui32SrcH = _ui32SrcH;
		m_ui32DstW = _ui32DstW; m_ui32DstH = _ui32DstH;
		m_fIntermediateFormat = _fIntermediateFormat;
		m_fTargetFormat = _fTargetFormat;

		return true;
	}

	/**
	 * Builds a Look-Up Table (LUT) texture for resampling weights.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _pcbCommandList The command buffer to record upload barriers.
	 * \param _ui32SrcSize The source dimension.
	 * \param _ui32DstSize The destination dimension.
	 * \param _piLut The LUT image to create.
	 * \param _pdmLutMemory The LUT device memory.
	 * \param _ivLutView The LUT image view.
	 * \param _pbUpload The upload buffer.
	 * \param _pdmUploadMemory The upload buffer memory.
	 * \param _ui32OutMaxTaps Receives the maximum kernel taps.
	 * \return Returns true if the LUT was successfully built.
	 **/
	bool CVulkanResampler::BuildLUT( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, uint32_t /*_ui32SrcSize*/, uint32_t _ui32DstSize, std::unique_ptr<CVulkanImage> &_piLut, std::unique_ptr<CVulkanDeviceMemory> &_pdmLutMemory, CVulkan::LSN_IMAGE_VIEW &_ivLutView, std::unique_ptr<CVulkanBuffer> &_pbUpload, std::unique_ptr<CVulkanDeviceMemory> &_pdmUploadMemory, uint32_t &_ui32OutMaxTaps ) {
		// Mock max taps for generic allocation logic. CResamplerBase logic belongs here.
		_ui32OutMaxTaps = 8; // Replace with actual CResamplerBase calculation
		VkDeviceSize stLutBytes = static_cast<VkDeviceSize>(_ui32DstSize) * _ui32OutMaxTaps * 4 * sizeof( float ); // RGBA32_SFLOAT

		_piLut = std::make_unique<CVulkanImage>();
		VkImageCreateInfo iciImageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		iciImageInfo.imageType = VK_IMAGE_TYPE_2D;
		iciImageInfo.extent.width = _ui32OutMaxTaps;
		iciImageInfo.extent.height = _ui32DstSize;
		iciImageInfo.extent.depth = 1;
		iciImageInfo.mipLevels = 1;
		iciImageInfo.arrayLayers = 1;
		iciImageInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		iciImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		iciImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		iciImageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		iciImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		iciImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		if ( !_piLut->CreateImage( _pvkDevice->GetDevice(), &iciImageInfo ) ) { return false; }

		VkMemoryRequirements mrMemReq;
		CVulkan::m_pfGetImageMemoryRequirements( _pvkDevice->GetDevice(), _piLut->Get(), &mrMemReq );

		VkMemoryAllocateInfo maiAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		maiAlloc.allocationSize = mrMemReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		_pdmLutMemory = std::make_unique<CVulkanDeviceMemory>();
		if ( !_pdmLutMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiAlloc ) ) { return false; }
		CVulkan::m_pfBindImageMemory( _pvkDevice->GetDevice(), _piLut->Get(), _pdmLutMemory->Get(), 0 );

		VkImageViewCreateInfo ivciView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		ivciView.image = _piLut->Get();
		ivciView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivciView.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		ivciView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ivciView.subresourceRange.levelCount = 1;
		ivciView.subresourceRange.layerCount = 1;
		if ( !_ivLutView.Create( _pvkDevice->GetDevice(), &ivciView ) ) { return false; }

		// Upload Buffer
		_pbUpload = std::make_unique<CVulkanBuffer>();
		VkBufferCreateInfo bciBuffer = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bciBuffer.size = stLutBytes;
		bciBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bciBuffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if ( !_pbUpload->CreateBuffer( _pvkDevice->GetDevice(), &bciBuffer ) ) { return false; }

		CVulkan::m_pfGetBufferMemoryRequirements( _pvkDevice->GetDevice(), _pbUpload->Get(), &mrMemReq );
		maiAlloc.allocationSize = mrMemReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
		
		_pdmUploadMemory = std::make_unique<CVulkanDeviceMemory>();
		if ( !_pdmUploadMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiAlloc ) ) { return false; }
		CVulkan::m_pfBindBufferMemory( _pvkDevice->GetDevice(), _pbUpload->Get(), _pdmUploadMemory->Get(), 0 );

		void* pvData = nullptr;
		if ( CVulkan::m_pfMapMemory( _pvkDevice->GetDevice(), _pdmUploadMemory->Get(), 0, stLutBytes, 0, &pvData ) == VK_SUCCESS ) {
			
			// =========================================================================
			// TODO: Execute CResamplerBase weights/indices logic here.
			// Example: std::memcpy( pvData, m_vWeights.data(), stLutBytes );
			// =========================================================================

			CVulkan::m_pfUnmapMemory( _pvkDevice->GetDevice(), _pdmUploadMemory->Get() );
		}

		// Copy Commands and Layout Transitions
		VkImageMemoryBarrier imbBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		imbBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imbBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imbBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imbBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imbBarrier.image = _piLut->Get();
		imbBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imbBarrier.subresourceRange.levelCount = 1;
		imbBarrier.subresourceRange.layerCount = 1;
		imbBarrier.srcAccessMask = 0;
		imbBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		CVulkan::m_pfCmdPipelineBarrier( _pcbCommandList->Get(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imbBarrier );

		VkBufferImageCopy bicCopy = {};
		bicCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bicCopy.imageSubresource.layerCount = 1;
		bicCopy.imageExtent = { _ui32OutMaxTaps, _ui32DstSize, 1 };
		CVulkan::m_pfCmdCopyBufferToImage( _pcbCommandList->Get(), _pbUpload->Get(), _piLut->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bicCopy );

		imbBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imbBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imbBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imbBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		CVulkan::m_pfCmdPipelineBarrier( _pcbCommandList->Get(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imbBarrier );

		return true;
	}

	/**
	 * Ensures the Pipeline State Objects and Descriptor Set Layouts are created.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _vSpirvVert The SPIR-V byte code for the vertex shader.
	 * \param _vSpirvFrag The SPIR-V byte code for the fragment shader.
	 * \return Returns true if the shaders are ready.
	 **/
	bool CVulkanResampler::EnsureShaders( CVulkanDevice * _pvkDevice, const std::vector<uint32_t> &/*_vSpirvVert*/, const std::vector<uint32_t> &/*_vSpirvFrag*/ ) {
		if LSN_UNLIKELY( !_pvkDevice || !m_rpPass1.Valid() || !m_rpPass2.Valid() ) { return false; }
		if ( m_ppPass1.get() && m_ppPass1->Get() ) { return true; }


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
		if ( !m_pdslDescriptorSetLayout->CreateDescriptorSetLayout( _pvkDevice->GetDevice(), &dslciLayoutInfo ) ) { return false; }

		VkDescriptorPoolSize dpsSize = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 };
		VkDescriptorPoolCreateInfo dpciPoolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		dpciPoolInfo.poolSizeCount = 1;
		dpciPoolInfo.pPoolSizes = &dpsSize;
		dpciPoolInfo.maxSets = 2;

		m_pdpDescriptorPool = std::make_unique<CVulkanDescriptorPool>();
		if ( !m_pdpDescriptorPool->CreateDescriptorPool( _pvkDevice->GetDevice(), &dpciPoolInfo ) ) { return false; }

		VkDescriptorSetLayout layouts[] = { m_pdslDescriptorSetLayout->Get(), m_pdslDescriptorSetLayout->Get() };
		VkDescriptorSetAllocateInfo dsaiAllocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		dsaiAllocInfo.descriptorPool = m_pdpDescriptorPool->Get();
		dsaiAllocInfo.descriptorSetCount = 2;
		dsaiAllocInfo.pSetLayouts = layouts;

		VkDescriptorSet allocatedSets[2];
		if ( CVulkan::m_pfAllocateDescriptorSets( _pvkDevice->GetDevice(), &dsaiAllocInfo, allocatedSets ) != VK_SUCCESS ) { return false; }
		m_dsPass1.dsDescriptorSet = allocatedSets[0];
		m_dsPass2.dsDescriptorSet = allocatedSets[1];


		VkPushConstantRange pcrPushConstant = {};
		pcrPushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pcrPushConstant.offset = 0;
		pcrPushConstant.size = 16; // 1 uint + 3 floats padding

		VkPipelineLayoutCreateInfo plciLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		plciLayoutInfo.setLayoutCount = 1;
		plciLayoutInfo.pSetLayouts = layouts;
		plciLayoutInfo.pushConstantRangeCount = 1;
		plciLayoutInfo.pPushConstantRanges = &pcrPushConstant;

		m_pplPipelineLayout = std::make_unique<CVulkanPipelineLayout>();
		if ( !m_pplPipelineLayout->CreatePipelineLayout( _pvkDevice->GetDevice(), &plciLayoutInfo ) ) { return false; }

		static const char * kVsGlsl =
			"#version 450\n"
			"layout(location = 0) in vec4 inPos;\n"
			"layout(location = 1) in vec2 inTex;\n"
			"void main() {\n"
			"    gl_Position = vec4(inPos.x, -inPos.y, inPos.z, inPos.w);\n" // Vulkan Y-flip
			"}\n";

		static const char * kPsResampleXGlsl =
			"#version 450\n"
			"layout(push_constant) uniform Push {\n"
			"    uint maxTaps;\n"
			"    vec3 padding;\n"
			"} push;\n"
			"layout(binding = 0) uniform sampler2D tSrc;\n"
			"layout(binding = 1) uniform sampler2D tLut;\n"
			"layout(location = 0) out vec4 outColor;\n"
			"void main() {\n"
			"    vec4 cFinal = vec4(0.0);\n"
			"    int dstX = int(gl_FragCoord.x);\n"
			"    int srcY = int(gl_FragCoord.y);\n"
			"    for (uint i = 0u; i < push.maxTaps; ++i) {\n"
			"        vec4 vLut = texelFetch(tLut, ivec2(dstX, int(i)), 0);\n"
			"        int srcX = int(vLut.g);\n"
			"        cFinal += texelFetch(tSrc, ivec2(srcX, srcY), 0) * vLut.r;\n"
			"    }\n"
			"    outColor = cFinal;\n"
			"}\n";

		static const char * kPsResampleYGlsl =
			"#version 450\n"
			"layout(push_constant) uniform Push {\n"
			"    uint maxTaps;\n"
			"    vec3 padding;\n"
			"} push;\n"
			"layout(binding = 0) uniform sampler2D tSrc;\n"
			"layout(binding = 1) uniform sampler2D tLut;\n"
			"layout(location = 0) out vec4 outColor;\n"
			"void main() {\n"
			"    vec4 cFinal = vec4(0.0);\n"
			"    int dstX = int(gl_FragCoord.x);\n"
			"    int dstY = int(gl_FragCoord.y);\n"
			"    for (uint i = 0u; i < push.maxTaps; ++i) {\n"
			"        vec4 vLut = texelFetch(tLut, ivec2(dstY, int(i)), 0);\n"
			"        int srcY = int(vLut.g);\n"
			"        cFinal += texelFetch(tSrc, ivec2(dstX, srcY), 0) * vLut.r;\n"
			"    }\n"
			"    outColor = cFinal;\n"
			"}\n";

		std::vector<uint32_t> vVert, vFragX, vFragY;
		if ( !CVulkan::CompileGlslToSpirv( kVsGlsl, "vertex", vVert ) ||
			 !CVulkan::CompileGlslToSpirv( kPsResampleXGlsl, "fragment", vFragX ) ||
			 !CVulkan::CompileGlslToSpirv( kPsResampleYGlsl, "fragment", vFragY ) ) { return false; }

		CVulkan::LSN_SHADER_MODULE smVert, smFragX, smFragY;
		if ( !CVulkan::LoadSpirv( _pvkDevice, vVert, smVert ) ||
			 !CVulkan::LoadSpirv( _pvkDevice, vFragX, smFragX ) ||
			 !CVulkan::LoadSpirv( _pvkDevice, vFragY, smFragY ) ) { return false; }


		VkPipelineShaderStageCreateInfo pssciShaderStages[2] = {};
		pssciShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pssciShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		pssciShaderStages[0].module = smVert.smShaderModule;
		pssciShaderStages[0].pName = "main";

		pssciShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pssciShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		pssciShaderStages[1].module = smFragX.smShaderModule;
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
		prsciRasterizer.cullMode = VK_CULL_MODE_NONE;
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
		

		gpciPipelineInfo.renderPass = m_rpPass1.rpRenderPass;
		m_ppPass1 = std::make_unique<CVulkanPipeline>();
		if ( !m_ppPass1->CreateGraphicsPipeline( _pvkDevice->GetDevice(), &gpciPipelineInfo ) ) { return false; }


		// Swap to Pass 2 Fragment Shader
		pssciShaderStages[1].module = smFragY.smShaderModule;
		gpciPipelineInfo.renderPass = m_rpPass2.rpRenderPass;
		m_ppPass2 = std::make_unique<CVulkanPipeline>();
		if ( !m_ppPass2->CreateGraphicsPipeline( _pvkDevice->GetDevice(), &gpciPipelineInfo ) ) { return false; }

		return true;
	}

	/**
	 * Executes the 2-pass resampling operation.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _pcbCommandList The command buffer in which to record draw commands.
	 * \param _ivSource The view of the source image to resample.
	 * \param _sSourceSampler The sampler to use for the source image.
	 * \param _fbTarget The final framebuffer to output to.
	 * \param _rOutput The destination rectangle.
	 * \param _bFlipY Determines whether the final pass should flip the Y axis.
	 * \return Returns true if rendering succeeded.
	 **/
	bool CVulkanResampler::Render( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkImageView _ivSource, VkSampler _sSourceSampler, VkFramebuffer _fbTarget, const lsw::LSW_RECT &_rOutput, bool /*_bFlipY*/ ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pcbCommandList || !_ivSource || !_fbTarget || !m_ppPass1.get() ) { return false; }

		VkDescriptorImageInfo diiPass1Src = { _sSourceSampler, _ivSource, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkDescriptorImageInfo diiPass1Lut = { m_sSampler.sSampler, m_ivLutXView.ivImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		VkWriteDescriptorSet wdsWrites[2] = {};
		wdsWrites[0] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, m_dsPass1.dsDescriptorSet, 0, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &diiPass1Src, nullptr, nullptr };
		wdsWrites[1] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, m_dsPass1.dsDescriptorSet, 1, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &diiPass1Lut, nullptr, nullptr };
		CVulkan::m_pfUpdateDescriptorSets( _pvkDevice->GetDevice(), 2, wdsWrites, 0, nullptr );

		VkRenderPassBeginInfo rpbiPass1 = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		rpbiPass1.renderPass = m_rpPass1.rpRenderPass;
		rpbiPass1.framebuffer = m_fbIntermediate.fbFramebuffer;
		rpbiPass1.renderArea.offset = { 0, 0 };
		rpbiPass1.renderArea.extent = { m_ui32DstW, m_ui32SrcH };
		
		CVulkan::m_pfCmdBeginRenderPass( _pcbCommandList->Get(), &rpbiPass1, VK_SUBPASS_CONTENTS_INLINE );

		VkViewport vViewport1 = { 0.0f, 0.0f, static_cast<float>(m_ui32DstW), static_cast<float>(m_ui32SrcH), 0.0f, 1.0f };
		CVulkan::m_pfCmdSetViewport( _pcbCommandList->Get(), 0, 1, &vViewport1 );
		VkRect2D rScissor1 = { { 0, 0 }, { m_ui32DstW, m_ui32SrcH } };
		CVulkan::m_pfCmdSetScissor( _pcbCommandList->Get(), 0, 1, &rScissor1 );

		CVulkan::m_pfCmdBindPipeline( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_ppPass1->Get() );
		VkBuffer buffers[] = { m_pbVbQuad->Get() };
		VkDeviceSize offsets[] = { 0 };
		CVulkan::m_pfCmdBindVertexBuffers( _pcbCommandList->Get(), 0, 1, buffers, offsets );
		CVulkan::m_pfCmdBindDescriptorSets( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pplPipelineLayout->Get(), 0, 1, &m_dsPass1.dsDescriptorSet, 0, nullptr );

		// Guaranteed layout matching: 4 bytes for MaxTaps, followed by 12 bytes of padding.
		uint32_t ui32PushPass1[4] = { m_ui32MaxTapsX, 0, 0, 0 };
		CVulkan::m_pfCmdPushConstants( _pcbCommandList->Get(), m_pplPipelineLayout->Get(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, 16, ui32PushPass1 );
		CVulkan::m_pfCmdDraw( _pcbCommandList->Get(), 4, 1, 0, 0 );
		CVulkan::m_pfCmdEndRenderPass( _pcbCommandList->Get() );

		VkImageMemoryBarrier imbIntermediate = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		imbIntermediate.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; 
		imbIntermediate.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imbIntermediate.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imbIntermediate.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		imbIntermediate.image = m_piIntermediate->Get();
		imbIntermediate.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		CVulkan::m_pfCmdPipelineBarrier( _pcbCommandList->Get(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imbIntermediate );

		VkDescriptorImageInfo diiPass2Src = { m_sSampler.sSampler, m_ivIntermediateView.ivImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkDescriptorImageInfo diiPass2Lut = { m_sSampler.sSampler, m_ivLutYView.ivImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		wdsWrites[0].dstSet = m_dsPass2.dsDescriptorSet; wdsWrites[0].pImageInfo = &diiPass2Src;
		wdsWrites[1].dstSet = m_dsPass2.dsDescriptorSet; wdsWrites[1].pImageInfo = &diiPass2Lut;
		CVulkan::m_pfUpdateDescriptorSets( _pvkDevice->GetDevice(), 2, wdsWrites, 0, nullptr );

		VkRenderPassBeginInfo rpbiPass2 = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		rpbiPass2.renderPass = m_rpPass2.rpRenderPass;
		rpbiPass2.framebuffer = _fbTarget;
		rpbiPass2.renderArea.offset = { 0, 0 };
		rpbiPass2.renderArea.extent = { static_cast<uint32_t>(_rOutput.Width()), static_cast<uint32_t>(_rOutput.Height()) };
		
		CVulkan::m_pfCmdBeginRenderPass( _pcbCommandList->Get(), &rpbiPass2, VK_SUBPASS_CONTENTS_INLINE );

		VkViewport vViewport2 = { 0.0f, 0.0f, static_cast<float>(_rOutput.Width()), static_cast<float>(_rOutput.Height()), 0.0f, 1.0f };
		CVulkan::m_pfCmdSetViewport( _pcbCommandList->Get(), 0, 1, &vViewport2 );
		VkRect2D rScissor2 = { { 0, 0 }, { static_cast<uint32_t>(_rOutput.Width()), static_cast<uint32_t>(_rOutput.Height()) } };
		CVulkan::m_pfCmdSetScissor( _pcbCommandList->Get(), 0, 1, &rScissor2 );

		CVulkan::m_pfCmdBindPipeline( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_ppPass2->Get() );
		CVulkan::m_pfCmdBindVertexBuffers( _pcbCommandList->Get(), 0, 1, buffers, offsets );
		CVulkan::m_pfCmdBindDescriptorSets( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pplPipelineLayout->Get(), 0, 1, &m_dsPass2.dsDescriptorSet, 0, nullptr );

		uint32_t ui32PushPass2[4] = { m_ui32MaxTapsY, 0, 0, 0 };
		CVulkan::m_pfCmdPushConstants( _pcbCommandList->Get(), m_pplPipelineLayout->Get(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, 16, ui32PushPass2 );
		CVulkan::m_pfCmdDraw( _pcbCommandList->Get(), 4, 1, 0, 0 );
		CVulkan::m_pfCmdEndRenderPass( _pcbCommandList->Get() );

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
