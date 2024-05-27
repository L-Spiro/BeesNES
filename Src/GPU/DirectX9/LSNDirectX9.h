/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The root of DirectX 9 functionality.
 */


#pragma once

#ifdef LSN_DX9

#include "../../LSNLSpiroNes.h"
#include "../../OS/LSNWindows.h"
#include "../LSNGpuBase.h"

#include <d3d9.h>

namespace lsn {

#if 0
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Enumerations.
	/** The blend types we support.  These are for blending to the backbuffer only. */
	enum LSN_BLEND_MODE {
		LSN_BM_ZERO									= D3DBLEND_ZERO,								/**< Blend factor is (0, 0, 0, 0). */
		LSN_BM_ONE									= D3DBLEND_ONE,									/**< Blend factor is (1, 1, 1, 1). */
		LSN_BM_SRC_COLOR							= D3DBLEND_SRCCOLOR,							/**< Blend factor is (Rs, Gs, Bs, As). */
		LSN_BM_INV_SRC_COLOR						= D3DBLEND_INVSRCCOLOR,							/**< Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As). */
		LSN_BM_DST_COLOR							= D3DBLEND_DESTCOLOR,							/**< Blend factor is (Rd, Gd, Bd, Ad). */
		LSN_BM_INV_DST_COLOR						= D3DBLEND_INVDESTCOLOR,						/**< Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad). */
		LSN_BM_SRC_ALPHA							= D3DBLEND_SRCALPHA,							/**< Blend factor is (As, As, As, As). */
		LSN_BM_INV_SRC_ALPHA						= D3DBLEND_INVSRCALPHA,							/**< Blend factor is (1 - As, 1 - As, 1 - As, 1 - As). */
		LSN_BM_DST_ALPHA							= D3DBLEND_DESTALPHA,							/**< Blend factor is (Ad Ad Ad Ad). */
		LSN_BM_INV_DEST_ALPHA						= D3DBLEND_INVDESTALPHA,						/**< Blend factor is (1 - Ad 1 - Ad 1 - Ad 1 - Ad). */
		LSN_BM_SRC_ALPHA_SAT						= D3DBLEND_SRCALPHASAT,							/**< Blend factor is (f, f, f, 1), where f = min(As, 1 - Ad). */

		LSN_BM_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** The blend operations we support. */
	enum LSN_BLEND_OP {
		LSN_BO_ADD									= D3DBLENDOP_ADD,								/**< Add source 1 and source 2. */
		LSN_BO_SUBTRACT								= D3DBLENDOP_SUBTRACT,							/**< Subtract source 1 and source 2. */
		LSN_BO_REV_SUBTRACT							= D3DBLENDOP_REVSUBTRACT,						/**< Subtract source 2 from source 1. */
		LSN_BO_MIN									= D3DBLENDOP_MIN,								/**< Find the minimum of source 1 and source 2. */
		LSN_BO_MAX									= D3DBLENDOP_MAX,								/**< Find the maximum of source 1 and source 2. */

		LSN_BO_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** Buffers. */
	enum LSN_BUFFER {
		LSN_B_COLOR_BUFFER							= D3DCLEAR_TARGET,								/**< Clear a render target. */
		LSN_B_STENCIL_BUFFER						= D3DCLEAR_STENCIL,								/**< Clear the stencil buffer. */
		LSN_B_DEPTH_BUFFER							= D3DCLEAR_ZBUFFER,								/**< Clear the depth buffer. */
		LSN_B_ALL									= LSN_B_COLOR_BUFFER | LSN_B_STENCIL_BUFFER |
			LSN_B_DEPTH_BUFFER,																		/**< Clear all buffers. */
	};

	/** The color-write modes. */
	enum LSN_COLOR_WRITE_ENABLE {
		LSN_CWM_RED									= D3DCOLORWRITEENABLE_RED,						/**< Allow data to be stored in the red component. */
		LSN_CWM_GREEN								= D3DCOLORWRITEENABLE_GREEN,					/**< Allow data to be stored in the green component. */
		LSN_CWM_BLUE								= D3DCOLORWRITEENABLE_BLUE,						/**< Allow data to be stored in the blue component. */
		LSN_CWM_ALPHA								= D3DCOLORWRITEENABLE_ALPHA,					/**< Allow data to be stored in the alpha component. */
		LSN_CWM_ALL									= LSN_CWM_RED | LSN_CWM_GREEN | LSN_CWM_BLUE |
			LSN_CWM_ALPHA,																			/**< Allow data to be stored in all components. */
	};

