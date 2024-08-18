/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for index buffers.
 */


#include "LSNIndexBufferBase.h"

namespace lsn {

	CIndexBufferBase::CIndexBufferBase() {
	}
	CIndexBufferBase::~CIndexBufferBase() {
		Reset();
	}


	// == Functions.
	/**
	 * Resets the object instance back to scratch, ready to be reused.
	 **/
	void CIndexBufferBase::Reset() {
		clear();
		m_ui32TotalIndices = 0;
		m_ui32MemoryPool = 0;
		m_ui32Usage = 0;
		m_ui32Format = 0;
	}

	/**
	 * Stores the given parameters and optionally allocates space for a copy of the vertices.
	 * 
	 * \param _ui32TotalIndices The total number of indices in the buffer.
	 * \param _ui32Pool The memory pool for the buffer.
	 * \param _ui32Usage The usage of the buffer.
	 * \param _ui32Format The format of the buffer.
	 * \param _sAllocSize The size to allocate for a copy.
	 * \return Returns false if _sAllocSize is non-zero and allocation fails.  Otherwise returns true.
	 **/
	bool CIndexBufferBase::Alloc( uint32_t _ui32TotalIndices, uint32_t _ui32Pool, uint32_t _ui32Usage, uint32_t _ui32Format, size_t _sAllocSize ) {
		if ( _sAllocSize ) {
			try {
				resize( _sAllocSize );
			}
			catch ( ... ) { return false; }
		}
		m_ui32TotalIndices = _ui32TotalIndices;
		m_ui32MemoryPool = _ui32Pool;
		m_ui32Usage = _ui32Usage;
		m_ui32Format = _ui32Format;
		return true;
	}

}	// namespace lsn
