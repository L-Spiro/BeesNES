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

		m_fbOutput.Reset();
		m_ivOutputView.Reset();
		if LSN_LIKELY( m_pdmOutputMemory.get() ) { m_pdmOutputMemory->Reset(); }
		if LSN_LIKELY( m_piOutput.get() ) { m_piOutput->Reset(); }
		m_piOutput.reset();
		m_pdmOutputMemory.reset();

		if LSN_LIKELY( m_pdmVbQuadMemory.get() ) { m_pdmVbQuadMemory->Reset(); }
		if LSN_LIKELY( m_pbVbQuad.get() ) { m_pbVbQuad->Reset(); }
		m_pbVbQuad.reset();
		m_pdmVbQuadMemory.reset();

		m_rpRenderPass.Reset();

		if LSN_LIKELY( m_ppShaderOutput.get() ) { m_ppShaderOutput->Reset(); }
		m_ppShaderOutput.reset();
		if LSN_LIKELY( m_ppShaderUpdate.get() ) { m_ppShaderUpdate->Reset(); }
		m_ppShaderUpdate.reset();

		if LSN_LIKELY( m_pplPipelineLayout.get() ) { m_pplPipelineLayout->Reset(); }
		m_pplPipelineLayout.reset();

		if LSN_LIKELY( m_pdslDescriptorSetLayout.get() ) { m_pdslDescriptorSetLayout->Reset(); }
		m_pdslDescriptorSetLayout.reset();

		if LSN_LIKELY( m_pdpDescriptorPool.get() ) { m_pdpDescriptorPool->Reset(); }
		m_pdpDescriptorPool.reset();

		m_stRenderTargetIdx = 0;
		m_ui32SrcW = 0;
		m_ui32SrcH = 0;
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
	bool CVulkanPhosphor::EnsureResources( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, uint32_t _ui32SrcW, uint32_t _ui32SrcH, VkFormat _fFormat ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pcbCommandList ) { return false; }
		if ( m_piPhosphor[0].get() && m_ui32SrcW == _ui32SrcW && m_ui32SrcH == _ui32SrcH ) { return true; }

		Reset();

		VkAttachmentDescription adPass = {};
		adPass.format = _fFormat;
		adPass.samples = VK_SAMPLE_COUNT_1_BIT;
		adPass.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		adPass.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		adPass.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		adPass.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		adPass.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		adPass.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference arRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		VkSubpassDescription sdSubpass = {};
		sdSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		sdSubpass.colorAttachmentCount = 1;
		sdSubpass.pColorAttachments = &arRef;

		VkRenderPassCreateInfo rpciInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		rpciInfo.attachmentCount = 1;
		rpciInfo.pAttachments = &adPass;
		rpciInfo.subpassCount = 1;
		rpciInfo.pSubpasses = &sdSubpass;
		if ( !m_rpRenderPass.Create( _pvkDevice->GetDevice(), &rpciInfo ) ) { return false; }


		auto CreateTexture = [&]( std::unique_ptr<CVulkanImage> &_piTarget, std::unique_ptr<CVulkanDeviceMemory> &_pdmMemory, CVulkan::LSN_IMAGE_VIEW &_ivView, CVulkan::LSN_FRAMEBUFFER &_fbTarget ) {
			_piTarget = std::make_unique<CVulkanImage>();
			VkImageCreateInfo iciInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			iciInfo.imageType = VK_IMAGE_TYPE_2D;
			iciInfo.extent = { _ui32SrcW, _ui32SrcH, 1 };
			iciInfo.mipLevels = 1;
			iciInfo.arrayLayers = 1;
			iciInfo.format = _fFormat;
			iciInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			iciInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			iciInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			iciInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			_piTarget->CreateImage( _pvkDevice->GetDevice(), &iciInfo );

			VkMemoryRequirements mrReq;
			CVulkan::m_pfGetImageMemoryRequirements( _pvkDevice->GetDevice(), _piTarget->Get(), &mrReq );

			VkMemoryAllocateInfo maiAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			maiAlloc.allocationSize = mrReq.size;
			maiAlloc.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

			_pdmMemory = std::make_unique<CVulkanDeviceMemory>();
			_pdmMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiAlloc );
			CVulkan::m_pfBindImageMemory( _pvkDevice->GetDevice(), _piTarget->Get(), _pdmMemory->Get(), 0 );

			VkImageViewCreateInfo ivciView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			ivciView.image = _piTarget->Get();
			ivciView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ivciView.format = _fFormat;
			ivciView.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			_ivView.Create( _pvkDevice->GetDevice(), &ivciView );

			VkFramebufferCreateInfo fbciFrame = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			fbciFrame.renderPass = m_rpRenderPass.rpRenderPass;
			fbciFrame.attachmentCount = 1;
			fbciFrame.pAttachments = &_ivView.ivImageView;
			fbciFrame.width = _ui32SrcW;
			fbciFrame.height = _ui32SrcH;
			fbciFrame.layers = 1;
			_fbTarget.Create( _pvkDevice->GetDevice(), &fbciFrame );
		};

		CreateTexture( m_piOutput, m_pdmOutputMemory, m_ivOutputView, m_fbOutput );
		CreateTexture( m_piPhosphor[0], m_pdmPhosphorMemory[0], m_ivPhosphorView[0], m_fbPhosphor[0] );
		CreateTexture( m_piPhosphor[1], m_pdmPhosphorMemory[1], m_ivPhosphorView[1], m_fbPhosphor[1] );

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

		VkMemoryRequirements mrReq;
		CVulkan::m_pfGetBufferMemoryRequirements( _pvkDevice->GetDevice(), m_pbVbQuad->Get(), &mrReq );
		VkMemoryAllocateInfo maiAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
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

		m_ui32SrcW = _ui32SrcW;
		m_ui32SrcH = _ui32SrcH;
		m_stRenderTargetIdx = 0;

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
		if ( m_ppShaderOutput.get() && m_ppShaderOutput->Get() && m_ppShaderUpdate.get() && m_ppShaderUpdate->Get() ) { return true; }

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
		m_dsPhosphor[0].dsDescriptorSet = allocatedSets[0];
		m_dsPhosphor[1].dsDescriptorSet = allocatedSets[1];

		VkPushConstantRange pcrPushConstant = {};
		pcrPushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pcrPushConstant.offset = 0;
		pcrPushConstant.size = 16; // { float decayR, float decayG, float decayB, float initDecay }

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

		static const char * kPsOutputGlsl =
			"#version 450\n"
			"layout(push_constant) uniform Push {\n"
			"    float decayR;\n"
			"    float decayG;\n"
			"    float decayB;\n"
			"    float initDecay;\n"
			"} push;\n"
			"layout(binding = 0) uniform sampler2D tSrc;\n"
			"layout(binding = 1) uniform sampler2D tHistory;\n"
			"layout(location = 0) in vec2 inTex;\n"
			"layout(location = 0) out vec4 outColor;\n"
			"void main() {\n"
			"    vec4 curColor = texture(tSrc, inTex);\n"
			"    vec4 oldColor = texture(tHistory, inTex);\n"
			"    vec3 oldDecayed = oldColor.rgb * vec3(push.decayR, push.decayG, push.decayB);\n"
			"    outColor = vec4(max(oldDecayed, curColor.rgb), curColor.a);\n"
			"}\n";

		static const char * kPsUpdateGlsl =
			"#version 450\n"
			"layout(push_constant) uniform Push {\n"
			"    float decayR;\n"
			"    float decayG;\n"
			"    float decayB;\n"
			"    float initDecay;\n"
			"} push;\n"
			"layout(binding = 0) uniform sampler2D tSrc;\n"
			"layout(binding = 1) uniform sampler2D tHistory;\n"
			"layout(location = 0) in vec2 inTex;\n"
			"layout(location = 0) out vec4 outColor;\n"
			"void main() {\n"
			"    vec4 curColor = texture(tSrc, inTex);\n"
			"    vec4 oldColor = texture(tHistory, inTex);\n"
			"    vec3 oldDecayed = oldColor.rgb * vec3(push.decayR, push.decayG, push.decayB);\n"
			"    vec3 scaledCurrent = curColor.rgb * push.initDecay;\n"
			"    outColor = vec4(max(scaledCurrent, oldDecayed), curColor.a);\n"
			"}\n";

		std::vector<uint32_t> vVert, vFragOutput, vFragUpdate;
		if ( !CVulkan::CompileGlslToSpirv( kVsGlsl, "vertex", vVert ) || 
			 !CVulkan::CompileGlslToSpirv( kPsOutputGlsl, "fragment", vFragOutput ) ||
			 !CVulkan::CompileGlslToSpirv( kPsUpdateGlsl, "fragment", vFragUpdate ) ) { return false; }

		CVulkan::LSN_SHADER_MODULE smVert, smFragOutput, smFragUpdate;
		if ( !CVulkan::LoadSpirv( _pvkDevice, vVert, smVert ) || 
			 !CVulkan::LoadSpirv( _pvkDevice, vFragOutput, smFragOutput ) ||
			 !CVulkan::LoadSpirv( _pvkDevice, vFragUpdate, smFragUpdate ) ) { return false; }


		VkPipelineShaderStageCreateInfo pssciShaderStages[2] = {};
		pssciShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pssciShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		pssciShaderStages[0].module = smVert.smShaderModule;
		pssciShaderStages[0].pName = "main";

		pssciShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pssciShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		pssciShaderStages[1].module = smFragOutput.smShaderModule;
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
		gpciPipelineInfo.renderPass = m_rpRenderPass.rpRenderPass;
		gpciPipelineInfo.subpass = 0;

		m_ppShaderOutput = std::make_unique<CVulkanPipeline>();
		if ( !m_ppShaderOutput->CreateGraphicsPipeline( _pvkDevice->GetDevice(), &gpciPipelineInfo ) ) { return false; }

		pssciShaderStages[1].module = smFragUpdate.smShaderModule;
		m_ppShaderUpdate = std::make_unique<CVulkanPipeline>();
		if ( !m_ppShaderUpdate->CreateGraphicsPipeline( _pvkDevice->GetDevice(), &gpciPipelineInfo ) ) { return false; }

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
	bool CVulkanPhosphor::RenderPhosphor( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkImageView _ivSource, VkSampler _sSampler, uint32_t _ui32SrcW, uint32_t _ui32SrcH, float _fDecayR, float _fDecayG, float _fDecayB, float _fInitDecay ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pcbCommandList || !_ivSource || !m_ppShaderOutput.get() || !m_ppShaderUpdate.get() ) { return false; }

		VkDescriptorImageInfo diiSrc = { _sSampler, _ivSource, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkDescriptorImageInfo diiHist = { _sSampler, m_ivPhosphorView[m_stRenderTargetIdx].ivImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		VkWriteDescriptorSet wdsWrites[2] = {};
		wdsWrites[0] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, m_dsPhosphor[0].dsDescriptorSet, 0, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &diiSrc, nullptr, nullptr };
		wdsWrites[1] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, m_dsPhosphor[0].dsDescriptorSet, 1, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &diiHist, nullptr, nullptr };
		CVulkan::m_pfUpdateDescriptorSets( _pvkDevice->GetDevice(), 2, wdsWrites, 0, nullptr );

		VkViewport vViewport = { 0.0f, 0.0f, static_cast<float>(_ui32SrcW), static_cast<float>(_ui32SrcH), 0.0f, 1.0f };
		VkRect2D rScissor = { { 0, 0 }, { _ui32SrcW, _ui32SrcH } };

		VkBuffer buffers[] = { m_pbVbQuad->Get() };
		VkDeviceSize offsets[] = { 0 };
		float fPush[4] = { _fDecayR, _fDecayG, _fDecayB, _fInitDecay };

		// =========================================================================
		// PASS 1: Render Visual Composite to m_fbOutput
		// =========================================================================
		VkRenderPassBeginInfo rpbiPass1 = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		rpbiPass1.renderPass = m_rpRenderPass.rpRenderPass;
		rpbiPass1.framebuffer = m_fbOutput.fbFramebuffer;
		rpbiPass1.renderArea.offset = { 0, 0 };
		rpbiPass1.renderArea.extent = { _ui32SrcW, _ui32SrcH };

		CVulkan::m_pfCmdBeginRenderPass( _pcbCommandList->Get(), &rpbiPass1, VK_SUBPASS_CONTENTS_INLINE );
		
		CVulkan::m_pfCmdSetViewport( _pcbCommandList->Get(), 0, 1, &vViewport );
		CVulkan::m_pfCmdSetScissor( _pcbCommandList->Get(), 0, 1, &rScissor );
		CVulkan::m_pfCmdBindPipeline( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_ppShaderOutput->Get() );
		CVulkan::m_pfCmdBindVertexBuffers( _pcbCommandList->Get(), 0, 1, buffers, offsets );
		CVulkan::m_pfCmdBindDescriptorSets( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pplPipelineLayout->Get(), 0, 1, &m_dsPhosphor[0].dsDescriptorSet, 0, nullptr );
		CVulkan::m_pfCmdPushConstants( _pcbCommandList->Get(), m_pplPipelineLayout->Get(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof( fPush ), fPush );
		
		CVulkan::m_pfCmdDraw( _pcbCommandList->Get(), 4, 1, 0, 0 );
		CVulkan::m_pfCmdEndRenderPass( _pcbCommandList->Get() );

		VkImageMemoryBarrier imbOutput = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		imbOutput.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imbOutput.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imbOutput.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imbOutput.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		imbOutput.image = m_piOutput->Get();
		imbOutput.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		CVulkan::m_pfCmdPipelineBarrier( _pcbCommandList->Get(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imbOutput );


		// =========================================================================
		// PASS 2: Update History Buffer with Scaled Current Frame
		// =========================================================================
		size_t stWriteIndex = m_stRenderTargetIdx ^ 1;

		VkRenderPassBeginInfo rpbiPass2 = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		rpbiPass2.renderPass = m_rpRenderPass.rpRenderPass;
		rpbiPass2.framebuffer = m_fbPhosphor[stWriteIndex].fbFramebuffer;
		rpbiPass2.renderArea.offset = { 0, 0 };
		rpbiPass2.renderArea.extent = { _ui32SrcW, _ui32SrcH };

		CVulkan::m_pfCmdBeginRenderPass( _pcbCommandList->Get(), &rpbiPass2, VK_SUBPASS_CONTENTS_INLINE );

		CVulkan::m_pfCmdSetViewport( _pcbCommandList->Get(), 0, 1, &vViewport );
		CVulkan::m_pfCmdSetScissor( _pcbCommandList->Get(), 0, 1, &rScissor );
		CVulkan::m_pfCmdBindPipeline( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_ppShaderUpdate->Get() );
		CVulkan::m_pfCmdBindVertexBuffers( _pcbCommandList->Get(), 0, 1, buffers, offsets );
		CVulkan::m_pfCmdBindDescriptorSets( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pplPipelineLayout->Get(), 0, 1, &m_dsPhosphor[0].dsDescriptorSet, 0, nullptr );
		CVulkan::m_pfCmdPushConstants( _pcbCommandList->Get(), m_pplPipelineLayout->Get(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof( fPush ), fPush );

		CVulkan::m_pfCmdDraw( _pcbCommandList->Get(), 4, 1, 0, 0 );
		CVulkan::m_pfCmdEndRenderPass( _pcbCommandList->Get() );

		VkImageMemoryBarrier imbUpdate = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		imbUpdate.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imbUpdate.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imbUpdate.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imbUpdate.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		imbUpdate.image = m_piPhosphor[stWriteIndex]->Get();
		imbUpdate.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		CVulkan::m_pfCmdPipelineBarrier( _pcbCommandList->Get(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imbUpdate );

		m_stRenderTargetIdx = stWriteIndex;

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
