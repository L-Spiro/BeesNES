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

		m_gShaderGamma = CNesPalette::LSN_G_NONE;
	}

	/**
	 * Ensures the target resources (like the vertex buffer quad) are created.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \return Returns true if resources are ready.
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
		maiVbAlloc.memoryTypeIndex = CVulkan::FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrVbMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

		m_pdmVbQuadMemory = std::make_unique<CVulkanDeviceMemory>();
		if ( !m_pdmVbQuadMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiVbAlloc ) ) { return false; }
		CVulkan::m_pfBindBufferMemory( _pvkDevice->GetDevice(), m_pbVbQuad->Get(), m_pdmVbQuadMemory->Get(), 0 );

		void * pvData = nullptr;
		if ( CVulkan::m_pfMapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get(), 0, sizeof( Quad ), 0, &pvData ) == VK_SUCCESS ) {
			std::memcpy( pvData, Quad, sizeof( Quad ) );
			CVulkan::m_pfUnmapMemory( _pvkDevice->GetDevice(), m_pdmVbQuadMemory->Get() );
		}

		return true;
	}

	/**
	 * Ensures the Pipeline State Object is compiled.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _rpRenderPass The render pass that the pipeline will execute within.
	 * \param _gGamma The specific gamma curve to hard-code into the generated shader.
	 * \param _fFormat The target format for the pipeline.
	 * \param _vSpirvVert The SPIR-V byte code for the vertex shader.
	 * \param _vSpirvFrag The SPIR-V byte code for the fragment shader.
	 * \return Returns true if the shader is ready.
	 **/
	bool CVulkanTextureGamma::EnsureShaders( CVulkanDevice * _pvkDevice, VkRenderPass _rpRenderPass, CNesPalette::LSN_GAMMA _gGamma, VkFormat /*_fFormat*/, const std::vector<uint32_t> &/*_vSpirvVert*/, const std::vector<uint32_t> &/*_vSpirvFrag*/ ) {
		if LSN_UNLIKELY( !_pvkDevice || !_rpRenderPass ) { return false; }
		if ( m_ppShader.get() && m_ppShader->Get() ) {
			if ( m_gShaderGamma == _gGamma ) { return true; }
			m_ppShader->Reset();
		}
		m_gShaderGamma = _gGamma;

		if ( !m_pdslDescriptorSetLayout.get() ) {
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
		}

		if ( !m_pplPipelineLayout.get() ) {
			VkDescriptorSetLayout layouts[] = { m_pdslDescriptorSetLayout->Get() };
			VkPipelineLayoutCreateInfo plciLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
			plciLayoutInfo.setLayoutCount = 1;
			plciLayoutInfo.pSetLayouts = layouts;
			plciLayoutInfo.pushConstantRangeCount = 0;
			plciLayoutInfo.pPushConstantRanges = nullptr;

			m_pplPipelineLayout = std::make_unique<CVulkanPipelineLayout>();
			if ( !m_pplPipelineLayout->CreatePipelineLayout( _pvkDevice->GetDevice(), &plciLayoutInfo ) ) { return false; }
		}

		std::string sGammaCall = "c.rgb";
		switch ( _gGamma ) {
			case CNesPalette::LSN_G_CRT1 :			{ sGammaCall = "CrtProperToLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_CRT2 :			{ sGammaCall = "CrtProper2ToLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_sRGB :			{ sGammaCall = "sRGBtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_SMPTE170M :		{ sGammaCall = "SMPTE170MtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_DCIP3 :			{ sGammaCall = "DCIP3toLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_ADOBERGB :		{ sGammaCall = "AdobeRGBtoLinear3( c.rgb )"; break; }
			case CNesPalette::LSN_G_SMPTE240M :		{ sGammaCall = "SMPTE240MtoLinear3_Precise( c.rgb )"; break; }
			case CNesPalette::LSN_G_POW_1_96 :		{ sGammaCall = "pow( c.rgb, vec3( 1.96 ) )"; break; }
			case CNesPalette::LSN_G_POW_2_0 :		{ sGammaCall = "pow( c.rgb, vec3( 2.0 ) )"; break; }
			case CNesPalette::LSN_G_POW_2_2 :		{ sGammaCall = "pow( c.rgb, vec3( 2.2222220897674560546875 ) )"; break; }
			case CNesPalette::LSN_G_POW_2_35 :		{ sGammaCall = "pow( c.rgb, vec3( 2.35 ) )"; break; }
			case CNesPalette::LSN_G_POW_2_4 :		{ sGammaCall = "pow( c.rgb, vec3( 2.4 ) )"; break; }
			case CNesPalette::LSN_G_POW_2_5 :		{ sGammaCall = "pow( c.rgb, vec3( 2.5 ) )"; break; }
			case CNesPalette::LSN_G_POW_2_7 :		{ sGammaCall = "pow( c.rgb, vec3( 2.7 ) )"; break; }
			case CNesPalette::LSN_G_POW_2_8 :		{ sGammaCall = "pow( c.rgb, vec3( 2.8 ) )"; break; }
			default : { break; }
		}

		static const char * kVsGlsl =
			"#version 450\n"
			"layout(location = 0) in vec4 inPos;\n"
			"layout(location = 1) in vec2 inTex;\n"
			"layout(location = 0) out vec2 outTex;\n"
			"void main() {\n"
			"    gl_Position = vec4(inPos.x, -inPos.y, inPos.z, inPos.w);\n"
			"    outTex = inTex;\n"
			"}\n";

		// Embed the gamma functions directly to bypass file resolution requirements.
		static const char * kPsGammaFuncs =
			"float sRGBtoLinear_Precise(float v) {\n"
			"    const float t = 0.0392857142857142918601631720;\n"
			"    const float im = 1.0 / 12.9232101807878549948327417951;\n"
			"    return (v <= t) ? (v * im) : pow((v + 0.055) * (1.0 / 1.055), 2.4);\n"
			"}\n"
			"vec3 sRGBtoLinear3_Precise(vec3 c) { return vec3(sRGBtoLinear_Precise(c.r), sRGBtoLinear_Precise(c.g), sRGBtoLinear_Precise(c.b)); }\n"
			"\n"
			"float SMPTE170MtoLinear_Precise(float v) {\n"
			"    const float t = 0.0812428582986351593975271612180;\n"
			"    const float a = 1.09929682680944297568093048767;\n"
			"    const float ao = 0.09929682680944297568093048767;\n"
			"    return (v <= t) ? (v / 4.5) : pow((v + ao) / a, 1.0 / 0.45);\n"
			"}\n"
			"vec3 SMPTE170MtoLinear3_Precise(vec3 c) { return vec3(SMPTE170MtoLinear_Precise(c.r), SMPTE170MtoLinear_Precise(c.g), SMPTE170MtoLinear_Precise(c.b)); }\n"
			"\n"
			"float DCIP3toLinear(float v) { return pow(v, 2.6); }\n"
			"vec3 DCIP3toLinear3(vec3 c) { return vec3(DCIP3toLinear(c.r), DCIP3toLinear(c.g), DCIP3toLinear(c.b)); }\n"
			"\n"
			"float AdobeRGBtoLinear(float v) { return pow(v, 2.19921875); }\n"
			"vec3 AdobeRGBtoLinear3(vec3 c) { return vec3(AdobeRGBtoLinear(c.r), AdobeRGBtoLinear(c.g), AdobeRGBtoLinear(c.b)); }\n"
			"\n"
			"float SMPTE240MtoLinear_Precise(float v) {\n"
			"    const float t = 0.0912863421177801115380390228893;\n"
			"    const float a = 1.1115721959217312597711924126997;\n"
			"    const float ao = 0.1115721959217312597711924126997;\n"
			"    return (v < t) ? (v / 4.0) : pow((v + ao) / a, 1.0 / 0.45);\n"
			"}\n"
			"vec3 SMPTE240MtoLinear3_Precise(vec3 c) { return vec3(SMPTE240MtoLinear_Precise(c.r), SMPTE240MtoLinear_Precise(c.g), SMPTE240MtoLinear_Precise(c.b)); }\n"
			"\n"
			"float CrtProperToLinear(float v, float Lw, float B) {\n"
			"    const float Alpha1 = 2.6;\n"
			"    const float Alpha2 = 3.0;\n"
			"    const float Vc = 0.35;\n"
			"    float K = Lw / pow(1.0 + B, Alpha1);\n"
			"    if (v < Vc) { return K * pow(Vc + B, (Alpha1 - Alpha2)) * pow(v + B, Alpha2); }\n"
			"    return K * pow(v + B, Alpha1);\n"
			"}\n"
			"float CrtProperToLinear(float v) { return CrtProperToLinear(v, 1.0, 0.0181); }\n"
			"vec3 CrtProperToLinear3(vec3 c) { return vec3(CrtProperToLinear(c.r), CrtProperToLinear(c.g), CrtProperToLinear(c.b)); }\n"
			"\n"
			"float CrtProper2ToLinear(float v) {\n"
			"    const float Alpha = 0.1115721959217312597711924126997473649680614471435546875;\n"
			"    const float Beta = 1.1115721959217312875267680283286608755588531494140625;\n"
			"    const float Cut = 0.0912863421177801115380390228892792947590351104736328125;\n"
			"    if (v >= 0.36) { return pow(v, 2.31); }\n"
			"    float frac = v / 0.36;\n"
			"    float a = (v <= Cut) ? (v / 4.0) : pow((v + Alpha) / Beta, 1.0 / 0.45);\n"
			"    float b = pow(v, 2.31);\n"
			"    return a * (1.0 - frac) + b * frac;\n"
			"}\n"
			"vec3 CrtProper2ToLinear3(vec3 c) { return vec3(CrtProper2ToLinear(c.r), CrtProper2ToLinear(c.g), CrtProper2ToLinear(c.b)); }\n";

		std::string sPsGlsl =
			"#version 450\n"
			+ std::string(kPsGammaFuncs) +
			"layout(binding = 0) uniform sampler2D tTex;\n"
			"layout(location = 0) in vec2 inTex;\n"
			"layout(location = 0) out vec4 outColor;\n"
			"void main() {\n"
			"    vec4 c = texture(tTex, inTex);\n"
			"    c.rgb = " + sGammaCall + ";\n"
			"    outColor = c;\n"
			"}\n";

		std::vector<uint32_t> vVert, vFrag;
		if ( !CVulkan::CompileGlslToSpirv( kVsGlsl, "vertex", vVert ) || !CVulkan::CompileGlslToSpirv( sPsGlsl.c_str(), "fragment", vFrag ) ) { return false; }

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
		gpciPipelineInfo.renderPass = _rpRenderPass;
		gpciPipelineInfo.subpass = 0;

		if ( !m_ppShader.get() ) { m_ppShader = std::make_unique<CVulkanPipeline>(); }
		if ( !m_ppShader->CreateGraphicsPipeline( _pvkDevice->GetDevice(), &gpciPipelineInfo ) ) { return false; }

		return true;
	}

	/**
	 * Renders the texture to the surface.
	 * 
	 * \param _pvkDevice The Vulkan device.
	 * \param _pcbCommandList The command buffer in which to record draw commands.
	 * \param _dsSourceTexture The descriptor set bounding the source image and sampler.
	 * \param _ui32Width The target width.
	 * \param _ui32Height The target height.
	 * \param _gGamma The gamma curve to apply.
	 * \return Returns true if rendering commands were successfully recorded.
	 **/
	bool CVulkanTextureGamma::Render( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkDescriptorSet _dsSourceTexture, uint32_t _ui32Width, uint32_t _ui32Height, CNesPalette::LSN_GAMMA /*_gGamma*/ ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pcbCommandList || !_dsSourceTexture || !m_ppShader.get() ) { return false; }


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


		CVulkan::m_pfCmdBindPipeline( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_ppShader->Get() );


		VkBuffer buffers[] = { m_pbVbQuad->Get() };
		VkDeviceSize offsets[] = { 0 };
		CVulkan::m_pfCmdBindVertexBuffers( _pcbCommandList->Get(), 0, 1, buffers, offsets );


		CVulkan::m_pfCmdBindDescriptorSets( _pcbCommandList->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pplPipelineLayout->Get(), 0, 1, &_dsSourceTexture, 0, nullptr );

		CVulkan::m_pfCmdDraw( _pcbCommandList->Get(), 4, 1, 0, 0 );

		return true;
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
