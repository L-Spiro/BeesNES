#ifdef LSN_VULKAN1

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The Vulkan 1.0 palette filter.
 */

#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNVulkanFilterBase.h"

#include "../GPU/Vulkan/LSNVulkanImage.h"
#include "../GPU/Vulkan/LSNVulkanDeviceMemory.h"
#include "../GPU/Vulkan/LSNVulkanBuffer.h"
#include "../GPU/Vulkan/LSNVulkanPipeline.h"
#include "../GPU/Vulkan/LSNVulkanPipelineLayout.h"
#include "../GPU/Vulkan/LSNVulkanDescriptorSetLayout.h"
#include "../GPU/Vulkan/LSNVulkanDescriptorPool.h"
#include "../GPU/Vulkan/LSNVulkanCommandPool.h"
#include "../GPU/Vulkan/LSNVulkanCommandBuffer.h"
#include "../GPU/Vulkan/LSNVulkanFence.h"
#include "../GPU/Vulkan/LSNVulkanSemaphore.h"

#include <Helpers/LSWHelpers.h>
#include <memory>
#include <vector>

namespace lsn {

	/**
	 * Class CVulkanPaletteFilter
	 * \brief A standard 24-bit RGB filter for Vulkan 1.0.
	 *
	 * Description: A standard 24-bit RGB filter utilizing Vulkan Pipeline State Objects and explicit transfer buffers.
	 */
	class CVulkanPaletteFilter : public CVulkanFilterBase {
	public :
		CVulkanPaletteFilter();
		virtual ~CVulkanPaletteFilter();


		// == Functions.
		/**
		 * Sets the basic parameters for the filter.
		 *
		 * \param _stBuffers The number of render targets to create.
		 * \param _ui16Width The console screen width.  Typically 256.
		 * \param _ui16Height The console screen height.  Typically 240.
		 * \return Returns the input format requested of the PPU.
		 */
		virtual CDisplayClient::LSN_PPU_OUT_FORMAT				Init( size_t _stBuffers, uint16_t _ui16Width, uint16_t _ui16Height ) override;

		/**
		 * Gets the convolution sampler to use for resampling.
		 * 
		 * \param _ui32Width The target width.
		 * \param _ui32Height The target height.
		 * \return Returns the desired convolution sampler to use.
		 **/
		virtual inline CResamplerBase::LSN_FILTER_FUNCS			GetPreferredConvolutionFilter( uint32_t _ui32Width, uint32_t _ui32Height ) override {
			float fResolutionFactor = std::min( static_cast<float>(_ui32Width) / static_cast<float>(m_ui32SrcW), static_cast<float>(_ui32Height) / static_cast<float>(m_ui32SrcH) );
			if ( fResolutionFactor < 2.5 ) { return CResamplerBase::LSN_FF_CARDINALSPLINEUNIFORM; }
			if ( fResolutionFactor < 3.5 ) { return CResamplerBase::LSN_FF_ROBIDOUXSHARP; }
			return CResamplerBase::LSN_FF_LINEAR;
		}

		/**
		 * Sets the palette.
		 * 
		 * \param _pfRgba512 Pointer to 2048 floats (512 * RGBA).
		 * \return Returns true if the memory for the palette copy was able to be allocated and _pfRgba512 is not nullptr.
		 **/
		bool													SetLut( const float * _pfRgba512 );

		/**
		 * Called when the filter is about to become active.
		 */
		virtual void											Activate() override;

		/**
		 * Called when the filter is about to become inactive.
		 */
		virtual void											DeActivate() override;

		/**
		 * Tells the filter that rendering to the source buffer has completed and that it should filter the results.
		 *
		 * \param _pui8Input The buffer to be filtered.
		 * \param _ui32Width On input, this is the width of the buffer in pixels.  On return, it is filled with the final width.
		 * \param _ui32Height On input, this is the height of the buffer in pixels.  On return, it is filled with the final height.
		 * \param _ui16BitDepth On input, this is the bit depth of the buffer.  On return, it is filled with the final bit depth.
		 * \param _ui32Stride On input, this is the stride of the buffer.  On return, it is filled with the final stride, in bytes.
		 * \param _ui64PpuFrame The PPU frame associated with the input data.
		 * \param _ui64RenderStartCycle The cycle at which rendering of the first pixel began.
		 * \param _i32DispLeft The display area left.
		 * \param _i32DispTop The display area top.
		 * \param _ui32DispWidth The display area width.
		 * \param _ui32DispHeight The display area height
		 * \return Returns a pointer to the filtered output buffer.
		 */
		virtual uint8_t *										ApplyFilter( uint8_t * _pui8Input, uint32_t &_ui32Width, uint32_t &_ui32Height, uint16_t &_ui16BitDepth, uint32_t &_ui32Stride, uint64_t _ui64PpuFrame, uint64_t _ui64RenderStartCycle,
			int32_t _i32DispLeft, int32_t _i32DispTop, uint32_t _ui32DispWidth, uint32_t _ui32DispHeight ) override;

		/**
		 * Informs the filter of a window resize.
		 **/
		virtual void											FrameResize() override;


	protected :
		// == Members.
		CVulkanCommandPool										m_cpCommandPool;				/**< Command pool for managing the active frame. */
		CVulkanCommandBuffer									m_cbCommandBuffer;				/**< Command buffer for recording passes. */

