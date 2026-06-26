/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for applying integer nearest-neighbor scaling to a texture.
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

#include <memory>
#include <vector>

namespace lsn {

	/**
	 * Class CVulkanTexturePixelScaler
	 * \brief A generic helper class for applying integer nearest-neighbor scaling to a texture.
	 *
	 * Description: Scales a texture by integer factors using nearest-neighbor sampling.
	 */
	class CVulkanTexturePixelScaler {
	public :
		CVulkanTexturePixelScaler();
		~CVulkanTexturePixelScaler();


		// == Functions.
		/**
		 * Resets the resources and internal states.
		 **/
		void													Reset();

		/**
		 * Ensures the target resources are created and match the requested size.
		 * 
		 * \param _pvkDevice The Vulkan device.
		 * \param _ui32DstW The target width.
		 * \param _ui32DstH The target height.
		 * \param _fFormat The target format.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CVulkanDevice * _pvkDevice, uint32_t _ui32DstW, uint32_t _ui32DstH, VkFormat _fFormat );

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
		bool													EnsureShader( CVulkanDevice * _pvkDevice, VkRenderPass _rpRenderPass, VkFormat _fFormat, const std::vector<uint32_t> &_vSpirvVert, const std::vector<uint32_t> &_vSpirvFrag );

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
		bool													Render( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkDescriptorSet _dsSourceTexture, uint32_t _ui32NativeW, uint32_t _ui32NativeH, const lsw::LSW_RECT &_rOutput, bool _bFlipY );

		/**
		 * Gets the internal target view for the next pass to sample from.
		 *
		 * \return Returns the image view of the scaled target.
		 **/
		inline VkImageView										GetTargetView() const { return m_ivTargetView.ivImageView; }

		/**
		 * Gets the width of the scaled target.
		 *
		 * \return Returns the width of the scaled target in pixels.
		 **/
		inline uint32_t											GetWidth() const { return m_ui32DstW; }

		/**
		 * Gets the height of the scaled target.
		 *
		 * \return Returns the height of the scaled target in pixels.
		 **/
		inline uint32_t											GetHeight() const { return m_ui32DstH; }


	protected :
		// == Types.
#pragma pack( push, 1 )
		struct LSN_XYZRHWTEX1 {
			float fX, fY, fZ, fW;
			float fU, fV;
		};

		struct LSN_SCALER_CONSTANTS {
			uint32_t ui32NativeW;
			uint32_t ui32NativeH;
			float fFlipY;
		};
#pragma pack( pop )

		// == Members.
		std::unique_ptr<CVulkanImage>							m_piTarget;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmTargetMemory;
		CVulkan::LSN_IMAGE_VIEW									m_ivTargetView;

		std::unique_ptr<CVulkanBuffer>							m_pbVbQuad;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmVbQuadMemory;

		std::unique_ptr<CVulkanPipeline>						m_ppShader;
		std::unique_ptr<CVulkanPipelineLayout>					m_pplPipelineLayout;
		std::unique_ptr<CVulkanDescriptorSetLayout>				m_pdslDescriptorSetLayout;

		uint32_t												m_ui32DstW = 0;
		uint32_t												m_ui32DstH = 0;
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
