/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 hardware wrapper for 2-pass GPU resampling.
 */

#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "../../Filters/LSNResamplerBase.h"
#include "LSNVulkanDevice.h"
#include "LSNVulkanImage.h"
#include "LSNVulkanDeviceMemory.h"
#include "LSNVulkanBuffer.h"
#include "LSNVulkanPipeline.h"
#include "LSNVulkanPipelineLayout.h"
#include "LSNVulkanDescriptorSetLayout.h"
#include "LSNVulkanDescriptorPool.h"
#include "LSNVulkanCommandBuffer.h"

#include <Helpers/LSWHelpers.h>
#include <memory>
#include <vector>

namespace lsn {

	/**
	 * Class CVulkanResampler
	 * \brief A Vulkan 1.0 hardware wrapper for 2-pass GPU resampling.
	 *
	 * Description: Uses CResamplerBase to generate kernel weights and source offsets, uploads them to 
	 * look-up textures, and executes a 2-pass compute-like pixel shader operation utilizing raw texel fetches.
	 */
	class CVulkanResampler : public CResamplerBase {
	public :
		CVulkanResampler();
		virtual ~CVulkanResampler();


		// == Functions.
		/**
		 * Resets the resources and internal states.
		 **/
		void													Reset();

		/**
		 * Ensures all render targets, intermediate buffers, and LUTs are created.
		 *
		 * \param _pvkDevice The Vulkan device.
		 * \param _pcbCommandList The command list used to record barriers and copy operations.
		 * \param _ui32SrcW The source width.
		 * \param _ui32SrcH The source height.
		 * \param _ui32DstW The destination width.
		 * \param _ui32DstH The destination height.
		 * \param _fIntermediateFormat The format for the intermediate target.
		 * \param _fTargetFormat The format for the final target.
		 * \return Returns true if resources are ready.
		 **/
		bool													EnsureResources( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, uint32_t _ui32SrcW, uint32_t _ui32SrcH, uint32_t _ui32DstW, uint32_t _ui32DstH, VkFormat _fIntermediateFormat, VkFormat _fTargetFormat );

		/**
		 * Ensures the Pipeline State Objects and Descriptor Set Layouts are created.
		 * 
		 * \param _pvkDevice The Vulkan device.
		 * \param _vSpirvVert The SPIR-V byte code for the vertex shader.
		 * \param _vSpirvFrag The SPIR-V byte code for the fragment shader.
		 * \return Returns true if the shaders are ready.
		 **/
		bool													EnsureShaders( CVulkanDevice * _pvkDevice, const std::vector<uint32_t> &_vSpirvVert, const std::vector<uint32_t> &_vSpirvFrag );

		/**
		 * Executes the 2-pass resampling operation.
		 * 
		 * \param _pvkDevice The Vulkan device.
		 * \param _pcbCommandList The command buffer in which to record draw commands.
		 * \param _ivSource The view of the source image to resample.
		 * \param _sSourceSampler The sampler to use for the source image.
		 * \param _fbTarget The final framebuffer to output to.
		 * \param _rOutput The destination rectangle.
		 * \param _bFlipY Determines whether the final pass should flip the Y axis.
		 * \return Returns true if rendering succeeded.
		 **/
		bool													Render( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, VkImageView _ivSource, VkSampler _sSourceSampler, VkFramebuffer _fbTarget, const lsw::LSW_RECT &_rOutput, bool _bFlipY );

		/**
		 * Sets the current filter to use.
		 * 
		 * \param _ffFilter The filter to use.
		 **/
		inline void												SetFilter( LSN_FILTER_FUNCS _ffFilter ) {
			 if ( _ffFilter != m_ffFilter ) {
				m_ui32SrcW = m_ui32SrcH = 0;
				m_ffFilter = _ffFilter;
			 }
		}

		/**
		 * Gets the current filter.
		 * 
		 * \return Returns the current filter.
		 **/
		inline LSN_FILTER_FUNCS									GetFilter() const { return m_ffFilter; }


