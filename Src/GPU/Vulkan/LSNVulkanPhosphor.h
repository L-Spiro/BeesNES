/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 hardware wrapper for a phosphor-decay post-processing effect.
 */

#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"
#include "LSNVulkanImage.h"
#include "LSNVulkanDeviceMemory.h"
#include "LSNVulkanBuffer.h"
#include "LSNVulkanPipeline.h"
#include "LSNVulkanPipelineLayout.h"
#include "LSNVulkanDescriptorSetLayout.h"
#include "LSNVulkanCommandBuffer.h"
#include "LSNVulkanDescriptorPool.h"
#include "LSNVulkanDescriptorSet.h"

#include <Helpers/LSWHelpers.h>
#include <memory>
#include <vector>

namespace lsn {

	/**
	 * Class CVulkanPhosphor
	 * \brief A Vulkan 1.0 hardware wrapper for a phosphor-decay post-processing effect.
	 *
	 * Description: Uses an internal ping-pong buffer to track phosphor decay over time, applying independent
	 * decay rates to the red, green, and blue channels, along with an initial decay scalar.
	 */
	class CVulkanPhosphor {
	public :
		CVulkanPhosphor();
		virtual ~CVulkanPhosphor();


		// == Functions.
		/**
		 * Resets the resources and internal states.
		 **/
		void													Reset();

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
		bool													EnsureResources( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, uint32_t _ui32SrcW, uint32_t _ui32SrcH, VkFormat _fTargetFormat );

		/**
		 * Ensures the Pipeline State Objects and Descriptor Set Layouts are created.
		 * 
		 * \param _pvkDevice The Vulkan device.
		 * \param _fTargetFormat The format of the RTV this shader will output to.
		 * \param _vSpirvVert The SPIR-V byte code for the vertex shader.
		 * \param _vSpirvFrag The SPIR-V byte code for the fragment shader.
		 * \return Returns true if the shaders are ready.
		 **/
		bool													EnsureShaders( CVulkanDevice * _pvkDevice, VkFormat _fTargetFormat, const std::vector<uint32_t> &_vSpirvVert, const std::vector<uint32_t> &_vSpirvFrag );

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
		bool													RenderPhosphor( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkImageView _ivInputTexture, VkSampler _sInputSampler, uint32_t _ui32SrcW, uint32_t _ui32SrcH, float _fDecayR, float _fDecayG, float _fDecayB, float _fInitialDecay );

		/**
		 * Gets the current output target of the phosphor pass.
		 * 
		 * \return Returns the view of the target that was just rendered to, so the next pass can read from it.
		 **/
		inline VkImageView										GetCurrentPhosphorView() const { return m_ivPhosphorView[m_stRenderTargetIdx ^ 1].ivImageView; }


	protected :
		// == Types.
#pragma pack( push, 1 )
		struct LSN_XYZRHWTEX1 {
			float fX, fY, fZ, fW;
			float fU, fV;
		};

		struct LSN_PHOSPHOR_CONSTANTS {
			float fDecayR;
			float fDecayG;
			float fDecayB;
			float fInitialDecay;
		};
#pragma pack( pop )


		// == Members.
		std::unique_ptr<CVulkanImage>							m_piPhosphor[2];
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmPhosphorMemory[2];
		CVulkan::LSN_IMAGE_VIEW									m_ivPhosphorView[2];
		CVulkan::LSN_FRAMEBUFFER								m_fbPhosphor[2];
		
		std::unique_ptr<CVulkanBuffer>							m_pbVbQuad;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmVbQuadMemory;

		std::unique_ptr<CVulkanPipeline>						m_ppShader;
		std::unique_ptr<CVulkanPipelineLayout>					m_pplPipelineLayout;
		std::unique_ptr<CVulkanDescriptorSetLayout>				m_pdslDescriptorSetLayout;
		std::unique_ptr<CVulkanDescriptorPool>					m_pdpDescriptorPool;
		CVulkan::LSN_DESCRIPTOR_SET								m_dsPhosphor[2];

		CVulkan::LSN_RENDER_PASS								m_rpRenderPass;
		CVulkan::LSN_SAMPLER									m_sSampler;

		size_t													m_stRenderTargetIdx = 0;
		uint32_t												m_ui32SrcW = 0;
		uint32_t												m_ui32SrcH = 0;
		VkFormat												m_fFormat = VK_FORMAT_UNDEFINED;


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
