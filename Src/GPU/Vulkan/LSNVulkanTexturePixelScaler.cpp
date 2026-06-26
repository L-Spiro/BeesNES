/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for applying integer nearest-neighbor scaling to a texture.
 */

#ifdef LSN_VULKAN1

#include "LSNVulkanTexturePixelScaler.h"

namespace lsn {

	CVulkanTexturePixelScaler::CVulkanTexturePixelScaler() {
	}
	CVulkanTexturePixelScaler::~CVulkanTexturePixelScaler() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the resources and internal states.
	 **/
	void CVulkanTexturePixelScaler::Reset() {
		m_ivTargetView.Reset();
		if LSN_LIKELY( m_pdmTargetMemory.get() ) { m_pdmTargetMemory->Reset(); }
		if LSN_LIKELY( m_piTarget.get() ) { m_piTarget->Reset(); }
		
		if LSN_LIKELY( m_pdmVbQuadMemory.get() ) { m_pdmVbQuadMemory->Reset(); }
		if LSN_LIKELY( m_pbVbQuad.get() ) { m_pbVbQuad->Reset(); }

		if LSN_LIKELY( m_ppShader.get() ) { m_ppShader->Reset(); }
		if LSN_LIKELY( m_pplPipelineLayout.get() ) { m_pplPipelineLayout->Reset(); }
		if LSN_LIKELY( m_pdslDescriptorSetLayout.get() ) { m_pdslDescriptorSetLayout->Reset(); }

		m_piTarget.reset();
		m_pdmTargetMemory.reset();
		m_pbVbQuad.reset();
		m_pdmVbQuadMemory.reset();
		m_ppShader.reset();
		m_pplPipelineLayout.reset();
		m_pdslDescriptorSetLayout.reset();

		m_ui32DstW = 0;
		m_ui32DstH = 0;
		m_fFormat = VK_FORMAT_UNDEFINED;
	}

	/**
	 * Ensures the target resources are created and match the requested size.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _ui32DstW The target width.
	 * \param _ui32DstH The target height.
	 * \param _fFormat The target format.
	 * \return Returns true if resources are ready.
	 **/
	bool CVulkanTexturePixelScaler::EnsureResources( CVulkanDevice * _pvkDevice, uint32_t _ui32DstW, uint32_t _ui32DstH, VkFormat _fFormat ) {
		if LSN_UNLIKELY( !_pvkDevice ) { return false; }
		
		if ( m_piTarget.get() && m_piTarget->Get() && m_ui32DstW == m_ui32DstW && m_ui32DstH == m_ui32DstH && m_fFormat == _fFormat ) {
			return true;
		}

		m_ivTargetView.Reset();
		if ( m_pdmTargetMemory.get() ) { m_pdmTargetMemory->Reset(); }
		if ( m_piTarget.get() ) { m_piTarget->Reset(); }

		m_piTarget = std::make_unique<CVulkanImage>();
		VkImageCreateInfo iciImageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		iciImageInfo.imageType = VK_IMAGE_TYPE_2D;
		iciImageInfo.extent.width = _ui32DstW;
		iciImageInfo.extent.height = _ui32DstH;
		iciImageInfo.extent.depth = 1;
		iciImageInfo.mipLevels = 1;
		iciImageInfo.arrayLayers = 1;
		iciImageInfo.format = _fFormat;
		iciImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		iciImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		iciImageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		iciImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		iciImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		if ( !m_piTarget->CreateImage( _pvkDevice->GetDevice(), &iciImageInfo ) ) { return false; }

		VkMemoryRequirements mrImageMemReq;
		CVulkan::m_pfGetImageMemoryRequirements( _pvkDevice->GetDevice(), m_piTarget->Get(), &mrImageMemReq );

		VkMemoryAllocateInfo maiImageAllocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		maiImageAllocInfo.allocationSize = mrImageMemReq.size;
		maiImageAllocInfo.memoryTypeIndex = FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrImageMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		m_pdmTargetMemory = std::make_unique<CVulkanDeviceMemory>();
		if ( !m_pdmTargetMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiImageAllocInfo ) ) { return false; }
		CVulkan::m_pfBindImageMemory( _pvkDevice->GetDevice(), m_piTarget->Get(), m_pdmTargetMemory->Get(), 0 );

