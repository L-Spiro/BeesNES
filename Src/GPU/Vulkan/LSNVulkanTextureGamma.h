/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for applying a gamma curve to a texture.
 */

#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "../../Filters/LSNNesPalette.h"
#include "LSNVulkanDevice.h"
#include "LSNVulkanImage.h"
#include "LSNVulkanDeviceMemory.h"
#include "LSNVulkanBuffer.h"
#include "LSNVulkanPipeline.h"
#include "LSNVulkanPipelineLayout.h"
#include "LSNVulkanDescriptorSetLayout.h"
#include "LSNVulkanCommandBuffer.h"

#include <Helpers/LSWHelpers.h>
#include <memory>
#include <vector>

namespace lsn {

	/**
	 * Class CVulkanTextureGamma
	 * \brief A generic helper class for applying a gamma curve to a texture.
	 *
	 * Description: Performs a 1:1 texture fetch and applies a specified gamma curve, outputting to a render target.
	 */
	class CVulkanTextureGamma {
	public :
		CVulkanTextureGamma();
		~CVulkanTextureGamma();


		// == Functions.
		/**
		 * Resets the resources and internal states.
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
		 * Ensures the Pipeline State Object is compiled.
		 * 
		 * \param _pvkDevice The Vulkan device.
		 * \param _rpRenderPass The render pass that the pipeline will execute within.
		 * \param _fFormat The target format for the pipeline.
		 * \param _vSpirvVert The SPIR-V byte code for the vertex shader.
		 * \param _vSpirvFrag The SPIR-V byte code for the fragment shader.
		 * \return Returns true if the shader is ready.
		 **/
		bool													EnsureShaders( CVulkanDevice * _pvkDevice, VkRenderPass _rpRenderPass, VkFormat _fFormat, const std::vector<uint32_t> &_vSpirvVert, const std::vector<uint32_t> &_vSpirvFrag );

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
		bool													Render( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkDescriptorSet _dsSourceTexture, uint32_t _ui32Width, uint32_t _ui32Height, CNesPalette::LSN_GAMMA _gGamma );


	protected :
		// == Types.
#pragma pack( push, 1 )
		struct LSN_XYZRHWTEX1 {
			float fX, fY, fZ, fW;
			float fU, fV;
		};

		struct LSN_GAMMA_CONSTANTS {
			uint32_t ui32GammaMode;
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
		 **/
		bool													LoadSpirv( CVulkanDevice * _pvkDevice, const std::vector<uint32_t> &_vSpirv, CVulkan::LSN_SHADER_MODULE &_smModule );

		/**
		 * Helper to find an appropriate memory type index for allocations.
		 **/
		uint32_t												FindMemoryType( VkPhysicalDevice _pdDevice, uint32_t _ui32TypeFilter, VkMemoryPropertyFlags _mpfProperties );
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
