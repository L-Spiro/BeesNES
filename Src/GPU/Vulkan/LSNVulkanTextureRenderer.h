/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for rendering a texture to a surface with bilinear sampling and gamma correction.
 */

#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"
#include "LSNVulkanBuffer.h"
#include "LSNVulkanDeviceMemory.h"
#include "LSNVulkanPipeline.h"
#include "LSNVulkanPipelineLayout.h"
#include "LSNVulkanDescriptorSetLayout.h"
#include "LSNVulkanCommandBuffer.h"

#include <Helpers/LSWHelpers.h>
#include <memory>
#include <vector>

namespace lsn {

	/**
	 * Class CVulkanTextureRenderer
	 * \brief A generic helper class for rendering a texture to a surface.
	 *
	 * Description: Accepts an input texture and an output render target and simply renders the result from the input to the output using bilinear sampling and a basic copy/gamma shader.
	 */
	class CVulkanTextureRenderer {
	public :
		CVulkanTextureRenderer();
		~CVulkanTextureRenderer();


		// == Functions.
		/**
		 * Resets the vertex buffer, shaders, and internal states.
		 **/
		void													Reset();

		/**
		 * Ensures the target resources (like the vertex buffer quad) are created.
		 *
		 * \param _pvkDevice The Vulkan device.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CVulkanDevice * _pvkDevice );

		/**
		 * Ensures the Pipeline State Object and Descriptor Set Layouts are created for the specified format.
		 *
		 * \param _pvkDevice The Vulkan device.
		 * \param _rpRenderPass The render pass that the pipeline will execute within.
		 * \param _fFormat The target format for the pipeline.
		 * \param _vSpirvVert The SPIR-V byte code for the vertex shader.
		 * \param _vSpirvFrag The SPIR-V byte code for the fragment shader.
		 * \return Returns true if the shaders are ready.
		 **/
		bool													EnsureShaders( CVulkanDevice * _pvkDevice, VkRenderPass _rpRenderPass, VkFormat _fFormat, const std::vector<uint32_t> &_vSpirvVert, const std::vector<uint32_t> &_vSpirvFrag );

		/**
		 * Renders the texture to the surface.
		 *
		 * \param _pvkDevice The Vulkan device.
		 * \param _pcbCommandList The command buffer in which to record draw commands.
		 * \param _dsSourceTexture The descriptor set bounding the source image and sampler.
		 * \param _rOutput The destination rectangle in client pixels.
		 * \return Returns true if rendering commands were successfully recorded.
		 **/
		bool													Render( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkDescriptorSet _dsSourceTexture, const lsw::LSW_RECT &_rOutput );


	protected :
		// == Types.
#pragma pack( push, 1 )
		struct LSN_XYZRHWTEX1 {
			float fX, fY, fZ, fW;
			float fU, fV;
		};
#pragma pack( pop )


		// == Members.
		std::unique_ptr<CVulkanBuffer>							m_pbVbQuad;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmVbQuadMemory;

		std::unique_ptr<CVulkanPipeline>						m_ppShader;
		std::unique_ptr<CVulkanPipelineLayout>					m_pplPipelineLayout;
		std::unique_ptr<CVulkanDescriptorSetLayout>				m_pdslDescriptorSetLayout;


		// == Functions.
		/**
		 * Creates a Vulkan shader module from SPIR-V code.
		 *
		 * \param _pvkDevice The Vulkan device.
		 * \param _vSpirv The compiled SPIR-V code.
		 * \param _smModule The shader module wrapper to populate.
		 * \return Returns true on success.
		 **/
		bool													LoadSpirv( CVulkanDevice * _pvkDevice, const std::vector<uint32_t> &_vSpirv, CVulkan::LSN_SHADER_MODULE &_smModule );

		/**
		 * Helper to find an appropriate memory type index for allocations.
		 *
		 * \param _pdDevice The physical device.
		 * \param _ui32TypeFilter A bitmask specifying the acceptable memory types.
		 * \param _mpfProperties The required memory properties.
		 * \return Returns the index of the memory type, or 0 if none is found.
		 **/
		uint32_t												FindMemoryType( VkPhysicalDevice _pdDevice, uint32_t _ui32TypeFilter, VkMemoryPropertyFlags _mpfProperties );
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