	protected :
		// == Types.
#pragma pack( push, 1 )
		struct LSN_XYZRHWTEX1 {
			float fX, fY, fZ, fW;
			float fU, fV;
		};

		struct LSN_RESAMPLER_CONSTANTS {
			uint32_t ui32SrcSize;
			uint32_t ui32DstSize;
			uint32_t ui32MaxTaps;
			float fFlipY;
		};
#pragma pack( pop )


		// == Members.
		std::unique_ptr<CVulkanImage>							m_piIntermediate;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmIntermediateMemory;
		CVulkan::LSN_IMAGE_VIEW									m_ivIntermediateView;
		CVulkan::LSN_FRAMEBUFFER								m_fbIntermediate;

		std::unique_ptr<CVulkanImage>							m_piLutX;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmLutXMemory;
		CVulkan::LSN_IMAGE_VIEW									m_ivLutXView;
		std::unique_ptr<CVulkanBuffer>							m_pbLutXUpload;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmLutXUploadMemory;

		std::unique_ptr<CVulkanImage>							m_piLutY;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmLutYMemory;
		CVulkan::LSN_IMAGE_VIEW									m_ivLutYView;
		std::unique_ptr<CVulkanBuffer>							m_pbLutYUpload;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmLutYUploadMemory;

		std::unique_ptr<CVulkanBuffer>							m_pbVbQuad;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmVbQuadMemory;

		std::unique_ptr<CVulkanPipeline>						m_ppPass1;
		std::unique_ptr<CVulkanPipeline>						m_ppPass2;
		std::unique_ptr<CVulkanPipelineLayout>					m_pplPipelineLayout;
		std::unique_ptr<CVulkanDescriptorSetLayout>				m_pdslDescriptorSetLayout;
		std::unique_ptr<CVulkanDescriptorPool>					m_pdpDescriptorPool;
		CVulkan::LSN_DESCRIPTOR_SET								m_dsPass1;
		CVulkan::LSN_DESCRIPTOR_SET								m_dsPass2;

		CVulkan::LSN_RENDER_PASS								m_rpPass1;
		CVulkan::LSN_RENDER_PASS								m_rpPass2;
		CVulkan::LSN_SAMPLER									m_sSampler;

		/** The filter to use. */
		LSN_FILTER_FUNCS										m_ffFilter = LSN_FF_CARDINALSPLINEUNIFORM;

		uint32_t												m_ui32SrcW = 0, m_ui32SrcH = 0;
		uint32_t												m_ui32DstW = 0, m_ui32DstH = 0;
		uint32_t												m_ui32MaxTapsX = 0, m_ui32MaxTapsY = 0;
		VkFormat												m_fIntermediateFormat = VK_FORMAT_UNDEFINED;
		VkFormat												m_fTargetFormat = VK_FORMAT_UNDEFINED;


		// == Functions.
		/**
		 * Builds a Look-Up Table (LUT) texture for resampling weights.
		 * 
		 * \param _pvkDevice The Vulkan device.
		 * \param _pcbCommandList The command buffer to record upload barriers.
		 * \param _ui32SrcSize The source dimension.
		 * \param _ui32DstSize The destination dimension.
		 * \param _piLut The LUT image to create.
		 * \param _pdmLutMemory The LUT device memory.
		 * \param _ivLutView The LUT image view.
		 * \param _pbUpload The upload buffer.
		 * \param _pdmUploadMemory The upload buffer memory.
		 * \param _ui32OutMaxTaps Receives the maximum kernel taps.
		 * \return Returns true if the LUT was successfully built.
		 **/
		bool													BuildLUT( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandList, uint32_t _ui32SrcSize, uint32_t _ui32DstSize, std::unique_ptr<CVulkanImage> &_piLut, std::unique_ptr<CVulkanDeviceMemory> &_pdmLutMemory, CVulkan::LSN_IMAGE_VIEW &_ivLutView, std::unique_ptr<CVulkanBuffer> &_pbUpload, std::unique_ptr<CVulkanDeviceMemory> &_pdmUploadMemory, uint32_t &_ui32OutMaxTaps );

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
