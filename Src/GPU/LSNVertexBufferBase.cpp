/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for vertex buffers.
 */


#include "LSNVertexBufferBase.h"

namespace lsn {

	CVertexBufferBase::CVertexBufferBase() {
	}
	CVertexBufferBase::~CVertexBufferBase() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object instance back to scratch, ready to be reused.
	 **/
	void CVertexBufferBase::Reset() {
		clear();
		m_ui32Size = 0;
		m_ui32MemoryPool = 0;
		m_ui32Usage = 0;
		m_ui32Format = 0;
	}

	/**
	 * Stores the given parameters and optionally allocates space for a copy of the vertices.
	 * 
	 * \param _ui32Size The byte size of the buffer.
	 * \param _ui32Pool The memory pool for the buffer.
	 * \param _ui32Usage The usage of the buffer.
	 * \param _ui32Format The format of the buffer.
	 * \param _bAllocSize If true, a copy buffer is allocated.
	 * \return Returns false if _bAllocSize is true and allocation fails.  Otherwise returns true.
	 **/
	bool CVertexBufferBase::Alloc( uint32_t _ui32Size, uint32_t _ui32Pool, uint32_t _ui32Usage, uint32_t _ui32Format, bool _bAllocSize ) {
		if ( _bAllocSize ) {
			try {
				resize( _ui32Size );
			}
			catch ( ... ) { return false; }
		}
		m_ui32Size = _ui32Size;
		m_ui32MemoryPool = _ui32Pool;
		m_ui32Usage = _ui32Usage;
		m_ui32Format = _ui32Format;
		return true;
	}

}	// namespace lsn
