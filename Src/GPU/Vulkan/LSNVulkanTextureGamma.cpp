/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for applying a gamma curve to a texture.
 */

#ifdef LSN_VULKAN1

#include "LSNVulkanTextureGamma.h"

#include <cstring>

namespace lsn {

	CVulkanTextureGamma::CVulkanTextureGamma() {
	}
	CVulkanTextureGamma::~CVulkanTextureGamma() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the resources and internal states.
	 **/
	void CVulkanTextureGamma::Reset() {
		if LSN_LIKELY( m_pdmVbQuadMemory.get() ) { m_pdmVbQuadMemory->Reset(); }
		if LSN_LIKELY( m_pbVbQuad.get() ) { m_pbVbQuad->Reset(); }

		if LSN_LIKELY( m_ppShader.get() ) { m_ppShader->Reset(); }
		if LSN_LIKELY( m_pplPipelineLayout.get() ) { m_pplPipelineLayout->Reset(); }
		if LSN_LIKELY( m_pdslDescriptorSetLayout.get() ) { m_pdslDescriptorSetLayout->Reset(); }

		m_pbVbQuad.reset();
		m_pdmVbQuadMemory.reset();
		m_ppShader.reset();
		m_pplPipelineLayout.reset();
		m_pdslDescriptorSetLayout.reset();
	}

	/**
	 * Ensures the target resources (like the vertex buffer quad) are created.
	 **/
	bool CVulkanTextureGamma::EnsureResources( CVulkanDevice * _pvkDevice ) {
		if LSN_UNLIKELY( !_pvkDevice ) { return false; }
		if ( m_pbVbQuad.get() && m_pbVbQuad->Get() ) { return true; }

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

		VkMemoryRequirements mrVbMemReq;
		CVulkan::m_pfGetBufferMemoryRequirements( _pvkDevice->GetDevice(), m_pbVbQuad->Get(), &mrVbMemReq );

		VkMemoryAllocateInfo maiVbAlloc = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		maiVbAlloc.allocationSize = mrVbMemReq.size;
		maiVbAlloc.memoryTypeIndex = FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrVbMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

		m_pdmVbQuadMemory = std::make_unique<CVulkanDeviceMemory>();
		if ( !m_pdmVbQuadMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiVbAlloc ) ) { return false; }
		CVulkan::m_pfBindBufferMemory( _pvkDevice->GetDevice(), m_pbVbQuad->Get(), m_pdmVbQuadMemory->Get(), 0 );

