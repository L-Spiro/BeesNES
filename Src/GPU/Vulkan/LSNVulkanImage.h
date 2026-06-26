/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 image.
 */


#pragma once

#ifdef LSN_VULKAN1

#include "../../LSNLSpiroNes.h"
#include "LSNVulkanDevice.h"

namespace lsn {

	/**
	 * Class CVulkanImage
	 * \brief A Vulkan 1.0 image.
	 *
	 * Description: A Vulkan 1.0 image.
	 */
	class CVulkanImage {
	public :
		// == Various constructors.
		CVulkanImage();
		~CVulkanImage();


		// == Functions.
		/**
		 * Resets the object back to scratch.
		 **/
		void													Reset();

		/**
		 * Creates an image object.
		 * 
		 * \param _dDevice The Vulkan logical device.
		 * \param _piciCreateInfo A pointer to a VkImageCreateInfo structure containing parameters to be used to create the image.
		 * \return Returns true if the image was created.
		 **/
		bool													CreateImage(
			VkDevice _dDevice,
			const VkImageCreateInfo * _piciCreateInfo );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns the wrapped Vulkan image handle.
		 **/
		inline VkImage											Get() const { return m_iImage.iImage; }


	protected :
		// == Members.
		/** The wrapped RAII image object. */
		CVulkan::LSN_IMAGE										m_iImage;
	};

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
