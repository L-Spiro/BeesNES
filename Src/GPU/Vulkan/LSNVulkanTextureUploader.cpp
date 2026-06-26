/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for uploading CPU texel arrays to a Vulkan 1.0 texture.
 */

#ifdef LSN_VULKAN1

#include "LSNVulkanTextureUploader.h"

#include <cstring>

namespace lsn {

	CVulkanTextureUploader::CVulkanTextureUploader() {
	}
	CVulkanTextureUploader::~CVulkanTextureUploader() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the texture and internal states.
	 **/
	void CVulkanTextureUploader::Reset() {
		m_ivImageView.Reset();
		if LSN_LIKELY( m_pdmImageMemory.get() ) { m_pdmImageMemory->Reset(); }
		if LSN_LIKELY( m_piImage.get() ) { m_piImage->Reset(); }
		if LSN_LIKELY( m_pdmUploadMemory.get() ) { m_pdmUploadMemory->Reset(); }
		if LSN_LIKELY( m_pbUploadBuffer.get() ) { m_pbUploadBuffer->Reset(); }

		m_piImage.reset();
		m_pdmImageMemory.reset();
		m_pbUploadBuffer.reset();
		m_pdmUploadMemory.reset();

		m_ui32Width = 0;
		m_ui32Height = 0;
		m_fFormat = VK_FORMAT_UNDEFINED;
	}

	/**
	 * Uploads a texel array to the managed texture, creating or recreating the texture if dimensions or formats change.
	 * 
	 * \param _pvkDevice The active Vulkan device.
	 * \param _pcbCommandBuffer The command buffer used to record the copy commands and transition barriers.
	 * \param _pui8Data The raw RGBA byte array from the CPU.
	 * \param _ui32Width The width of the image.
	 * \param _ui32Height The height of the image.
	 * \param _ui32Stride The pitch of the input byte array.
	 * \param _fFormat The Vulkan format of the texture.
	 * \return Returns true on success.
	 **/
	bool CVulkanTextureUploader::UploadTexels( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandBuffer, const uint8_t * _pui8Data, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Stride, VkFormat _fFormat ) {
		if LSN_UNLIKELY( !_pvkDevice || !_pcbCommandBuffer || !_pui8Data || !_ui32Width || !_ui32Height ) { return false; }

		uint32_t ui32Bpp = 4; 
		VkDeviceSize stImageSize = static_cast<VkDeviceSize>(_ui32Width) * _ui32Height * ui32Bpp;

		if LSN_UNLIKELY( m_ui32Width != _ui32Width || m_ui32Height != _ui32Height || m_fFormat != _fFormat ) {
			Reset();

			m_piImage = std::make_unique<CVulkanImage>();
			VkImageCreateInfo iciImageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			iciImageInfo.imageType = VK_IMAGE_TYPE_2D;
			iciImageInfo.extent.width = _ui32Width;
			iciImageInfo.extent.height = _ui32Height;
			iciImageInfo.extent.depth = 1;
			iciImageInfo.mipLevels = 1;
			iciImageInfo.arrayLayers = 1;
			iciImageInfo.format = _fFormat;
			iciImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			iciImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			iciImageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			iciImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			iciImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

			if ( !m_piImage->CreateImage( _pvkDevice->GetDevice(), &iciImageInfo ) ) { return false; }

			VkMemoryRequirements mrImageMemReq;
			CVulkan::m_pfGetImageMemoryRequirements( _pvkDevice->GetDevice(), m_piImage->Get(), &mrImageMemReq );

			VkMemoryAllocateInfo maiImageAllocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			maiImageAllocInfo.allocationSize = mrImageMemReq.size;
			maiImageAllocInfo.memoryTypeIndex = FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrImageMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

			m_pdmImageMemory = std::make_unique<CVulkanDeviceMemory>();
			if ( !m_pdmImageMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiImageAllocInfo ) ) { return false; }
			CVulkan::m_pfBindImageMemory( _pvkDevice->GetDevice(), m_piImage->Get(), m_pdmImageMemory->Get(), 0 );

			VkImageViewCreateInfo ivciViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			ivciViewInfo.image = m_piImage->Get();
			ivciViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ivciViewInfo.format = _fFormat;
			ivciViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ivciViewInfo.subresourceRange.baseMipLevel = 0;
			ivciViewInfo.subresourceRange.levelCount = 1;
			ivciViewInfo.subresourceRange.baseArrayLayer = 0;
			ivciViewInfo.subresourceRange.layerCount = 1;
			m_ivImageView.Create( _pvkDevice->GetDevice(), &ivciViewInfo );

			m_pbUploadBuffer = std::make_unique<CVulkanBuffer>();
			VkBufferCreateInfo bciBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			bciBufferInfo.size = stImageSize;
			bciBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			bciBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if ( !m_pbUploadBuffer->CreateBuffer( _pvkDevice->GetDevice(), &bciBufferInfo ) ) { return false; }

			VkMemoryRequirements mrBufferMemReq;
			CVulkan::m_pfGetBufferMemoryRequirements( _pvkDevice->GetDevice(), m_pbUploadBuffer->Get(), &mrBufferMemReq );

			VkMemoryAllocateInfo maiBufferAllocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			maiBufferAllocInfo.allocationSize = mrBufferMemReq.size;
			maiBufferAllocInfo.memoryTypeIndex = FindMemoryType( _pvkDevice->GetPhysicalDevice(), mrBufferMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

			m_pdmUploadMemory = std::make_unique<CVulkanDeviceMemory>();
			if ( !m_pdmUploadMemory->AllocateMemory( _pvkDevice->GetDevice(), &maiBufferAllocInfo ) ) { return false; }
			CVulkan::m_pfBindBufferMemory( _pvkDevice->GetDevice(), m_pbUploadBuffer->Get(), m_pdmUploadMemory->Get(), 0 );

			m_ui32Width = _ui32Width;
			m_ui32Height = _ui32Height;
			m_fFormat = _fFormat;
		}

		void * pvData = nullptr;
		if ( CVulkan::m_pfMapMemory( _pvkDevice->GetDevice(), m_pdmUploadMemory->Get(), 0, stImageSize, 0, &pvData ) == VK_SUCCESS ) {
			uint32_t ui32CopyStride = _ui32Width * ui32Bpp;

			if LSN_LIKELY( ui32CopyStride == _ui32Stride ) {
				std::memcpy( pvData, _pui8Data, static_cast<size_t>(_ui32Height) * _ui32Stride );
			}
			else {
				for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
					const uint8_t * pui8SrcRow = _pui8Data + Y * _ui32Stride;
					uint8_t * pui8DstRow = reinterpret_cast<uint8_t *>(pvData) + Y * ui32CopyStride;
					std::memcpy( pui8DstRow, pui8SrcRow, ui32CopyStride );
				}
			}
			CVulkan::m_pfUnmapMemory( _pvkDevice->GetDevice(), m_pdmUploadMemory->Get() );
		}