		CVulkanFence											m_fRenderFence;					/**< GPU to CPU synchronization fence. */
		CVulkanSemaphore										m_sImageAvailable;				/**< Semaphore signaled when the swapchain image is ready. */
		CVulkanSemaphore										m_sRenderFinished;				/**< Semaphore signaled when drawing commands complete. */

		CVulkan::LSN_RENDER_PASS								m_rpInitialPass;				/**< The render pass format utilized by the palette step. */
		std::unique_ptr<CVulkanImage>							m_piInitial;					/**< Initial floating-point render target. */
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmInitialMemory;				/**< Memory for the initial render target. */
		CVulkan::LSN_IMAGE_VIEW									m_ivInitialView;				/**< View of the initial render target. */
		CVulkan::LSN_FRAMEBUFFER								m_fbInitial;					/**< Framebuffer wrapping the initial target. */

		std::unique_ptr<CVulkanImage>							m_piIndex;						/**< Index texture (16-bit). */
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmIndexMemory;				/**< Memory for the index texture. */
		CVulkan::LSN_IMAGE_VIEW									m_ivIndexView;					/**< View of the index texture. */
		std::unique_ptr<CVulkanBuffer>							m_pbIndexUpload;				/**< Host-visible buffer for index data upload. */
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmIndexUploadMemory;			/**< Memory for the index upload buffer. */

		std::unique_ptr<CVulkanImage>							m_piPalette;					/**< 512x1 LUT texture. */
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmPaletteMemory;				/**< Memory for the palette texture. */
		CVulkan::LSN_IMAGE_VIEW									m_ivPaletteView;				/**< View of the palette texture. */
		std::unique_ptr<CVulkanBuffer>							m_pbPaletteUpload;				/**< Host-visible buffer for palette data upload. */
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmPaletteUploadMemory;		/**< Memory for the palette upload buffer. */

		std::unique_ptr<CVulkanBuffer>							m_pbVbQuad;						/**< Screen-space quad vertex buffer. */
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmVbQuadMemory;				/**< Memory for the quad vertex buffer. */

		std::unique_ptr<CVulkanPipeline>						m_ppShader;						/**< The pipeline state object for the palette resolve pass. */
		std::unique_ptr<CVulkanPipelineLayout>					m_pplPipelineLayout;			/**< The layout describing bindings to the shader. */
		std::unique_ptr<CVulkanDescriptorSetLayout>				m_pdslDescriptorSetLayout;		/**< The layout of the descriptor set. */
		std::unique_ptr<CVulkanDescriptorPool>					m_pdpDescriptorPool;			/**< Pool from which the descriptor set is allocated. */
		CVulkan::LSN_DESCRIPTOR_SET								m_dsTextureSet;					/**< The descriptor set holding the bound textures. */

		std::vector<float>										m_vLut;							/**< The palette look-up table. */
		std::vector<uint8_t>									m_vOutputBuffer;				/**< The output buffer. */

		uint32_t												m_ui32SrcW = 0;					/**< The native source width. */
		uint32_t												m_ui32SrcH = 0;					/**< The native source height. */
		bool													m_bValidState = false;			/**< True if all Vulkan components are valid. */
		bool													m_bUpdatePalette = true;		/**< True if the palette LUT needs to be uploaded. */
		bool													m_bIndicesDirty = false;		/**< True if indices were uploaded and need to be transitioned. */


		// == Functions.
		/**
		 * \brief Ensures internal size is updated and size-dependent resources are (re)created.
		 * 
		 * \return Returns true on success.
		 */
		bool													EnsureSizeAndResources();

		/**
		 * \brief Ensures pixel shaders, vertex shaders, and PSOs are created.
		 * 
		 * \return Returns true if all shaders are ready.
		 */
		bool													EnsureShaders();

		/**
		 * \brief Releases size-dependent resources.
		 */
		void													ReleaseSizeDependents();

		/**
		 * Creates a Vulkan shader module from SPIR-V code.
		 *
		 * \param _pvkDevice The Vulkan device.
		 * \param _vSpirv The compiled SPIR-V code.
		 * \param _smModule The shader module wrapper to populate.
		 * \return Returns true on success.
		 */
		bool													LoadSpirv( CVulkanDevice * _pvkDevice, const std::vector<uint32_t> &_vSpirv, CVulkan::LSN_SHADER_MODULE &_smModule );

		/**
		 * \brief Updates the 512-entry float RGBA LUT in the upload buffer.
		 * 
		 * \return Returns true on success.
		 */
		bool													UpdateLut();

		/**
		 * \brief Uploads the 16-bit PPU indices to the upload buffer.
		 * 
		 * \param _pui16Idx Source pointer to the index image (row-major).
		 * \param _ui32W Image width in pixels.
		 * \param _ui32H Image height in pixels.
		 * \param _ui32SrcPitch Source pitch in bytes; pass 0 for tightly packed.
		 * \return Returns true on success.
		 */
		bool													UploadIndices( const uint16_t * _pui16Idx, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32SrcPitch = 0 );

		/**
		 * \brief Renders the pipeline to the backbuffer.
		 * 
		 * \param _rOutput The destination rectangle in client pixels where the NES image should appear.
		 * \param _ui32ImageIndex The swapchain image index.
		 * \return Returns true if the draw succeeded; false on failure.
		 */
		bool													Render( const lsw::LSW_RECT &_rOutput, uint32_t _ui32ImageIndex );

	private :
		typedef CVulkanFilterBase								CParent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
