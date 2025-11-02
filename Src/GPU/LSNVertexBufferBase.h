/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for vertex buffers.
 */


#pragma once

#include "../LSNLSpiroNes.h"

#include <vector>

namespace lsn {

	/**
	 * Class CVertexBufferBase
	 * \brief The base class for vertex buffers.
	 *
	 * Description: The base class for vertex buffers.
	 */
	class CVertexBufferBase : public std::vector<uint8_t>{
	public :
		CVertexBufferBase();
		~CVertexBufferBase();


		// == Functions.
		/**
		 * Resets the object instance back to scratch, ready to be reused.
		 **/
		virtual void											Reset();

		/**
		 * Gets the size of the buffer in bytes.
		 *
		 * \return Returns the size of the buffer in bytes.
		 **/
		inline uint32_t											Size() const { return m_ui32Size; }

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
		bool													Alloc( uint32_t _ui32Size, uint32_t _ui32Pool, uint32_t _ui32Usage, uint32_t _ui32Format, bool _bAllocSize = false );


	protected :
		// == Members.
		uint32_t												m_ui32Size = 0;								/**< Size in bytes. */
		uint32_t												m_ui32MemoryPool = 0;						/**< The memory pool. */
		uint32_t												m_ui32Usage = 0;							/**< Vertex-buffer usage. */
		uint32_t												m_ui32Format = 0;							/**< The vertex-buffer format. */
	};

}	// namespace lsn
