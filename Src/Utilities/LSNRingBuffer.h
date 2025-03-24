/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A ring-buffer implementation.
 */


#pragma once

#include "../OS/LSNOs.h"
#include "LSNAlignmentAllocator.h"

#include <vector>


namespace lsn {

	/**
	 * Class CRingBuffer
	 * \brief A ring-buffer implementation.
	 *
	 * Description: A ring-buffer implementation.
	 */
	 template <typename _tnType>
	class CRingBuffer {
	public :
		CRingBuffer( size_t _sCapacity ) :
			m_stHead( 0 ),
			m_stTail( 0 ),
			m_stItems( 0 ) {
			m_vBuffer.resize( _sCapacity );
		}


		// == Functions.
		/**
		 * Adds an item to the ring buffer.  The item is always added, even if the buffer is full, but false is returned if the buffer is full before adding, indicating that the ring buffer
		 *	size should be increased.
		 * 
		 * \param _tnItem The item to add to the head of the ring buffer.
		 * \return Returns true the ring buffer was not already full before adding the item.
		 **/
		inline bool														Push( const _tnType &_tnItem ) {
			bool bFull = Full();
			m_vBuffer[m_stHead++] = _tnItem;
			m_stHead %= m_vBuffer.size();
			if LSN_UNLIKELY( bFull ) {
				m_stTail = (m_stTail + 1) % m_vBuffer.size();
				return false;
			}
			++m_stItems;
			return true;
		}

		/**
		 * Removes an item from the tail of the ring buffer.
		 * 
		 * \return Returns the popped object, which may be invalid if nothing has been pushed.
		 **/
		inline _tnType													Pop() {
			if LSN_UNLIKELY( !m_stItems ) { throw std::runtime_error( "Ring buffer empty." ); }

			_tnType * ptnReturnMe = &m_vBuffer[m_stTail++];
			m_stTail %= m_vBuffer.size();
			--m_stItems;
			return (*ptnReturnMe);
		}

		/**
		 * Removes an item from the tail of the ring buffer.
		 * 
		 * \return Returns a reference to the popped object, which may be invalid if nothing has been pushed.
		 **/
		inline _tnType &												Pop_Ref() {
			if LSN_UNLIKELY( !m_stItems ) { throw std::runtime_error( "Ring buffer empty." ); }

			_tnType * ptnReturnMe = &m_vBuffer[m_stTail++];
			m_stTail %= m_vBuffer.size();
			--m_stItems;
			return (*ptnReturnMe);
		}

		/**
		 * Like pop, but discards the item instead of returning it.
		 **/
		inline void														Pop_Discard() {
			if LSN_LIKELY( m_stItems ) {
				m_stTail = (m_stTail + 1) % m_vBuffer.size();
				--m_stItems;
			}
		}

		/**
		 * Determines if the ring buffer is full or not.
		 * 
		 * \return Returns true if the ring buffer is full.
		 **/
		inline bool														Full() const { return m_stItems == m_vBuffer.size(); }



	protected :
		// == Members.
		/** The vector object. */
		std::vector<_tnType, CAlignmentAllocator<_tnType, 64>>			m_vBuffer;
		/** The index where to push into the ring buffer. */
		size_t															m_stHead = 0;
		/** The index from where to pop from the ring buffer. */
		size_t															m_stTail = 0;
		/** Number of items. */
		size_t															m_stItems = 0;
	};

}	// namespace lsn
