/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 render target resource.
 */


#ifdef LSN_DX9

#include "LSNDirectX9RenderTarget.h"

namespace lsn {

	CDirectX9RenderTarget::CDirectX9RenderTarget( CDirectX9Device * _pdx9dDevice ) :
		m_pdx9dDevice( _pdx9dDevice ),
		m_d3ds9Surface( nullptr ) {
	}
	CDirectX9RenderTarget::~CDirectX9RenderTarget() {
		Reset();
	}

	// == Functions.
	/**
	 * Must perform some action when the device is lost.
	 */
	void CDirectX9RenderTarget::OnDeviceLost() {
		if ( !m_bResourceCanBeLost || !m_d3ds9Surface ) { return; }

		CDirectX9::SafeRelease( m_d3ds9Surface );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	bool CDirectX9RenderTarget::OnDeviceReset() {
		if ( !m_bResourceCanBeLost ) { return true; }
		//return CreateApiTexture( NULL, 0 );
		return true;
	}

	/**
	 * Resets everything to scratch.
	 */
	void CDirectX9RenderTarget::Reset() {
		CDirectX9::SafeRelease( m_d3ds9Surface );

		m_bResourceCanBeLost = false;
		Parent::Reset();
	}

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
	bool CDirectX9RenderTarget::CreateRenderTarget(
		UINT _uiWidth,
		UINT _uiHeight,
		D3DFORMAT _fFormat,
		D3DMULTISAMPLE_TYPE _mtMultiSample,
		DWORD _dwMultisampleQuality,
		BOOL _bLockable ) {
		Reset();
		if ( SUCCEEDED( m_pdx9dDevice->GetDirectX9Device()->CreateRenderTarget( _uiWidth, _uiHeight, _fFormat, _mtMultiSample, _dwMultisampleQuality, _bLockable,
			&m_d3ds9Surface, nullptr ) ) ) {
			SetSurface( _uiWidth, _uiHeight, 1 );
			return true;
		}
		return false;
	}

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
	bool CDirectX9RenderTarget::CreateDepthStencilSurface(
		UINT _uiWidth,
		UINT _uiHeight,
		D3DFORMAT _fFormat,
		D3DMULTISAMPLE_TYPE _mtMultiSample,
		DWORD _dwMultisampleQuality,
		BOOL _bDiscard ) {
		Reset();
		if ( SUCCEEDED( m_pdx9dDevice->GetDirectX9Device()->CreateDepthStencilSurface( _uiWidth, _uiHeight, _fFormat, _mtMultiSample, _dwMultisampleQuality, _bDiscard,
			&m_d3ds9Surface, nullptr ) ) ) {
			SetSurface( _uiWidth, _uiHeight, 1 );
			return true;
		}
		return false;
	}

}	// namespace lsn

#endif	// #ifdef LSN_DX9
