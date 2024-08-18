/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for index buffers.
 */


#pragma once

#include "../LSNLSpiroNes.h"

#include <vector>

namespace lsn {

	/**
	 * Class CIndexBufferBase
	 * \brief The base class for index buffers.
	 *
	 * Description: The base class for index buffers.
	 */
	class CIndexBufferBase : public std::vector<uint8_t>{
	public :
		CIndexBufferBase();
		~CIndexBufferBase();


		// == Functions.
		/**
		 * Resets the object instance back to scratch, ready to be reused.
		 **/
		virtual void											Reset();

		/**
		 * Gets the total number of indices.
		 *
		 * \return Returns the total number of indices.
		 **/
		inline uint32_t											TotalIndices() const { m_ui32TotalIndices; }

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
		bool													Alloc( uint32_t _ui32TotalIndices, uint32_t _ui32Pool, uint32_t _ui32Usage, uint32_t _ui32Format, size_t _sAllocSize = 0 );


	protected :
		// == Members.
		uint32_t												m_ui32TotalIndices = 0;						/**< Total indices. */
		uint32_t												m_ui32MemoryPool = 0;						/**< The memory pool. */
		uint32_t												m_ui32Usage = 0;							/**< Index-buffer usage. */
		uint32_t												m_ui32Format = 0;							/**< The index-buffer format. */
	};

}	// namespace lsn
