/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A very simple byte stream implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <vector>


namespace lsn {

	/**
	 * Class CStream
	 * \brief A very simple byte stream implementation.
	 *
	 * Description: A very simple byte stream implementation.
	 */
	class CStream {
	public :
		CStream( std::vector<uint8_t> &_vStream ) :
			m_vStream( _vStream ),
			m_stPos( 0 ) {
		}


		// == Functions.
		// ========
		// READING
		// ========
		/**
		 * Reads a uint8_t from the stream.
		 *
		 * \param _ui8Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadUi8( uint8_t &_ui8Value ) {
			return Read( _ui8Value );
		}

		/**
		 * Reads a uint16_t from the stream.
		 *
		 * \param _ui16Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadUi16( uint16_t &_ui16Value ) {
			return Read( _ui16Value );
		}

		/**
		 * Reads a uint32_t from the stream.
		 *
		 * \param _ui32Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadUi32( uint32_t &_ui32Value ) {
			return Read( _ui32Value );
		}

		/**
		 * Reads a uint64_t from the stream.
		 *
		 * \param _ui64Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadUi64( uint64_t &_ui64Value ) {
			return Read( _ui64Value );
		}

		/**
		 * Reads an int8_t from the stream.
		 *
		 * \param _i8Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadI8( int8_t &_i8Value ) {
			return Read( _i8Value );
		}

		/**
		 * Reads an int16_t from the stream.
		 *
		 * \param _i16Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadI16( int16_t &_i16Value ) {
			return Read( _i16Value );
		}

		/**
		 * Reads an int32_t from the stream.
		 *
		 * \param _i32Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadI32( int32_t &_i32Value ) {
			return Read( _i32Value );
		}

		/**
		 * Reads an int64_t from the stream.
		 *
		 * \param _i64Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadI64( int64_t &_i64Value ) {
			return Read( _i64Value );
		}

		/**
		 * Reads an bool from the stream.
		 *
		 * \param _bValue Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadBool( bool &_bValue ) {
			return Read<bool>( _bValue );
		}


		// ========
		// WRITING
		// ========
		/**
		 * Writes a uint8_t to the stream.
		 *
		 * \param _ui8Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteUi8( uint8_t _ui8Value ) {
			return Write( _ui8Value );
		}

		/**
		 * Writes a uint16_t to the stream.
		 *
		 * \param _ui16Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteUi16( uint16_t _ui16Value ) {
			return Write( _ui16Value );
		}

		/**
		 * Writes a uint32_t to the stream.
		 *
		 * \param _ui32Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteUi32( uint32_t _ui32Value ) {
			return Write( _ui32Value );
		}

		/**
		 * Writes a uint64_t to the stream.
		 *
		 * \param _ui64Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteUi64( uint64_t _ui64Value ) {
			return Write( _ui64Value );
		}

		/**
		 * Writes an int8_t to the stream.
		 *
		 * \param _i8Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteI8( int8_t _i8Value ) {
			return Write( _i8Value );
		}

		/**
		 * Writes an int16_t to the stream.
		 *
		 * \param _i16Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteI16( int16_t _i16Value ) {
			return Write( _i16Value );
		}

		/**
		 * Writes an int32_t to the stream.
		 *
		 * \param _i32Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteI32( int32_t _i32Value ) {
			return Write( _i32Value );
		}

		/**
		 * Writes an int64_t to the stream.
		 *
		 * \param _i64Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteI64( int64_t _i64Value ) {
			return Write( _i64Value );
		}

		/**
		 * Writes an bool to the stream.
		 *
		 * \param _bValue The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteBool( bool _bValue ) {
			return Write<bool>( _bValue );
		}


		// ========
		// BASE
		// ========
		/**
		 * Reads a value of the given template type.
		 *
		 * \param _tValue Holds the return value if successful.
		 * \return Returns true if there was enough space left in the stream to read the given value.
		 */
		template <typename _tType>
		inline bool									Read( _tType &_tValue ) {
			if ( (m_vStream.size() - m_stPos) >= sizeof( _tType ) ) {
				_tValue = (*reinterpret_cast<_tType *>(&m_vStream.data()[m_stPos]));
				m_stPos += sizeof( _tType );
				return true;
			}
			return false;
		}

		/**
		 * Writes a value of the given template type.
		 *
		 * \param _tValue Holds the return value if successful.
		 * \return Returns true if there was enough space left in the stream to read the given value.
		 */
		template <typename _tType>
		inline bool									Write( _tType _tValue ) {
			if ( m_stPos + sizeof( _tType ) > m_vStream.size() ) {
				m_vStream.resize( m_stPos + sizeof( _tType ) );
			}
			(*reinterpret_cast<_tType *>(&m_vStream.data()[m_stPos])) = _tValue;
			m_stPos += sizeof( _tType );
			return true;
		}



	protected :
		// == Members.
		/** The vector object. */
		std::vector<uint8_t> &						m_vStream;
		/** Our position within the vector. */
		size_t										m_stPos;
	};

}	// namespace lsn
