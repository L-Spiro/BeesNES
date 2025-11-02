#ifdef LSN_DX9

/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 pixel shader resource.
 */

#pragma once

#include "../../LSNLSpiroNes.h"
#include "LSNDirectX9.h"
#include "LSNDirectX9Device.h"
#include "LSNDirectX9LosableResource.h"
#include <vector>

namespace lsn {

	/**
	 * Class CDirectX9PixelShader
	 * \brief A Direct3D 9 pixel shader resource.
	 *
	 * Description: Wraps an IDirect3DPixelShader9 and participates in the losable-resource system.
	 * On device loss, the shader is released; on reset, it is recreated from cached bytecode or
	 * re-assembled from ASCII assembly if provided.
	 */
	class CDirectX9PixelShader : public CDirectX9LosableResource {
	public :
		// == Various constructors.
		CDirectX9PixelShader( CDirectX9Device * _pdx9dDevice );
		virtual ~CDirectX9PixelShader();


		// == Functions.
		/**
		 * Must perform some action when the device is lost.
		 */
		virtual void											OnDeviceLost() override;

		/**
		 * Must renew resources when the device is reset.
		 *
		 * \return Return true if the renewal is successful, false otherwise.
		 */
		virtual bool											OnDeviceReset() override;

		/**
		 * Resets everything to scratch.
		 */
		void													Reset();

		/**
		 * Creates the pixel shader from assembled bytecode.
		 *
		 * \param _pdwByteCode Pointer to assembled shader bytecode (DWORD stream as returned by D3DXAssembleShader or fxc).
		 * \param _stDwords Number of DWORDs in _pdwByteCode.
		 * \return Returns true if the shader was created.
		 **/
		bool													CreateFromByteCode( const DWORD * _pdwByteCode, size_t _stDwords );

		/**
		 * Creates the pixel shader from ASCII assembly (ps_2_0, etc.). The assembly text is cached so the
		 * shader can be recreated on device reset.
		 *
		 * \param _pszAsm Null-terminated ASCII assembly string.
		 * \return Returns true if assembled and created successfully.
		 **/
		bool													CreateFromAsm( const char * _pszAsm );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline IDirect3DPixelShader9 *							Get() { return m_pd3dps9Shader; }

		/**
		 * \brief Determines whether this pixel shader currently wraps a valid D3D9 shader.
		 * \return Returns true if the shader exists.
		 */
		inline bool												Valid() const { return m_pd3dps9Shader != nullptr; }


	protected :
		// == Members.
		/** The actual Direct3D 9 pixel shader. */
		IDirect3DPixelShader9 *									m_pd3dps9Shader = nullptr;
		/** A pointer to the Direct3D 9 device. */
		CDirectX9Device *										m_pdx9dDevice = nullptr;

		/** Cached bytecode for reset path (DWORDs). */
		std::vector<DWORD>										m_vByteCode;
		/** Optional cached ASCII assembly for reset path. */
		std::vector<char>										m_vAsm;

	private :
		/**
		 * \brief Creates the pixel shader from assembled bytecode.
		 * 
		 * \param _pdwByteCode Pointer to DWORD stream of assembled shader.
		 * \param _stDwords Number of DWORDs in \p _pdwByteCode.
		 * \return Returns true if the shader was created.
		 */
		bool													CreateInternalFromByteCode( const DWORD * _pdwByteCode, size_t _stDwords );

		/**
		 * \brief Assembles ASCII shader text and creates the pixel shader.
		 * 
		 * \param _pszAsm Null-terminated ASCII assembly (e.g., ps_2_0).
		 * \return Returns true if assembly and creation both succeeded.
		 */
		bool													AssembleAndCreate( const char * /*_pszAsm*/ );
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
