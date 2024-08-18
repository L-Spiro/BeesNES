/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D index buffer.
 */


#ifdef LSN_DX9

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../LSNIndexBufferBase.h"
#include "LSNDirectX9.h"
#include "LSNDirectX9Device.h"
#include "LSNDirectX9LosableResource.h"

namespace lsn {

	/**
	 * Class CDirectX9IndexBuffer
	 * \brief A Direct3D index buffer.
	 *
	 * Description: A Direct3D index buffer.
	 */
	class CDirectX9IndexBuffer : public CDirectX9LosableResource, public CIndexBufferBase {
	public :
		// == Various constructors.
		CDirectX9IndexBuffer( CDirectX9Device * _pdx9dDevice );
		~CDirectX9IndexBuffer();


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
		virtual void											Reset();

		/**
		 * Creates an index buffer.
		 * 
		 * \param _uiLength Size of the index buffer, in bytes.
		 * \param _dwUsage Usage can be 0, which indicates no usage value. However, if usage is desired, use a combination of one or more D3DUSAGE constants. It is good practice to match the usage parameter in CreateIndexBuffer with the behavior flags in IDirect3D9::CreateDevice.
		 * \param _fFormat Member of the D3DFORMAT enumerated type, describing the format of the index buffer.
		 * \param _pPool Member of the D3DPOOL enumerated type, describing a valid memory class into which to place the resource.
		 * \return Returns true if the index buffer was created.
		 **/
		bool													CreateIndexBuffer(
			UINT _uiLength,
			DWORD _dwUsage,
			D3DFORMAT _fFormat,
			D3DPOOL _pPool );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline IDirect3DIndexBuffer9 *							Get() { return m_pd3dib9Buffer; }


	protected :
		// == Members.
		/** The actual DirectX 9 texture. */
		IDirect3DIndexBuffer9 *									m_pd3dib9Buffer;
		/** A pointer to the Direct3D 9 device. */
		CDirectX9Device *										m_pdx9dDevice;

	private :
		typedef CIndexBufferBase								Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
