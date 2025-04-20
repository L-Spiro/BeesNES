/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A very simple byte stream implementation.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Utilities/LSNUtilities.h"
#include "LSNStreamBase.h"

#include <vector>


namespace lsn {

	/**
	 * Class CStream
	 * \brief A very simple byte stream implementation.
	 *
	 * Description: A very simple byte stream implementation.
	 */
	class CStream : public CStreamBase {
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
		virtual bool								ReadUi8( uint8_t &_ui8Value ) const {
			return Read( _ui8Value );
		}

		/**
		 * Reads a uint16_t from the stream.
		 *
		 * \param _ui16Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadUi16( uint16_t &_ui16Value ) const {
			return Read( _ui16Value );
		}

		/**
		 * Reads a uint32_t from the stream.
		 *
		 * \param _ui32Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadUi32( uint32_t &_ui32Value ) const {
			return Read( _ui32Value );
		}

		/**
		 * Reads a uint64_t from the stream.
		 *
		 * \param _ui64Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadUi64( uint64_t &_ui64Value ) const {
			return Read( _ui64Value );
		}

		/**
		 * Reads an int8_t from the stream.
		 *
		 * \param _i8Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadI8( int8_t &_i8Value ) const {
			return Read( _i8Value );
		}

		/**
		 * Reads an int16_t from the stream.
		 *
		 * \param _i16Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadI16( int16_t &_i16Value ) const {
			return Read( _i16Value );
		}

		/**
		 * Reads an int32_t from the stream.
		 *
		 * \param _i32Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadI32( int32_t &_i32Value ) const {
			return Read( _i32Value );
		}

		/**
		 * Reads an int64_t from the stream.
		 *
		 * \param _i64Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadI64( int64_t &_i64Value ) const {
			return Read( _i64Value );
		}

		/**
		 * Reads an bool from the stream.
		 *
		 * \param _bValue Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadBool( bool &_bValue ) const {
			return Read<bool>( _bValue );
		}

		/**
		 * Reads a char string from the stream.
		 * 
		 * \param _sString Holds the return string.
		 * \return Returns true if there was enough space left in the stream to read the string.
		 **/
		virtual bool								ReadString( std::string &_sString ) const {
			_sString.clear();
			uint32_t ui32Len;
			if ( !Read( ui32Len ) ) { return false; }
			try {
				for ( size_t I = 0; I < ui32Len; ++I ) {
					uint8_t ui8Tmp;
					if ( !Read( ui8Tmp ) ) { return false; }
					_sString.push_back( char( ui8Tmp ) );
				}
			}
			catch( ... ) {
				return false;
			}
			return true;
		}

