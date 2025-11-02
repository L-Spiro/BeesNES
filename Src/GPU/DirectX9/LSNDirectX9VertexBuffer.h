/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A Direct3D 9 vertex buffer.
 */


#ifdef LSN_DX9

#pragma once

#include "../../LSNLSpiroNes.h"
#include "../LSNVertexBufferBase.h"
#include "LSNDirectX9.h"
#include "LSNDirectX9Device.h"
#include "LSNDirectX9LosableResource.h"

namespace lsn {

	/**
	 * Class CDirectX9VertexBuffer
	 * \brief A Direct3D 9 vertex buffer.
	 *
	 * Description: A Direct3D 9 vertex buffer.
	 */
	class CDirectX9VertexBuffer : public CDirectX9LosableResource, public CVertexBufferBase {
	public :
		// == Various constructors.
		CDirectX9VertexBuffer( CDirectX9Device * _pdx9dDevice );
		virtual ~CDirectX9VertexBuffer();


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
		 * Creates a vertex buffer.
		 * 
		 * \param _uiLength Size of the vertex buffer, in bytes. For FVF vertex buffers, Length must be large enough to contain at least one vertex, but it need not be a multiple of the vertex size. Length is not validated for non-FVF buffers.
		 * \param _dwUsage Usage can be 0, which indicates no usage value. However, if usage is desired, use a combination of one or more D3DUSAGE constants. It is good practice to match the usage parameter in CreateVertexBuffer with the behavior flags in IDirect3D9::CreateDevice.
		 * \param _dwFVF Combination of D3DFVF, a usage specifier that describes the vertex format of the vertices in this buffer. If this parameter is set to a valid FVF code, the created vertex buffer is an FVF vertex buffer (see Remarks). Otherwise, if this parameter is set to zero, the vertex buffer is a non-FVF vertex buffer.
		 * \param _pPool Member of the D3DPOOL enumerated type, describing a valid memory class into which to place the resource. Do not set to D3DPOOL_SCRATCH.
		 * \return Returns true if the vertex buffer was created.
		 **/
		bool													CreateVertexBuffer(
			UINT _uiLength,
			DWORD _dwUsage,
			DWORD _dwFVF,
			D3DPOOL _pPool );

		/**
		 * Gets the wrapped object.
		 *
		 * \return Returns a pointer to the wrapped object.
		 **/
		inline IDirect3DVertexBuffer9 *							Get() { return m_pd3dvb9Buffer; }

		/**
		 * \brief Determines whether this vertex buffer currently wraps a valid D3D9 buffer.
		 * 
		 * \return Returns true if the vertex buffer exists.
		 */
		inline bool												Valid() const { return m_pd3dvb9Buffer != nullptr; }

		/**
		 * \brief Locks the vertex buffer.
		 *
		 * \param _uiOffset Offset in bytes from the start of the buffer to the range to lock.
		 * \param _uiSize Size (in bytes) of the range to lock; 0 locks to end.
		 * \param _ppvData Returns a pointer to the locked memory.
		 * \param _dwFlags Combination of LSN_D3DLOCK flags (e.g., LSN_D3DLOCK_DISCARD).
		 * \return Returns true on success.
		 */
		inline bool												Lock( UINT _uiOffset, UINT _uiSize, void ** _ppvData, DWORD _dwFlags ) {
			if LSN_UNLIKELY( !m_pd3dvb9Buffer ) { return false; }
			return SUCCEEDED( m_pd3dvb9Buffer->Lock( _uiOffset, _uiSize, _ppvData, _dwFlags ) );
		}

		/**
		 * \brief Unlocks the vertex buffer.
		 */
		inline void												Unlock() {
			if LSN_UNLIKELY( m_pd3dvb9Buffer ) {
				m_pd3dvb9Buffer->Unlock();
			}
		}



	protected :
		// == Members.
		/** The actual DirectX 9 texture. */
		IDirect3DVertexBuffer9 *								m_pd3dvb9Buffer;
		/** A pointer to the Direct3D 9 device. */
		CDirectX9Device *										m_pdx9dDevice;

	private :
		typedef CVertexBufferBase								Parent;
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX9
