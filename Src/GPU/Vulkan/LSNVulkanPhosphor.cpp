/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 hardware wrapper for a phosphor-decay post-processing effect.
 */

#ifdef LSN_VULKAN1

#include "LSNVulkanPhosphor.h"
#include <cstring>

namespace lsn {

	CVulkanPhosphor::CVulkanPhosphor() :
		m_stRenderTargetIdx( 0 ) {
	}
	CVulkanPhosphor::~CVulkanPhosphor() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the resources and internal states.
	 **/
	void CVulkanPhosphor::Reset() {
		for ( int I = 0; I < 2; ++I ) {
			m_dsPhosphor[I].Reset();
			m_fbPhosphor[I].Reset();
			m_ivPhosphorView[I].Reset();
			if LSN_LIKELY( m_pdmPhosphorMemory[I].get() ) { m_pdmPhosphorMemory[I]->Reset(); }
			if LSN_LIKELY( m_piPhosphor[I].get() ) { m_piPhosphor[I]->Reset(); }

			m_piPhosphor[I].reset();
			m_pdmPhosphorMemory[I].reset();
		}

		if LSN_LIKELY( m_pdmVbQuadMemory.get() ) { m_pdmVbQuadMemory->Reset(); }
		if LSN_LIKELY( m_pbVbQuad.get() ) { m_pbVbQuad->Reset(); }
		if LSN_LIKELY( m_ppShader.get() ) { m_ppShader->Reset(); }
		if LSN_LIKELY( m_pplPipelineLayout.get() ) { m_pplPipelineLayout->Reset(); }
		if LSN_LIKELY( m_pdslDescriptorSetLayout.get() ) { m_pdslDescriptorSetLayout->Reset(); }
		if LSN_LIKELY( m_pdpDescriptorPool.get() ) { m_pdpDescriptorPool->Reset(); }

		m_pbVbQuad.reset();
		m_pdmVbQuadMemory.reset();
		m_ppShader.reset();
		m_pplPipelineLayout.reset();
		m_pdslDescriptorSetLayout.reset();
		m_pdpDescriptorPool.reset();
		
		m_rpRenderPass.Reset();
		m_sSampler.Reset();

		m_stRenderTargetIdx = 0;
		m_ui32SrcW = 0;
		m_ui32SrcH = 0;
		m_fFormat = VK_FORMAT_UNDEFINED;
	}

	/**
	 * Ensures the ping-pong render targets are created and properly sized.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _pcbCommandList The command buffer used to record initial layout transition barriers.
	 * \param _ui32SrcW The width of the source output.
	 * \param _ui32SrcH The height of the source output.
	 * \param _fTargetFormat The format of the RTV this shader will output to (and the ping-pong buffers).
	 * \return Returns true if resources are ready.
	 **/
	bool CVulkanPhosphor::EnsureResources( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, uint32_t _ui32SrcW, uint32_t _ui32SrcH, VkFormat _fTargetFormat ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pcbCommandList ) { return false; }
		
		if ( m_piPhosphor[0].get() && m_ui32SrcW == m_ui32SrcW && m_ui32SrcH == m_ui32SrcH && m_fFormat == _fTargetFormat ) {
			return true;
		}

		Reset();

		// 1. Create Render Pass
		VkAttachmentDescription adAttachment = {};
		adAttachment.format = _fTargetFormat;
		adAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		adAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // We draw a full screen quad over it.
		adAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		adAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		adAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		adAttachment.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		adAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference arColorRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

		VkSubpassDescription sdSubpass = {};
		sdSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		sdSubpass.colorAttachmentCount = 1;
		sdSubpass.pColorAttachments = &arColorRef;

		VkRenderPassCreateInfo rpciInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		rpciInfo.attachmentCount = 1;
		rpciInfo.pAttachments = &adAttachment;
		rpciInfo.subpassCount = 1;
		rpciInfo.pSubpasses = &sdSubpass;

		if ( !m_rpRenderPass.Create( _pvkDevice->GetDevice(), &rpciInfo ) ) { return false; }

