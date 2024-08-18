/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for the primary interface to the underlying graphics API.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "Helpers/LSWHelpers.h"

namespace lsn {

	// == Enumerations.
	/** Memory pools. */
	//enum LSN_MEMORY_POOL : uint32_t {
	//	LSN_MP_DEFAULT,																			/**< For GPU-accessible memory in DirectX 9, 12, and Vulkan. */
	//	LSN_MP_MANAGED,																			/**< Managed by runtime, typically with system memory backing (DirectX 9). */
	//	LSN_MP_SYSTEM_MEM,																		/**< Direct CPU access for frequent updates (DirectX 9, Vulkan). */
	//	LSN_MP_READ_BACK,																		/**< GPU to CPU readback (DirectX 12). */
	//	LSN_MP_STAGING,																			/**< Temporary storage for data transfer (Vulkan, DirectX 12). */
	//};

	///** Texture usages. */
	//enum LSN_TEXTURE_USAGE : uint32_t {
	//	LSN_TU_SHADER_RESOURCE,																	/**< Textures used in shaders for sampling. */
	//	LSN_TU_RENDER_TARGET,																	/**< Textures used as render targets. */
	//	LSN_TU_DEPTH_STENCIL,																	/**< Textures used for depth and stencil operations. */
	//	LSN_TU_UNORDERED_ACCESS,																/**< Textures that can be read and written by shaders (DirectX 12, Vulkan). */
	//	LSN_TU_COPY_SRC,																		/**< Textures used as sources for copy operations (DirectX 12, Vulkan). */
	//	LSN_TU_COPY_DST,																		/**< Textures used as destinations for copy operations (DirectX 12, Vulkan). */
	//	LSN_TU_INPUT_ATTACHMENT,																/**< Textures used as input attachments in render passes (Vulkan). */
	//};

	///** Texture formats. */
	//enum LSN_TEXTURE_FORMATS : uint32_t {
	//	LSN_TF_R8_UNORM,																		/**< 8-bit unsigned normalized red channel. */
	//	LSN_TF_R8G8_UNORM,																		/**< 8-bit unsigned normalized red and green channels. */
	//	LSN_TF_R8G8B8_UNORM,																	/**< 8-bit unsigned normalized red, green, and blue channels. */
	//	LSN_TF_R8G8B8A8_UNORM,																	/**< 8-bit unsigned normalized red, green, blue, and alpha channels. */
	//	LSN_TF_R8G8B8_SRGB,																		/**< 8-bit unsigned normalized red, green, and blue channels in sRGB color space. */
	//	LSN_TF_R8G8B8A8_SRGB,																	/**< 8-bit unsigned normalized red, green, blue, and alpha channels in sRGB color space. */
	//	LSN_TF_R16_UINT,																		/**< 16-bit unsigned integer red channel. */
	//	LSN_TF_R16_SFLOAT,																		/**< 16-bit floating-point red channel. */
	//	LSN_TF_R32_SFLOAT,																		/**< 32-bit floating-point red channel. */
	//	LSN_TF_A1R5G5B5_UNORM_PACK16,															/**< 1-bit alpha, 5-bit red, 5-bit green, and 5-bit blue channels packed into 16 bits. */
	//	LSN_TF_BC3_UNORM_BLOCK,																	/**< Compressed format with 8-bit unsigned normalized color and 4-bit alpha. */
	//	LSN_TF_BC3_SRGB_BLOCK,																	/**< Compressed format with 8-bit unsigned normalized color and 4-bit alpha in sRGB color space. */
	//	LSN_TF_BC6H_UFLOAT_BLOCK,																/**< Compressed format with unsigned floating-point color, high dynamic range (HDR). */
	//	LSN_TF_BC7_UNORM_BLOCK,																	/**< Compressed format with 8-bit unsigned normalized color, high quality. */
	//	LSN_TF_BC7_SRGB_BLOCK																	/**< Compressed format with 8-bit unsigned normalized color in sRGB color space, high quality. */
	//};

	/**
	 * Class CGpuBase
	 * \brief The base class for the primary interface to the underlying graphics API.
	 *
	 * Description: The base class for the primary interface to the underlying graphics API.
	 */
	class CGpuBase {
	public :
		CGpuBase();
		virtual ~CGpuBase();


		// == Functions.
		/**
		 * Creates an API device.
		 *
		 * \param _hWnd The window to which to attach.
		 * \param _sAdapter The adapter to use.
		 * \return Returns true if the device was created.
		 **/
		virtual bool											Create( HWND /*_hWnd*/, const std::string &/*_sAdapter*/ );


	protected :
		// == Members.
		lsw::LSW_HMODULE										m_hLib;									/**< The API DLL. */
	};

}	// namespace lsn