	/** Comparison functions. */
	enum LSN_COMPARISON_FUNC {
		LSN_CF_NEVER								= D3DCMP_NEVER,									/**< Never pass the comparison. */
		LSN_CF_ALWAYS								= D3DCMP_ALWAYS,								/**< Always pass the comparison. */
		LSN_CF_LESS									= D3DCMP_LESS,									/**< If the source data is less than the destination data, the comparison passes. */
		LSN_CF_EQUAL								= D3DCMP_EQUAL,									/**< If the source data is equal to the destination data, the comparison passes. */
		LSN_CF_NOTEQUAL								= D3DCMP_NOTEQUAL,								/**< If the source data is not equal to the destination data, the comparison passes. */
		LSN_CF_LESSEQUAL							= D3DCMP_LESSEQUAL,								/**< If the source data is less than or equal to the destination data, the comparison passes. */
		LSN_CF_GREATER								= D3DCMP_GREATER,								/**< If the source data is greater than the destination data, the comparison passes. */
		LSN_CF_GREATEREQUAL							= D3DCMP_GREATEREQUAL,							/**< If the source data is greater than or equal to the destination data, the comparison passes. */
	};

	/** Culling modes. */
	enum LSN_CULL_MODE {
		LSN_CM_NONE									= D3DCULL_NONE,									/**< No culling. */
		LSN_CM_FRONT								= D3DCULL_CCW,									/**< Cull front faces. */
		LSN_CM_BACK									= D3DCULL_CW,									/**< Cull back faces. */
	};

	/** Depth write masks. */
	enum LSN_DEPTH_WRITE_MASK {
		LSN_DRM_ZERO								= 0,											/**< Turn off writes to the depth-stencil buffer. */
		LSN_DRM_ALL									= 1,											/**< Turn on writes to the depth-stencil buffer. */
	};

	/** Fill modes. */
	enum LSN_FILL_MODE {
		LSN_FM_SOLID								= D3DFILL_SOLID,								/**< Solid filling mode. */
		LSN_FM_WIRE									= D3DFILL_WIREFRAME,							/**< Wire-frame filling mode. */
	};

	/** Filters. */
	enum LSN_FILTER {
		/** Use point sampling for minification, magnification, and mip-level sampling. */
		LSN_F_MIN_MAG_MIP_POINT						= LSN_ENC_BASIC_FILTER( LSN_FILTER_POINT, LSN_FILTER_POINT, LSN_FILTER_POINT, false ),
		/** Use point sampling for minification and magnification; use linear interpolation for mip-level sampling. */
		LSN_F_MIN_MAG_POINT_MIP_LINEAR				= LSN_ENC_BASIC_FILTER( LSN_FILTER_POINT, LSN_FILTER_POINT, LSN_FILTER_LINEAR, false ),
		/** Use point sampling for minification; use linear interpolation for magnification; use point sampling for mip-level sampling. */
		LSN_F_MIN_POINT_MAG_LINEAR_MIP_POINT		= LSN_ENC_BASIC_FILTER( LSN_FILTER_POINT, LSN_FILTER_LINEAR, LSN_FILTER_POINT, false ),
		/** Use point sampling for minification; use linear interpolation for magnification and mip-level sampling. */
		LSN_F_MIN_POINT_MAG_MIP_LINEAR				= LSN_ENC_BASIC_FILTER( LSN_FILTER_POINT, LSN_FILTER_LINEAR, LSN_FILTER_LINEAR, false ),
		/** Use linear interpolation for minification; use point sampling for magnification and mip-level sampling. */
		LSN_F_MIN_LINEAR_MAG_MIP_POINT				= LSN_ENC_BASIC_FILTER( LSN_FILTER_LINEAR, LSN_FILTER_POINT, LSN_FILTER_POINT, false ),
		/** Use linear interpolation for minification; use point sampling for magnification; use linear interpolation for mip-level sampling. */
		LSN_F_MIN_LINEAR_MAG_POINT_MIP_LINEAR		= LSN_ENC_BASIC_FILTER( LSN_FILTER_LINEAR, LSN_FILTER_POINT, LSN_FILTER_LINEAR, false ),
		/** Use linear interpolation for minification and magnification; use point sampling for mip-level sampling. */
		LSN_F_MIN_MAG_LINEAR_MIP_POINT				= LSN_ENC_BASIC_FILTER( LSN_FILTER_LINEAR, LSN_FILTER_LINEAR, LSN_FILTER_POINT, false ),
		/** Use linear interpolation for minification, magnification, and mip-level sampling. */
		LSN_F_MIN_MAG_MIP_LINEAR					= LSN_ENC_BASIC_FILTER( LSN_FILTER_LINEAR, LSN_FILTER_LINEAR, LSN_FILTER_LINEAR, false ),
		/** Use anisotropic interpolation for minification, magnification, and mip-level sampling. */
		LSN_F_ANISOTROPIC							= LSN_ENCODE_ANISOTROPIC_FILTER( false ),
	};