		// 2. Create the two ping-pong buffers.
		VkImageMemoryBarrier imbBarriers[2] = {};
		for ( int I = 0; I < 2; ++I ) {
			m_piPhosphor[I] = std::make_unique<CVulkanImage>();
			VkImageCreateInfo iciImageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			iciImageInfo.imageType = VK_IMAGE_TYPE_2D;
			iciImageInfo.extent.width = _ui32SrcW;
			iciImageInfo.extent.height = _ui32SrcH;
			iciImageInfo.extent.depth = 1;
			iciImageInfo.mipLevels = 1;
			iciImageInfo.arrayLayers = 1;
			iciImageInfo.format = _fTargetFormat;
			iciImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			iciImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			iciImageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			iciImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			iciImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

			if ( !m_piPhosphor[I]->CreateImage( _pvkDevice->GetDevice(), &iciImageInfo ) ) { return false; }

			VkMemoryRequirements mrImageMemReq;
			CVulkan::m_pfGetImageMemoryRequirements( _pvkDevice->GetDevice(), m_piPhosphor[I]->Get(), &mrImageMemReq );

			VkMemoryAllocateInfo maiImageAllocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			maiImageAllocInfo.allocationSize = mrImageMemReq.size;
			maiImageAllocInfo.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrImageMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

			m_pdmPhosphorMemory[I] = std::make_unique<CVulkanDeviceMemory>();
			if ( !m_pdmPhosphorMemory[I]->AllocateMemory( _pvkDevice->GetDevice(), &maiImageAllocInfo ) ) { return false; }
			CVulkan::m_pfBindImageMemory( _pvkDevice->GetDevice(), m_piPhosphor[I]->Get(), m_pdmPhosphorMemory[I]->Get(), 0 );

			VkImageViewCreateInfo ivciViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			ivciViewInfo.image = m_piPhosphor[I]->Get();
			ivciViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ivciViewInfo.format = _fTargetFormat;
			ivciViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ivciViewInfo.subresourceRange.baseMipLevel = 0;
			ivciViewInfo.subresourceRange.levelCount = 1;
			ivciViewInfo.subresourceRange.baseArrayLayer = 0;
			ivciViewInfo.subresourceRange.layerCount = 1;
			
			if ( !m_ivPhosphorView[I].Create( _pvkDevice->GetDevice(), &ivciViewInfo ) ) { return false; }

			VkFramebufferCreateInfo fbciInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			fbciInfo.renderPass = m_rpRenderPass.rpRenderPass;
			fbciInfo.attachmentCount = 1;
			fbciInfo.pAttachments = &m_ivPhosphorView[I].ivImageView;
			fbciInfo.width = _ui32SrcW;
			fbciInfo.height = _ui32SrcH;
			fbciInfo.layers = 1;

			if ( !m_fbPhosphor[I].Create( _pvkDevice->GetDevice(), &fbciInfo ) ) { return false; }

			// Prep transition barriers
			imbBarriers[I].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imbBarriers[I].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imbBarriers[I].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imbBarriers[I].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imbBarriers[I].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imbBarriers[I].image = m_piPhosphor[I]->Get();
			imbBarriers[I].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imbBarriers[I].subresourceRange.baseMipLevel = 0;
			imbBarriers[I].subresourceRange.levelCount = 1;
			imbBarriers[I].subresourceRange.baseArrayLayer = 0;
			imbBarriers[I].subresourceRange.layerCount = 1;
			imbBarriers[I].srcAccessMask = 0;
			imbBarriers[I].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}

		// 3. Transition to SHADER_READ_ONLY_OPTIMAL.
		// The first pass reads from [1] while writing to [0]. Both must be initialized.
		CVulkan::m_pfCmdPipelineBarrier(
			_pcbCommandList->Get(),
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			2, imbBarriers
		);

		// 4. Create Vertex Buffer for a full screen quad.
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
		maiVbAllocInfo.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrVbMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