		VkImageViewCreateInfo ivciViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		ivciViewInfo.image = m_piTarget->Get();
		ivciViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivciViewInfo.format = _fFormat;
		ivciViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ivciViewInfo.subresourceRange.baseMipLevel = 0;
		ivciViewInfo.subresourceRange.levelCount = 1;
		ivciViewInfo.subresourceRange.baseArrayLayer = 0;
		ivciViewInfo.subresourceRange.layerCount = 1;
		
		if ( !m_ivTargetView.Create( _pvkDevice->GetDevice(), &ivciViewInfo ) ) { return false; }

		m_ui32DstW = _ui32DstW;
		m_ui32DstH = _ui32DstH;
		m_fFormat = _fFormat;

		return true;
	}

	/**
	 * Ensures the Pipeline State Object is created.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _rpRenderPass The render pass that the pipeline will execute within.
	 * \param _fFormat The target format for the pipeline.
	 * \param _vSpirvVert The SPIR-V byte code for the vertex shader.
	 * \param _vSpirvFrag The SPIR-V byte code for the fragment shader.
	 * \return Returns true if the shader is ready.
	 **/
	bool CVulkanTexturePixelScaler::EnsureShader( CVulkanDevice * _pvkDevice, VkRenderPass _rpRenderPass, VkFormat /*_fFormat*/, const std::vector<uint32_t> &_vSpirvVert, const std::vector<uint32_t> &_vSpirvFrag ) {
		if LSN_UNLIKELY( !_pvkDevice ) { return false; }
		if ( m_ppShader.get() && m_ppShader->Get() ) { return true; }


		VkDescriptorSetLayoutBinding dslbBinding = {};
		dslbBinding.binding = 0;
		dslbBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dslbBinding.descriptorCount = 1;
		dslbBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		dslbBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo dslciLayoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		dslciLayoutInfo.bindingCount = 1;
		dslciLayoutInfo.pBindings = &dslbBinding;

		m_pdslDescriptorSetLayout = std::make_unique<CVulkanDescriptorSetLayout>();
		if ( !m_pdslDescriptorSetLayout->CreateDescriptorSetLayout( _pvkDevice->GetDevice(), &dslciLayoutInfo ) ) { return false; }


		VkPushConstantRange pcrPushConstant = {};
		pcrPushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;
		pcrPushConstant.offset = 0;
		pcrPushConstant.size = sizeof( LSN_SCALER_CONSTANTS );

		VkDescriptorSetLayout layouts[] = { m_pdslDescriptorSetLayout->Get() };
		VkPipelineLayoutCreateInfo plciLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		plciLayoutInfo.setLayoutCount = 1;
		plciLayoutInfo.pSetLayouts = layouts;
		plciLayoutInfo.pushConstantRangeCount = 1;
		plciLayoutInfo.pPushConstantRanges = &pcrPushConstant;

		m_pplPipelineLayout = std::make_unique<CVulkanPipelineLayout>();
		if ( !m_pplPipelineLayout->CreatePipelineLayout( _pvkDevice->GetDevice(), &plciLayoutInfo ) ) { return false; }


		CVulkan::LSN_SHADER_MODULE smVert, smFrag;
		if ( !LoadSpirv( _pvkDevice, _vSpirvVert, smVert ) || !LoadSpirv( _pvkDevice, _vSpirvFrag, smFrag ) ) { return false; }

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
		viadAttributes[0].location = 0;							// Position.
		viadAttributes[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		viadAttributes[0].offset = offsetof( LSN_XYZRHWTEX1, fX );

		viadAttributes[1].binding = 0;
		viadAttributes[1].location = 1;							// TexCoord.
		viadAttributes[1].format = VK_FORMAT_R32G32_SFLOAT;
		viadAttributes[1].offset = offsetof( LSN_XYZRHWTEX1, fU );

		VkPipelineVertexInputStateCreateInfo pvisciVertexInput = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		pvisciVertexInput.vertexBindingDescriptionCount = 1;
		pvisciVertexInput.pVertexBindingDescriptions = &vibdBindingDesc;
		pvisciVertexInput.vertexAttributeDescriptionCount = 2;
		pvisciVertexInput.pVertexAttributeDescriptions = viadAttributes;

		VkPipelineInputAssemblyStateCreateInfo piasciInputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		piasciInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		piasciInputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo pvsciViewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		pvsciViewportState.viewportCount = 1;
		pvsciViewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo prsciRasterizer = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		prsciRasterizer.depthClampEnable = VK_FALSE;
		prsciRasterizer.rasterizerDiscardEnable = VK_FALSE;
		prsciRasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		prsciRasterizer.lineWidth = 1.0f;
		prsciRasterizer.cullMode = VK_CULL_MODE_NONE;
		prsciRasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

		VkPipelineMultisampleStateCreateInfo pmsciMultisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		pmsciMultisampling.sampleShadingEnable = VK_FALSE;
		pmsciMultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState pcbasColorBlendAttachment = {};
		pcbasColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		pcbasColorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo pcbsciColorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		pcbsciColorBlending.logicOpEnable = VK_FALSE;
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
		gpciPipelineInfo.renderPass = _rpRenderPass;
		gpciPipelineInfo.subpass = 0;

		m_ppShader = std::make_unique<CVulkanPipeline>();
		if ( !m_ppShader->CreateGraphicsPipeline( _pvkDevice->GetDevice(), &gpciPipelineInfo ) ) { return false; }


		LSN_XYZRHWTEX1 Quad[] = {
			{ -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f },
			{  1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f },
			{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{  1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f },
		};

		m_pbVbQuad = std::make_unique<CVulkanBuffer>();
		VkBufferCreateInfo bciBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bciBufferInfo.size = sizeof( Quad );
		bciBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bciBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if ( !m_pbVbQuad->CreateBuffer( _pvkDevice->GetDevice(), &bciBufferInfo ) ) { return false; }

		VkMemoryRequirements mrVbMemReq;
		CVulkan::m_pfGetBufferMemoryRequirements( _pvkDevice->GetDevice(), m_pbVbQuad->Get(), &mrVbMemReq );

		VkMemoryAllocateInfo maiVbAllocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		maiVbAllocInfo.allocationSize = mrVbMemReq.size;
		maiVbAllocInfo.memoryTypeIndex = FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrVbMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

		m_pdmVbQuadMemory = std::make_unique<CVulkanDeviceMemory>();
		if ( !m_pdmVbQuadMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiVbAllocInfo ) ) { return false; }
		CVulkan::m_pfBindBufferMemory( _pvkDevice->GetDevice(), m_pbVbQuad->Get(), m_pdmVbQuadMemory->Get(), 0 );

		void * pvData = nullptr;
		if ( CVulkan::m_pfMapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get(), 0, sizeof( Quad ), 0, &pvData ) == VK_SUCCESS ) {
			std::memcpy( pvData, Quad, sizeof( Quad ) );
			CVulkan::m_pfUnmapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get() );
		}

		return true;
	}

	/**
	 * Renders the integer scaling quad.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _pcbCommandList The command buffer in which to record draw commands.
	 * \param _dsSourceTexture The descriptor set bounding the source image and sampler.
	 * \param _ui32NativeW The base resolution width (prior to integer scaling).
	 * \param _ui32NativeH The base resolution height (prior to integer scaling).
	 * \param _rOutput The destination rectangle in client pixels.
	 * \param _bFlipY Determines whether the integer scaler pass should flip the Y axis.
	 * \return Returns true if rendering commands were successfully recorded.
	 **/
	bool CVulkanTexturePixelScaler::Render( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkDescriptorSet _dsSourceTexture, uint32_t _ui32NativeW, uint32_t _ui32NativeH, const lsw::LSW_RECT &_rOutput, bool _bFlipY ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pcbCommandList || !_dsSourceTexture || !m_ppShader.get() ) { return false; }

		// 1. Set Viewport and Scissor
		VkViewport vViewport = {};
		vViewport.x = static_cast<float>(_rOutput.left);
		vViewport.y = static_cast<float>(_rOutput.top);
		vViewport.width = static_cast<float>(_rOutput.Width());
		vViewport.height = static_cast<float>(_rOutput.Height());
		vViewport.minDepth = 0.0f;
		vViewport.maxDepth = 1.0f;
		CVulkan::m_pfCmdSetViewport( _pcbCommandList->Get(), 0, 1, &vViewport );

		VkRect2D rScissor = {};
		rScissor.offset = { _rOutput.left, _rOutput.top };
		rScissor.extent = { static_cast<uint32_t>(_rOutput.Width()), static_cast<uint32_t>(_rOutput.Height()) };
		CVulkan::m_pfCmdSetScissor( _pcbCommandList->Get(), 0, 1, &rScissor );

		// 2. Bind Pipeline
		CVulkan::m_pfCmdBindPipeline( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_ppShader->Get() );

		// 3. Bind Vertex Buffer
		VkBuffer buffers[] = { m_pbVbQuad->Get() };
		VkDeviceSize offsets[] = { 0 };
		CVulkan::m_pfCmdBindVertexBuffers( _pcbCommandList->Get(), 0, 1, buffers, offsets );

		// 4. Bind Descriptor Set
		CVulkan::m_pfCmdBindDescriptorSets( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pplPipelineLayout->Get(), 0, 1, &_dsSourceTexture, 0, nullptr );

		// 5. Push Constants (Replacing the DX12 Root Signature Constants)
		LSN_SCALER_CONSTANTS scConstants = { _ui32NativeW, _ui32NativeH, _bFlipY ? 1.0f : 0.0f };
		CVulkan::m_pfCmdPushConstants( _pcbCommandList->Get(), m_pplPipelineLayout->Get(), VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof( LSN_SCALER_CONSTANTS ), &scConstants );

		// 6. Draw Quad
		CVulkan::m_pfCmdDraw( _pcbCommandList->Get(), 4, 1, 0, 0 );

		return true;
	}

	/**
	 * Creates a Vulkan shader module from SPIR-V code.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _vSpirv The compiled SPIR-V code.
	 * \param _smModule The shader module wrapper to populate.
	 * \return Returns true on success.
	 **/
	bool CVulkanTexturePixelScaler::LoadSpirv( CVulkanDevice * _pvkDevice, const std::vector<uint32_t> &_vSpirv, CVulkan::LSN_SHADER_MODULE &_smModule ) {
		VkShaderModuleCreateInfo smciInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		smciInfo.codeSize = _vSpirv.size() * sizeof( uint32_t );
		smciInfo.pCode = _vSpirv.data();
		return _smModule.Create( _pvkDevice->GetDevice(), &smciInfo );
	}

	/**
	 * Helper to find an appropriate memory type index for allocations.
	 * 
	 * \param _pdDevice The physical device.
	 * \param _ui32TypeFilter A bitmask specifying the acceptable memory types.
	 * \param _mpfProperties The required memory properties.
	 * \return Returns the index of the memory type, or 0 if none is found.
	 **/
	uint32_t CVulkanTexturePixelScaler::FindMemoryType( VkPhysicalDevice _pdDevice, uint32_t _ui32TypeFilter, VkMemoryPropertyFlags _mpfProperties ) {
		VkPhysicalDeviceMemoryProperties pdmpMemProperties;
		CVulkan::m_pfGetPhysicalDeviceMemoryProperties( _pdDevice, &pdmpMemProperties );

		for ( uint32_t I = 0; I < pdmpMemProperties.memoryTypeCount; ++I ) {
			if ( (_ui32TypeFilter & (1 << I)) && (pdmpMemProperties.memoryTypes[I].propertyFlags & _mpfProperties) == _mpfProperties ) {
				return I;
			}
		}
		return 0; 
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
