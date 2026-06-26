/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic helper class for uploading CPU texel arrays to a Vulkan 1.0 texture.
 */

#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"
#include "LSNVulkanImage.h"
#include "LSNVulkanBuffer.h"
#include "LSNVulkanDeviceMemory.h"
#include "LSNVulkanCommandBuffer.h"

#include <memory>

namespace lsn {

	/**
	 * Class CVulkanTextureUploader
	 * \brief A generic helper class for uploading CPU texel arrays to a Vulkan 1.0 texture.
	 *
	 * Description: Owns a texture in device-local memory and handles the upload buffer mapping, memory copying, and GPU command recording to transfer CPU byte arrays to the GPU.
	 */
	class CVulkanTextureUploader {
	public :
		CVulkanTextureUploader();
		~CVulkanTextureUploader();


		// == Functions.
		/**
		 * Resets the texture and internal states.
		 **/
		void													Reset();

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
		bool													UploadTexels( CVulkanDevice * _pvkDevice, CVulkanCommandBuffer * _pcbCommandBuffer, const uint8_t * _pui8Data, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Stride, VkFormat _fFormat = VK_FORMAT_R8G8B8A8_UNORM );

		/**
		 * Gets the underlying Vulkan image wrapper.
		 * 
		 * \return Returns a pointer to the managed image object.
		 **/
		inline CVulkanImage *									GetImage() { return m_piImage.get(); }

		/**
		 * Gets the underlying Vulkan image view.
		 * 
		 * \return Returns the managed image view object.
		 **/
		inline CVulkan::LSN_IMAGE_VIEW &						GetImageView() { return m_ivImageView; }


	protected :
		// == Members.
		/** The managed texture. */
		std::unique_ptr<CVulkanImage>							m_piImage;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmImageMemory;
		CVulkan::LSN_IMAGE_VIEW									m_ivImageView;

		/** The upload buffer used to transfer data. */
		std::unique_ptr<CVulkanBuffer>							m_pbUploadBuffer;
		std::unique_ptr<CVulkanDeviceMemory>					m_pdmUploadMemory;

		uint32_t												m_ui32Width = 0;
		uint32_t												m_ui32Height = 0;
		VkFormat												m_fFormat = VK_FORMAT_UNDEFINED;


		// == Functions.
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
