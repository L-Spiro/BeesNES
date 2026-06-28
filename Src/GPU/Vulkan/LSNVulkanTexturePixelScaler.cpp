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
		if ( m_piTarget.get() && m_ui32DstW == _ui32DstW && m_ui32DstH == _ui32DstH ) { return true; }

		Reset();

		m_piTarget = std::make_unique<CVulkanImage>();
		VkImageCreateInfo iciInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		iciInfo.imageType = VK_IMAGE_TYPE_2D;
		iciInfo.extent = { _ui32DstW, _ui32DstH, 1 };
		iciInfo.mipLevels = 1;
		iciInfo.arrayLayers = 1;
		iciInfo.format = _fFormat;
		iciInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		iciInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		iciInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		iciInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		if ( !m_piTarget->CreateImage( _pvkDevice->GetDevice(), &iciInfo ) ) { return false; }

		VkMemoryRequirements mrReq;
		CVulkan::m_pfGetImageMemoryRequirements( _pvkDevice->GetDevice(), m_piTarget->Get(), &mrReq );

		VkMemoryAllocateInfo maiAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		maiAlloc.allocationSize = mrReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		m_pdmTargetMemory = std::make_unique<CVulkanDeviceMemory>();
		if ( !m_pdmTargetMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiAlloc ) ) { return false; }
		CVulkan::m_pfBindImageMemory( _pvkDevice->GetDevice(), m_piTarget->Get(), m_pdmTargetMemory->Get(), 0 );

		VkImageViewCreateInfo ivciView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		ivciView.image = m_piTarget->Get();
		ivciView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivciView.format = _fFormat;
		ivciView.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		if ( !m_ivTargetView.Create( _pvkDevice->GetDevice(), &ivciView ) ) { return false; }

		// --- Quad Creation ---
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

		CVulkan::m_pfGetBufferMemoryRequirements( _pvkDevice->GetDevice(), m_pbVbQuad->Get(), &mrReq );
		maiAlloc.allocationSize = mrReq.size;
		maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
		
		m_pdmVbQuadMemory = std::make_unique<CVulkanDeviceMemory>();
		if ( !m_pdmVbQuadMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiAlloc ) ) { return false; }
		CVulkan::m_pfBindBufferMemory( _pvkDevice->GetDevice(), m_pbVbQuad->Get(), m_pdmVbQuadMemory->Get(), 0 );

		void* pvData = nullptr;
		if ( CVulkan::m_pfMapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get(), 0, sizeof( Quad ), 0, &pvData ) == VK_SUCCESS ) {
			std::memcpy( pvData, Quad, sizeof( Quad ) );
			CVulkan::m_pfUnmapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get() );
		}

		m_ui32DstW = _ui32DstW;
		m_ui32DstH = _ui32DstH;

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
	bool CVulkanTexturePixelScaler::EnsureShaders( CVulkanDevice * _pvkDevice, VkRenderPass _rpRenderPass, VkFormat /*_fFormat*/, const std::vector<uint32_t> &/*_vSpirvVert*/, const std::vector<uint32_t> &/*_vSpirvFrag*/ ) {
		if LSN_UNLIKELY( !_pvkDevice || !_rpRenderPass ) { return false; }
		if ( m_ppShader.get() && m_ppShader->Get() ) { return true; }

		VkDescriptorSetLayoutBinding dslbBinding = {};
		dslbBinding.binding = 0;
		dslbBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dslbBinding.descriptorCount = 1;
		dslbBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo dslciLayoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		dslciLayoutInfo.bindingCount = 1;
		dslciLayoutInfo.pBindings = &dslbBinding;

		m_pdslDescriptorSetLayout = std::make_unique<CVulkanDescriptorSetLayout>();
		if ( !m_pdslDescriptorSetLayout->CreateDescriptorSetLayout( _pvkDevice->GetDevice(), &dslciLayoutInfo ) ) { return false; }

		VkPushConstantRange pcrPushConstant = {};
		pcrPushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pcrPushConstant.offset = 0;
		pcrPushConstant.size = 16; 

		VkDescriptorSetLayout layouts[] = { m_pdslDescriptorSetLayout->Get() };
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
			"layout(location = 0) out vec2 outTex;\n"
			"void main() {\n"
			"    gl_Position = vec4(inPos.x, -inPos.y, inPos.z, inPos.w);\n"
			"    outTex = inTex;\n"
			"}\n";

		static const char * kPsGlsl =
			"#version 450\n"
			"layout(push_constant) uniform Push { vec4 c0; } push;\n"
			"layout(binding = 0) uniform sampler2D tSrc;\n"
			"layout(location = 0) in vec2 inTex;\n"
			"layout(location = 0) out vec4 outColor;\n"
			"void main() {\n"
			"    float v = (floor(inTex.y * push.c0.x) + push.c0.z) * push.c0.y;\n"
			"    outColor = texture(tSrc, vec2(inTex.x, v));\n"
			"}\n";

		std::vector<uint32_t> vVert, vFrag;
		if ( !CVulkan::CompileGlslToSpirv( kVsGlsl, "vertex", vVert ) || !CVulkan::CompileGlslToSpirv( kPsGlsl, "fragment", vFrag ) ) { return false; }

		CVulkan::LSN_SHADER_MODULE smVert, smFrag;
		if ( !CVulkan::LoadSpirv( _pvkDevice, vVert, smVert ) || !CVulkan::LoadSpirv( _pvkDevice, vFrag, smFrag ) ) { return false; }

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
	bool CVulkanTexturePixelScaler::Render( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkDescriptorSet _dsSourceTexture, uint32_t _ui32SrcW, uint32_t _ui32SrcH, const lsw::LSW_RECT &/*_rOutput*/, bool /*_bFlipY*/ ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pcbCommandList || !_dsSourceTexture || !m_ppShader.get() ) { return false; }

		VkViewport vViewport = {};
		vViewport.x = 0.0f;
		vViewport.y = 0.0f;
		vViewport.width = static_cast<float>(m_ui32DstW);
		vViewport.height = static_cast<float>(m_ui32DstH);
		vViewport.minDepth = 0.0f;
		vViewport.maxDepth = 1.0f;
		CVulkan::m_pfCmdSetViewport( _pcbCommandList->Get(), 0, 1, &vViewport );

		VkRect2D rScissor = {};
		rScissor.offset = { 0, 0 };
		rScissor.extent = { m_ui32DstW, m_ui32DstH };
		CVulkan::m_pfCmdSetScissor( _pcbCommandList->Get(), 0, 1, &rScissor );


		CVulkan::m_pfCmdBindPipeline( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_ppShader->Get() );


		VkBuffer buffers[] = { m_pbVbQuad->Get() };
		VkDeviceSize offsets[] = { 0 };
		CVulkan::m_pfCmdBindVertexBuffers( _pcbCommandList->Get(), 0, 1, buffers, offsets );
		CVulkan::m_pfCmdBindDescriptorSets( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pplPipelineLayout->Get(), 0, 1, &_dsSourceTexture, 0, nullptr );

		float fSrcH = static_cast<float>(_ui32SrcH);
		float fC0[4] = { fSrcH, 1.0f / fSrcH, 0.5f, 0.0f };
		CVulkan::m_pfCmdPushConstants( _pcbCommandList->Get(), m_pplPipelineLayout->Get(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof( fC0 ), fC0 );

		CVulkan::m_pfCmdDraw( _pcbCommandList->Get(), 4, 1, 0, 0 );

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