		m_pdmVbQuadMemory = std::make_unique<CVulkanDeviceMemory>();
		if ( !m_pdmVbQuadMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiVbAllocInfo ) ) { return false; }
		CVulkan::m_pfBindBufferMemory( _pvkDevice->GetDevice(), m_pbVbQuad->Get(), m_pdmVbQuadMemory->Get(), 0 );

		void* pvData = nullptr;
		if ( CVulkan::m_pfMapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get(), 0, sizeof( Quad ), 0, &pvData ) == VK_SUCCESS ) {
			std::memcpy( pvData, Quad, sizeof( Quad ) );
			CVulkan::m_pfUnmapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get() );
		}

		// 5. Create Sampler for the ping-pong reads.
		VkSamplerCreateInfo sciSamplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		sciSamplerInfo.magFilter = VK_FILTER_NEAREST;
		sciSamplerInfo.minFilter = VK_FILTER_NEAREST;
		sciSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		sciSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sciSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sciSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sciSamplerInfo.maxAnisotropy = 1.0f;
		if ( !m_sSampler.Create( _pvkDevice->GetDevice(), &sciSamplerInfo ) ) { return false; }

		m_ui32SrcW = _ui32SrcW;
		m_ui32SrcH = _ui32SrcH;
		m_fFormat = _fTargetFormat;

		return true;
	}

	/**
	 * Ensures the Pipeline State Objects and Descriptor Set Layouts are created.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _fTargetFormat The format of the RTV this shader will output to.
	 * \param _vSpirvVert The SPIR-V byte code for the vertex shader.
	 * \param _vSpirvFrag The SPIR-V byte code for the fragment shader.
	 * \return Returns true if the shaders are ready.
	 **/
	bool CVulkanPhosphor::EnsureShaders( CVulkanDevice * _pvkDevice, VkFormat /*_fTargetFormat*/, const std::vector<uint32_t> &/*_vSpirvVert*/, const std::vector<uint32_t> &/*_vSpirvFrag*/ ) {
		if LSN_UNLIKELY( !_pvkDevice || !m_rpRenderPass.Valid() ) { return false; }
		if ( m_ppShader.get() && m_ppShader->Get() ) { return true; }


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


		VkDescriptorPoolSize dpsSize = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 };	// 2 sets * 2 bindings.
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

		// Move allocated sets into our strict RAII wrappers to guarantee leak-proof cleanup on Reset().
		// Since we didn't specify VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, destroying the pool implicitly destroys these sets.
		// However, the LSN_DESCRIPTOR_SET RAII wrapper will still safely relinquish its handle without an API-level error.
		m_dsPhosphor[0].dsDescriptorSet = allocatedSets[0];
		m_dsPhosphor[1].dsDescriptorSet = allocatedSets[1];


		VkPushConstantRange pcrPushConstant = {};
		pcrPushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pcrPushConstant.offset = 0;
		pcrPushConstant.size = sizeof( LSN_PHOSPHOR_CONSTANTS );

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
			"layout(push_constant) uniform Push {\n"
			"    float decayR;\n"
			"    float decayG;\n"
			"    float decayB;\n"
			"    float initDecay;\n"
			"} push;\n"
			"layout(binding = 0) uniform sampler2D tInput;\n"
			"layout(binding = 1) uniform sampler2D tPrev;\n"
			"layout(location = 0) in vec2 inTex;\n"
			"layout(location = 0) out vec4 outColor;\n"
			"void main() {\n"
			"    vec4 cur = texture(tInput, inTex);\n"
			"    vec4 prev = texture(tPrev, inTex);\n"
			"    vec4 decayed = prev * vec4(push.decayR, push.decayG, push.decayB, 1.0);\n"
			"    outColor = max(cur, decayed);\n"
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
		viadAttributes[0].location = 0;		// Position.
		viadAttributes[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		viadAttributes[0].offset = offsetof( LSN_XYZRHWTEX1, fX );

		viadAttributes[1].binding = 0;
		viadAttributes[1].location = 1;		// TexCoord.
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
		gpciPipelineInfo.renderPass = m_rpRenderPass.rpRenderPass;
		gpciPipelineInfo.subpass = 0;

		m_ppShader = std::make_unique<CVulkanPipeline>();
		if ( !m_ppShader->CreateGraphicsPipeline( _pvkDevice->GetDevice(), &gpciPipelineInfo ) ) { return false; }

		return true;
	}

	/**
	 * Renders the phosphor decay pass.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _pcbCommandList The command buffer in which to record draw commands.
	 * \param _ivInputTexture The incoming image view to process.
	 * \param _sInputSampler The sampler to use for the incoming image view.
	 * \param _ui32SrcW The width of the image.
	 * \param _ui32SrcH The height of the image.
	 * \param _fDecayR The red decay scalar.
	 * \param _fDecayG The green decay scalar.
	 * \param _fDecayB The blue decay scalar.
	 * \param _fInitialDecay The initial decay value.
	 * \return Returns true if rendering succeeded.
	 **/
	bool CVulkanPhosphor::RenderPhosphor( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkImageView _ivInputTexture, VkSampler _sInputSampler, uint32_t _ui32SrcW, uint32_t _ui32SrcH, float _fDecayR, float _fDecayG, float _fDecayB, float _fInitialDecay ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pcbCommandList || !_ivInputTexture || !_sInputSampler || !m_ppShader.get() ) { return false; }


		VkDescriptorImageInfo diiInput = { _sInputSampler, _ivInputTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkDescriptorImageInfo diiPrev = { m_sSampler.sSampler, m_ivPhosphorView[m_stRenderTargetIdx ^ 1].ivImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		VkWriteDescriptorSet wdsWrites[2] = {};
		wdsWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		wdsWrites[0].dstSet = m_dsPhosphor[m_stRenderTargetIdx].dsDescriptorSet;
		wdsWrites[0].dstBinding = 0;
		wdsWrites[0].dstArrayElement = 0;
		wdsWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		wdsWrites[0].descriptorCount = 1;
		wdsWrites[0].pImageInfo = &diiInput;

		wdsWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		wdsWrites[1].dstSet = m_dsPhosphor[m_stRenderTargetIdx].dsDescriptorSet;
		wdsWrites[1].dstBinding = 1;
		wdsWrites[1].dstArrayElement = 0;
		wdsWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		wdsWrites[1].descriptorCount = 1;
		wdsWrites[1].pImageInfo = &diiPrev;

		CVulkan::m_pfUpdateDescriptorSets( _pvkDevice->GetDevice(), 2, wdsWrites, 0, nullptr );


		VkRenderPassBeginInfo rpbiInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		rpbiInfo.renderPass = m_rpRenderPass.rpRenderPass;
		rpbiInfo.framebuffer = m_fbPhosphor[m_stRenderTargetIdx].fbFramebuffer;
		rpbiInfo.renderArea.offset = { 0, 0 };
		rpbiInfo.renderArea.extent = { _ui32SrcW, _ui32SrcH };
		
		CVulkan::m_pfCmdBeginRenderPass( _pcbCommandList->Get(), &rpbiInfo, VK_SUBPASS_CONTENTS_INLINE );


		VkViewport vViewport = {};
		vViewport.x = 0.0f;
		vViewport.y = 0.0f;
		vViewport.width = static_cast<float>(_ui32SrcW);
		vViewport.height = static_cast<float>(_ui32SrcH);
		vViewport.minDepth = 0.0f;
		vViewport.maxDepth = 1.0f;
		CVulkan::m_pfCmdSetViewport( _pcbCommandList->Get(), 0, 1, &vViewport );

		VkRect2D rScissor = {};
		rScissor.offset = { 0, 0 };
		rScissor.extent = { _ui32SrcW, _ui32SrcH };
		CVulkan::m_pfCmdSetScissor( _pcbCommandList->Get(), 0, 1, &rScissor );


		CVulkan::m_pfCmdBindPipeline( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_ppShader->Get() );

		VkBuffer buffers[] = { m_pbVbQuad->Get() };
		VkDeviceSize offsets[] = { 0 };
		CVulkan::m_pfCmdBindVertexBuffers( _pcbCommandList->Get(), 0, 1, buffers, offsets );


		CVulkan::m_pfCmdBindDescriptorSets( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pplPipelineLayout->Get(), 0, 1, &m_dsPhosphor[m_stRenderTargetIdx].dsDescriptorSet, 0, nullptr );


		LSN_PHOSPHOR_CONSTANTS pcConsts = { _fDecayR, _fDecayG, _fDecayB, _fInitialDecay };
		CVulkan::m_pfCmdPushConstants( _pcbCommandList->Get(), m_pplPipelineLayout->Get(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof( LSN_PHOSPHOR_CONSTANTS ), &pcConsts );


		CVulkan::m_pfCmdDraw( _pcbCommandList->Get(), 4, 1, 0, 0 );

		CVulkan::m_pfCmdEndRenderPass( _pcbCommandList->Get() );


		m_stRenderTargetIdx ^= 1;

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