		VkImageMemoryBarrier imbBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		imbBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imbBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imbBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imbBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imbBarrier.image = m_piImage->Get();
		imbBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imbBarrier.subresourceRange.baseMipLevel = 0;
		imbBarrier.subresourceRange.levelCount = 1;
		imbBarrier.subresourceRange.baseArrayLayer = 0;
		imbBarrier.subresourceRange.layerCount = 1;
		imbBarrier.srcAccessMask = 0;
		imbBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		CVulkan::m_pfCmdPipelineBarrier(
			_pcbCommandBuffer->Get(),
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &imbBarrier
		);

		VkBufferImageCopy bicRegion = {};
		bicRegion.bufferOffset = 0;
		bicRegion.bufferRowLength = 0;
		bicRegion.bufferImageHeight = 0;
		bicRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bicRegion.imageSubresource.mipLevel = 0;
		bicRegion.imageSubresource.baseArrayLayer = 0;
		bicRegion.imageSubresource.layerCount = 1;
		bicRegion.imageOffset = { 0, 0, 0 };
		bicRegion.imageExtent = { _ui32Width, _ui32Height, 1 };

		CVulkan::m_pfCmdCopyBufferToImage(
			_pcbCommandBuffer->Get(),
			m_pbUploadBuffer->Get(),
			m_piImage->Get(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &bicRegion
		);

		imbBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imbBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imbBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imbBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		CVulkan::m_pfCmdPipelineBarrier(
			_pcbCommandBuffer->Get(),
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &imbBarrier
		);

		return true;
	}

	/**
	 * Helper to find an appropriate memory type index for allocations.
	 * 
	 * \param _pdDevice The physical device.
	 * \param _ui32TypeFilter A bitmask specifying the acceptable memory types.
	 * \param _mpfProperties The required memory properties.
	 * \return Returns the index of the memory type, or 0 if none is found.
	 **/
	uint32_t CVulkanTextureUploader::FindMemoryType( VkPhysicalDevice _pdDevice, uint32_t _ui32TypeFilter, VkMemoryPropertyFlags _mpfProperties ) {
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