		/**
		 * Reads a u16char_t string from the stream.
		 * 
		 * \param _u16String Holds the return string.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		virtual bool								ReadStringU16( std::u16string &_u16String ) const {
			std::string sTmp;
			if ( !ReadString( sTmp ) ) { return false; }

			_u16String = CUtilities::Utf8ToUtf16( reinterpret_cast<const char8_t *>(sTmp.c_str()) );
			return true;
		}

		/**
		 * Reads a wchar_t string from the stream.
		 * 
		 * \param _wsString Holds the return string.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		virtual bool								ReadStringU16( std::wstring &_wsString ) const {
			std::string sTmp;
			if ( !ReadString( sTmp ) ) { return false; }

			auto aTmp = CUtilities::Utf8ToUtf16( reinterpret_cast<const char8_t *>(sTmp.c_str()) );
			_wsString = CUtilities::XStringToWString( aTmp.c_str(), aTmp.size() );
			return true;
		}

		/**
		 * Reads a line from the loaded memory.  Lines are terminated with \n.  \r characters are ignored.
		 * 
		 * \param _sString Holds the return string.
		 * \return Returns false if there are no characters left to read into the string.
		 **/
		virtual bool								ReadLine( std::string &_sString ) const {
			try {
				_sString.clear();
				if ( m_stPos >= m_vStream.size() ) { return false; }
				while ( m_stPos < m_vStream.size() ) {
					char cChar;
					if ( !Read<char>( cChar ) ) { return false; }
					if ( cChar == '\r' ) { continue; }
					if ( cChar == '\n' ) { return true; }
					_sString += cChar;
				}
			} catch ( ... ) {}
			return true;
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
		virtual bool								WriteUi8( uint8_t _ui8Value ) {
			return Write( _ui8Value );
		}

		/**
		 * Writes a uint16_t to the stream.
		 *
		 * \param _ui16Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteUi16( uint16_t _ui16Value ) {
			return Write( _ui16Value );
		}

		/**
		 * Writes a uint32_t to the stream.
		 *
		 * \param _ui32Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteUi32( uint32_t _ui32Value ) {
			return Write( _ui32Value );
		}

		/**
		 * Writes a uint64_t to the stream.
		 *
		 * \param _ui64Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteUi64( uint64_t _ui64Value ) {
			return Write( _ui64Value );
		}

		/**
		 * Writes an int8_t to the stream.
		 *
		 * \param _i8Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteI8( int8_t _i8Value ) {
			return Write( _i8Value );
		}

		/**
		 * Writes an int16_t to the stream.
		 *
		 * \param _i16Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteI16( int16_t _i16Value ) {
			return Write( _i16Value );
		}

		/**
		 * Writes an int32_t to the stream.
		 *
		 * \param _i32Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteI32( int32_t _i32Value ) {
			return Write( _i32Value );
		}

		/**
		 * Writes an int64_t to the stream.
		 *
		 * \param _i64Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteI64( int64_t _i64Value ) {
			return Write( _i64Value );
		}

		/**
		 * Writes an bool to the stream.
		 *
		 * \param _bValue The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteBool( bool _bValue ) {
			return Write<bool>( _bValue );
		}

		/**
		 * Writes a char string to the stream.
		 * 
		 * \param _sString The string to write.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		virtual bool								WriteString( const std::u8string &_sString ) {
			if constexpr ( sizeof( _sString.size() ) > sizeof( uint32_t ) ) {
				if ( _sString.size() & (~size_t( INT_MAX ) << 1) ) {
					if ( !WriteUi32( 0 ) ) { return false; }
					return true;
				}
			}
			if ( !Write( uint32_t( _sString.size() ) ) ) { return false; }
			for ( size_t I = 0; I < _sString.size(); ++I ) {
				if ( !WriteUi8( _sString[I] ) ) { return false; }
			}
			return true;
		}

		/**
		 * Writes a u16char_t string to the stream.
		 * 
		 * \param _u16String The string to write.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		virtual bool								WriteStringU16( const std::u16string &_u16String ) {
			bool bErrored = false;
			std::u8string sTmp = CUtilities::Utf16ToUtf8( _u16String.c_str(), &bErrored );
			if ( bErrored ) {
				return WriteString( std::u8string() );
			}
			return WriteString( sTmp );
		}

		/**
		 * Writes a u16char_t string to the stream.
		 * 
		 * \param _wsString The string to write.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		virtual bool								WriteStringU16( const std::wstring &_wsString ) {
			bool bErrored = false;
			std::u8string sTmp = CUtilities::Utf16ToUtf8( CUtilities::XStringToU16String( _wsString.c_str(), _wsString.size() ).c_str(), &bErrored );
			if ( bErrored ) {
				return WriteString( std::u8string() );
			}
			return WriteString( sTmp );
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
		bool										Read( _tType &_tValue ) const {
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
		bool										Write( _tType _tValue ) {
			if ( m_stPos + sizeof( _tType ) > m_vStream.size() ) {
				m_vStream.resize( m_stPos + sizeof( _tType ) );
			}
			(*reinterpret_cast<_tType *>(&m_vStream.data()[m_stPos])) = _tValue;
			m_stPos += sizeof( _tType );
			return true;
		}

		/**
		 * Reads data from the array.
		 * 
		 * \param _pui8Dst A pointer to the destination buffer.  If nullptr, _sSize bytes are skipped in the stream.
		 * \param _sSize The total number of bytes to copy or skip.
		 * \return Returns the total number of bytes copied.
		 **/
		virtual size_t								Read( uint8_t * _pui8Dst, size_t _sSize ) const {
			_sSize = std::min( _sSize, Remaining() );
			if ( _pui8Dst ) {
				std::memcpy( _pui8Dst, &m_vStream.data()[m_stPos], _sSize );
			}
			m_stPos += _sSize;
			return _sSize;
		}

		/**
		 * Writes data to the array.
		 * 
		 * \param _pui8Src A pointer to the source buffer.  If nullptr, 0's are written.
		 * \param _sSize The total number of bytes to write.
		 * \return Returns the total number of bytes written.
		 **/
		virtual size_t								Write( const uint8_t * _pui8Src, size_t _sSize ) {
			if ( m_stPos + _sSize > m_vStream.size() ) {
				m_vStream.resize( m_stPos + _sSize );
			}
			if ( _pui8Src ) {
				std::memcpy( &m_vStream.data()[m_stPos], _pui8Src, _sSize );
			}
			else {
				std::memset( &m_vStream.data()[m_stPos], 0, _sSize );
			}
			m_stPos += _sSize;
			return _sSize;
		}

		/**
		 * Moves the file pointer from the current position and returns the new position.
		 * 
		 * \param _i64Offset Amount by which to move the file pointer.
		 * \return Returns the new line position.
		 **/
		virtual uint64_t							MovePointerBy( int64_t _i64Offset ) const {
			int64_t i64New = int64_t( m_stPos ) + _i64Offset;
			m_stPos = static_cast<size_t>(std::clamp<int64_t>( i64New, 0, m_vStream.size() ));
			return m_stPos;
		}

		/**
		 * Moves the file pointer to the given file position.
		 * 
		 * \param _ui64Pos The new file position to set.
		 * \param _bFromEnd Whether _ui64Pos is from the end of the file or not. 
		 * \return Returns the new file position.
		 **/
		virtual uint64_t							MovePointerTo( uint64_t _ui64Pos, bool _bFromEnd = false ) const {
			int64_t i64New = _bFromEnd ? (m_vStream.size() - _ui64Pos) : _ui64Pos;
			m_stPos = static_cast<size_t>(std::clamp<int64_t>( i64New, 0, m_vStream.size() ));
			return m_stPos;
		}

		/**
		 * Gets the pointer to the current data in the stream.
		 *
		 * \return Returns a pointer to the current position in the vector.
		 **/
		virtual uint8_t *							Data() { return m_vStream.data() + m_stPos; }

		/**
		 * Gets the number of bytes remaining in the buffer.
		 *
		 * \return Returns the number of bytes remaining in the buffer.
		 **/
		virtual size_t								Remaining() const { return m_vStream.size() - m_stPos; }

		/**
		 * Gets the current position of the stream.
		 *
		 * \return Returns the current position of the stream.
		 **/
		virtual size_t								Pos() const { return m_stPos; }


	protected :
		// == Members.
		/** The vector object. */
		std::vector<uint8_t> &						m_vStream;
		/** Our position within the vector. */
		mutable size_t								m_stPos;
	};

}	// namespace lsn