		void* pvData = nullptr;
		if ( CVulkan::m_pfMapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get(), 0, sizeof( Quad ), 0, &pvData ) == VK_SUCCESS ) {
			std::memcpy( pvData, Quad, sizeof( Quad ) );
			CVulkan::m_pfUnmapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get() );
		}

		return true;
	}

	/**
	 * Ensures the Pipeline State Object is compiled.
	 **/
	bool CVulkanTextureGamma::EnsureShaders( CVulkanDevice * _pvkDevice, VkRenderPass _rpRenderPass, VkFormat /*_fFormat*/, const std::vector<uint32_t> &_vSpirvVert, const std::vector<uint32_t> &_vSpirvFrag ) {
		if LSN_UNLIKELY( !_pvkDevice || !_rpRenderPass ) { return false; }
		if ( m_ppShader.get() && m_ppShader->Get() ) { return true; }

		// 1. Create Descriptor Set Layout (Binding 0: Image/Sampler)
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

		// 2. Create Pipeline Layout with Push Constants for Gamma Enum
		VkPushConstantRange pcrPushConstant = {};
		pcrPushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pcrPushConstant.offset = 0;
		pcrPushConstant.size = sizeof( LSN_GAMMA_CONSTANTS );

		VkDescriptorSetLayout layouts[] = { m_pdslDescriptorSetLayout->Get() };
		VkPipelineLayoutCreateInfo plciLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		plciLayoutInfo.setLayoutCount = 1;
		plciLayoutInfo.pSetLayouts = layouts;
		plciLayoutInfo.pushConstantRangeCount = 1;
		plciLayoutInfo.pPushConstantRanges = &pcrPushConstant;

		m_pplPipelineLayout = std::make_unique<CVulkanPipelineLayout>();
		if ( !m_pplPipelineLayout->CreatePipelineLayout( _pvkDevice->GetDevice(), &plciLayoutInfo ) ) { return false; }

		// 3. Compile Shaders
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

		// 4. Setup Graphics Pipeline
		VkVertexInputBindingDescription vibdBindingDesc = {};
		vibdBindingDesc.binding = 0;
		vibdBindingDesc.stride = sizeof( LSN_XYZRHWTEX1 );
		vibdBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription viadAttributes[2] = {};
		viadAttributes[0].binding = 0;
		viadAttributes[0].location = 0; // Position
		viadAttributes[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		viadAttributes[0].offset = offsetof( LSN_XYZRHWTEX1, fX );

		viadAttributes[1].binding = 0;
		viadAttributes[1].location = 1; // TexCoord
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
	 * Renders the texture to the surface.
	 **/
	bool CVulkanTextureGamma::Render( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkDescriptorSet _dsSourceTexture, uint32_t _ui32Width, uint32_t _ui32Height, CNesPalette::LSN_GAMMA _gGamma ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pcbCommandList || !_dsSourceTexture || !m_ppShader.get() ) { return false; }

		// 1. Set Viewport and Scissor
		VkViewport vViewport = {};
		vViewport.x = 0.0f;
		vViewport.y = 0.0f;
		vViewport.width = static_cast<float>(_ui32Width);
		vViewport.height = static_cast<float>(_ui32Height);
		vViewport.minDepth = 0.0f;
		vViewport.maxDepth = 1.0f;
		CVulkan::m_pfCmdSetViewport( _pcbCommandList->Get(), 0, 1, &vViewport );

		VkRect2D rScissor = {};
		rScissor.offset = { 0, 0 };
		rScissor.extent = { _ui32Width, _ui32Height };
		CVulkan::m_pfCmdSetScissor( _pcbCommandList->Get(), 0, 1, &rScissor );

		// 2. Bind Pipeline
		CVulkan::m_pfCmdBindPipeline( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_ppShader->Get() );

		// 3. Bind Vertex Buffer
		VkBuffer buffers[] = { m_pbVbQuad->Get() };
		VkDeviceSize offsets[] = { 0 };
		CVulkan::m_pfCmdBindVertexBuffers( _pcbCommandList->Get(), 0, 1, buffers, offsets );

		// 4. Bind Descriptor Set
		CVulkan::m_pfCmdBindDescriptorSets( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pplPipelineLayout->Get(), 0, 1, &_dsSourceTexture, 0, nullptr );

		// 5. Push Constants
		LSN_GAMMA_CONSTANTS gcConsts = { static_cast<uint32_t>(_gGamma) };
		CVulkan::m_pfCmdPushConstants( _pcbCommandList->Get(), m_pplPipelineLayout->Get(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof( LSN_GAMMA_CONSTANTS ), &gcConsts );

		// 6. Draw Quad
		CVulkan::m_pfCmdDraw( _pcbCommandList->Get(), 4, 1, 0, 0 );

		return true;
	}

	/**
	 * Creates a Vulkan shader module from SPIR-V code.
	 **/
	bool CVulkanTextureGamma::LoadSpirv( CVulkanDevice * _pvkDevice, const std::vector<uint32_t> &_vSpirv, CVulkan::LSN_SHADER_MODULE &_smModule ) {
		VkShaderModuleCreateInfo smciInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		smciInfo.codeSize = _vSpirv.size() * sizeof( uint32_t );
		smciInfo.pCode = _vSpirv.data();
		return _smModule.Create( _pvkDevice->GetDevice(), &smciInfo );
	}

	/**
	 * Helper to find an appropriate memory type index for allocations.
	 **/
	uint32_t CVulkanTextureGamma::FindMemoryType( VkPhysicalDevice _pdDevice, uint32_t _ui32TypeFilter, VkMemoryPropertyFlags _mpfProperties ) {
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