	/** Stencil operations. */
	enum LSN_STENCIL_OP {
		LSN_SO_KEEP									= D3DSTENCILOP_KEEP,							/**< Keep the existing stencil data. */
		LSN_SO_ZERO									= D3DSTENCILOP_ZERO,							/**< Set the stencil data to 0. */
		LSN_SO_REPLACE								= D3DSTENCILOP_REPLACE,							/**< Set the stencil data to the reference value. */
		LSN_SO_INC_SAT								= D3DSTENCILOP_INCRSAT,							/**< Increment the stencil value by 1, and clamp the result. */
		LSN_SO_DEC_SAT								= D3DSTENCILOP_DECRSAT,							/**< Decrement the stencil value by 1, and clamp the result. */
		LSN_SO_INVERT								= D3DSTENCILOP_INVERT,							/**< Invert the stencil data. */
		LSN_SO_INC									= D3DSTENCILOP_INCR,							/**< Increment the stencil value by 1, and wrap the result if necessary. */
		LSN_SO_DEC									= D3DSTENCILOP_DECR,							/**< Decrement the stencil value by 1, and wrap the result if necessary. */
	};

	/** Texture addressing modes. */
	enum LSN_TEXTURE_ADDRESS_MODE {
		LSN_TAM_WRAP								= D3DTADDRESS_WRAP,								/**< Tile the texture at every (u,v) integer junction. */
		LSN_TAM_MIRROR								= D3DTADDRESS_MIRROR,							/**< Flip the texture at every (u,v) integer junction. */
		LSN_TAM_CLAMP								= D3DTADDRESS_CLAMP,							/**< Texture coordinates outside the range [0.0, 1.0] are set to the texture color at 0.0 or 1.0, respectively. */
		LSN_TAM_BORDER								= D3DTADDRESS_BORDER,							/**< Texture coordinates outside the range [0.0, 1.0] are set to the border color. */
		LSN_TAM_MIRROR_ONCE							= D3DTADDRESS_MIRRORONCE,						/**< Similar to LSN_TAM_MIRROR and LSN_TAM_CLAMP. Takes the absolute value of the texture coordinate (thus, mirroring around 0), and then clamps to the maximum value. */
	};

	/** Compile-time metrics.  For run-time, use the metrics structure returned by the graphics device. */
	enum LSN_METRICS {
		LSN_MAX_TEXTURE_UNITS						= 16,											/**< Total texture units bindable to a shader at a time. */
		LSN_MAX_SHADER_SAMPLERS						= 16,											/**< Total number of samplers that can be set on a shader at once. */
		LSN_MAX_RENDER_TARGET_COUNT					= 4,											/**< Total simultaneous render-target count. */
		LSN_MAX_VIEWPORTS_SCISSORS					= 1,											/**< Maximum number of viewports and scissors. */
		LSN_VIEWPORT_MIN							= 0,											/**< Minimum viewport bounds. */
		LSN_VIEWPORT_MAX							= 4096,											/**< Maximum viewport bounds. */
		LSN_MAX_VERTEX_ELEMENT_COUNT				= 16,											/**< Maximum number of inputs to a vertex shader. */
		LSN_MAX_VERTEX_STREAMS						= 16,											/**< Maximum number of simultaneous vertex buffers bound. */
		LSN_MAX_VERTEX_SEMANTIC						= 16,											/**< Maximum number of vertex buffer semantic indices. */
		
		LSN_FORCE_DWORD								= 0x7FFFFFFF
	};

	// Floating-point metrics.
#define LSN_MIN_DEPTH								(0.0f)											/**< Minimum depth value. */
#define LSN_MAX_DEPTH								(1.0f)											/**< Maximum depth value. */

	// Default values.
#define LSN_DEFAULT_STENCIL_READ_MASK				(0xFF)											/**< Default stencil read mask. */
#define LSN_DEFAULT_STENCIL_WRITE_MASK				(0xFF)											/**< Default stencil write mask. */
#endif	// 0


	// == Types.
	/**
	 * Class CDirectX9
	 * \brief The root of DirectX 9 functionality.
	 *
	 * Description: The root of DirectX 9 functionality.
	 */
	class CDirectX9 {
	public :
		// == Types.
		typedef IDirect3D9 * (WINAPI *							PfDirect3DCreate9)( UINT );


		// == Functions.
		/**
		 * Determines whether DirectX 9 is available.
		 *
		 * \return Returns TRUE if DirectX 9 functionality is available.
		 **/
		static inline BOOL										Supported();


	protected :
		// == Members.
		static BOOL												m_bSupported;								/**< Is Direct3D 9 supported? */


		// == Functions.
		/**
		 * Checks for Direct3D 9 support.
		 * 
		 * \return Returns true if Direct3D 9 is supported.
		 **/
		static bool												IsSupported();
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Determines whether DirectX 9 is available.
	 *
	 * \return Returns TRUE if DirectX 9 functionality is available.
	 **/
	inline BOOL CDirectX9::Supported() {
		if ( m_bSupported == 3 ) { m_bSupported = IsSupported(); }
		return m_bSupported;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
