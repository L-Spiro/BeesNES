/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Vulkan 1.0 image.
 */


#ifdef LSN_VULKAN1

#include "LSNVulkanImage.h"

namespace lsn {

	// == Various constructors.
	CVulkanImage::CVulkanImage() {
	}
	CVulkanImage::~CVulkanImage() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object back to scratch.
	 **/
	void CVulkanImage::Reset() {
		m_iImage.Reset();
	}

	/**
	 * Creates an image object.
	 * 
	 * \param _dDevice The Vulkan logical device.
	 * \param _piciCreateInfo A pointer to a VkImageCreateInfo structure containing parameters to be used to create the image.
	 * \return Returns true if the image was created.
	 **/
	bool CVulkanImage::CreateImage(
		VkDevice _dDevice,
		const VkImageCreateInfo * _piciCreateInfo ) {
		if ( !_dDevice || !_piciCreateInfo ) { return false; }
		Reset();
		return m_iImage.Create( _dDevice, _piciCreateInfo );
	}

}	// namespace lsn

#endif	// #ifdef LSN_VULKAN1
