/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 render target resource.
 */


#ifdef LSN_DX9

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../LSNSurface.h"
#include "LSNDirectX9.h"
#include "LSNDirectX9Device.h"
#include "LSNDirectX9LosableResource.h"
#include "LSNDirectX9Texture.h"

#include <memory>

namespace lsn {

	/**
	 * Class CDirectX9RenderTarget
	 * \brief A Direct3D 9 render target resource.
	 *
	 * Description: A Direct3D 9 render target resource.
	 */
	class CDirectX9RenderTarget : public CDirectX9LosableResource, public CSurface {
	public :
		// == Various constructors.
		CDirectX9RenderTarget( CDirectX9Device * _pdx9dDevice );
		virtual ~CDirectX9RenderTarget();


		// == Functions.
		/**
		 * Must perform some action when the device is lost.
		 */
		virtual void											OnDeviceLost();

		/**
		 * Must renew resources when the device is reset.
		 *
		 * \return Return true if the renewal is successful, false otherwise.
		 */
		virtual bool											OnDeviceReset();

		/**
		 * Resets everything to scratch.
		 */
		void													Reset();

		/**
		 * \brief Returns the underlying level-0 surface (AddRef()'d). Caller must Release().
		 * 
		 * \return Returns a valid IDirect3DSurface9* on success, or nullptr.
		 */
		IDirect3DSurface9 *										GetSurface() const;

		/**
		 * \brief Gets the texture wrapper if this render target was created as a texture RT.
		 * 
		 * \return Returns the color target texture wrapper, or nullptr if not a texture RT.
		 */
		inline CDirectX9Texture *								Texture() { return m_ptColorTex.get(); }

		/**
		 * \brief Creates a color render target as a 2-D texture (level 0), storing both the surface and the texture.
		 *
		 * \param _uiWidth Width in pixels.
		 * \param _uiHeight Height in pixels.
		 * \param _fFormat Floating-point or RGBA format (e.g., LSN_D3DFMT_A16B16G16R16F).
		 * \return Returns true on success.
		 */
		bool													CreateColorTarget( UINT _uiWidth, UINT _uiHeight, D3DFORMAT _fFormat );

		/**
		 * Creates a render target.
		 * 
		 * \param _uiWidth Width of the render-target surface, in pixels.
		 * \param _uiHeight Height of the render-target surface, in pixels.
		 * \param _fFormat Member of the D3DFORMAT enumerated type, describing the format of the render target.
		 * \param _mtMultiSample Member of the D3DMULTISAMPLE_TYPE enumerated type, which describes the multisampling buffer type. This parameter specifies the antialiasing type for this render target. When this surface is passed to IDirect3DDevice9::SetRenderTarget, its multisample type must be the same as that of the depth-stencil set by IDirect3DDevice9::SetDepthStencilSurface.
		 * \param _dwMultisampleQuality Quality level. The valid range is between zero and one less than the level returned by pQualityLevels used by IDirect3D9::CheckDeviceMultiSampleType. Passing a larger value returns the error, D3DERR_INVALIDCALL. The MultisampleQuality values of paired render targets, depth stencil surfaces, and the multisample type must all match.
		 * \param _bLockable Render targets are not lockable unless the application specifies TRUE for Lockable. Note that lockable render targets reduce performance on some graphics hardware. The readback performance (moving data from video memory to system memory) depends on the type of hardware used (AGP vs. PCI Express) and is usually far lower than upload performance (moving data from system to video memory). If you need read access to render targets, use GetRenderTargetData instead of lockable render targets.
		 * \return Returns true if the texture was created.
		 **/
		bool													CreateRenderTarget(
			UINT _uiWidth,
			UINT _uiHeight,
			D3DFORMAT _fFormat,
			D3DMULTISAMPLE_TYPE _mtMultiSample,
			DWORD _dwMultisampleQuality,
			BOOL _bLockable );

		/**
		 * Creates a depth-stencil render target.
		 * 
		 * \param _uiWidth Width of the depth-stencil surface, in pixels.
		 * \param _uiHeight Height of the depth-stencil surface, in pixels.
		 * \param _fFormat Member of the D3DFORMAT enumerated type, describing the format of the depth-stencil surface. This value must be one of the enumerated depth-stencil formats for this device.
		 * \param _mtMultiSample Member of the D3DMULTISAMPLE_TYPE enumerated type, describing the multisampling buffer type. This value must be one of the allowed multisample types. When this surface is passed to IDirect3DDevice9::SetDepthStencilSurface, its multisample type must be the same as that of the render target set by IDirect3DDevice9::SetRenderTarget.
		 * \param _dwMultisampleQuality Quality level. The valid range is between zero and one less than the level returned by pQualityLevels used by IDirect3D9::CheckDeviceMultiSampleType. Passing a larger value returns the error D3DERR_INVALIDCALL. The MultisampleQuality values of paired render targets, depth stencil surfaces, and the MultiSample type must all match.
		 * \param _bDiscard Set this flag to TRUE to enable z-buffer discarding, and FALSE otherwise. If this flag is set, the contents of the depth stencil buffer will be invalid after calling either IDirect3DDevice9::Present or IDirect3DDevice9::SetDepthStencilSurface with a different depth surface. This flag has the same behavior as the constant, D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL, in D3DPRESENTFLAG.
		 * \return Returns true if the texture was created.
		 **/
		bool													CreateDepthStencilSurface(
			UINT _uiWidth,
			UINT _uiHeight,
			D3DFORMAT _fFormat,
			D3DMULTISAMPLE_TYPE _mtMultiSample,
			DWORD _dwMultisampleQuality,
			BOOL _bDiscard );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline IDirect3DSurface9 *								Get() { return m_d3ds9Surface; }

		/**
		 * \brief Determines whether this render target currently wraps a valid surface.
		 * 
		 * \return Returns true if a surface exists.
		 */
		inline bool												Valid() const { return m_d3ds9Surface != nullptr; }


	protected :
		// == Members.
		/** The actual DirectX 9 texture. */
		IDirect3DSurface9 *										m_d3ds9Surface = nullptr;
		/** A pointer to the Direct3D 9 device. */
		CDirectX9Device *										m_pdx9dDevice = nullptr;
		/** If created as a texture RT, we keep the texture wrapper for later sampling. */
		std::unique_ptr<CDirectX9Texture>						m_ptColorTex;

	private :
		typedef CSurface										Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
