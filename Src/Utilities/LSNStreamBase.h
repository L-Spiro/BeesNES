/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for streams.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Utilities/LSNUtilities.h"
#include "LSNStreamBase.h"

#include <vector>


namespace lsn {

	/**
	 * Class CStreamBase
	 * \brief The base class for streams.
	 *
	 * Description: The base class for streams.
	 */
	class CStreamBase {
	public :


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
		virtual bool								ReadUi8( uint8_t &/*_ui8Value*/ ) const { return false; }

		/**
		 * Reads a uint16_t from the stream.
		 *
		 * \param _ui16Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadUi16( uint16_t &/*_ui16Value*/ ) const { return false; }

		/**
		 * Reads a uint32_t from the stream.
		 *
		 * \param _ui32Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadUi32( uint32_t &/*_ui32Value*/ ) const { return false; }

		/**
		 * Reads a uint64_t from the stream.
		 *
		 * \param _ui64Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadUi64( uint64_t &/*_ui64Value*/ ) const { return false; }

		/**
		 * Reads an int8_t from the stream.
		 *
		 * \param _i8Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadI8( int8_t &/*_i8Value*/ ) const { return false; }

		/**
		 * Reads an int16_t from the stream.
		 *
		 * \param _i16Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadI16( int16_t &/*_i16Value*/ ) const { return false; }

		/**
		 * Reads an int32_t from the stream.
		 *
		 * \param _i32Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadI32( int32_t &/*_i32Value*/ ) const { return false; }

		/**
		 * Reads an int64_t from the stream.
		 *
		 * \param _i64Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadI64( int64_t &/*_i64Value*/ ) const { return false; }

		/**
		 * Reads an bool from the stream.
		 *
		 * \param _bValue Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		virtual bool								ReadBool( bool &/*_bValue*/ ) const { return false; }

		/**
		 * Reads a char string from the stream.
		 * 
		 * \param _sString Holds the return string.
		 * \return Returns true if there was enough space left in the stream to read the string.
		 **/
		virtual bool								ReadString( std::string &/*_sString*/ ) const { return false; }

		/**
		 * Reads a u16char_t string from the stream.
		 * 
		 * \param _u16String Holds the return string.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		virtual bool								ReadStringU16( std::u16string &/*_u16String*/ ) const { return false; }

		/**
		 * Reads a wchar_t string from the stream.
		 * 
		 * \param _wsString Holds the return string.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		virtual bool								ReadStringU16( std::wstring &/*_wsString*/ ) const { return false; }


		// ========
		// WRITING
		// ========
		/**
		 * Writes a uint8_t to the stream.
		 *
		 * \param _ui8Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteUi8( uint8_t /*_ui8Value*/ ) { return false; }

		/**
		 * Writes a uint16_t to the stream.
		 *
		 * \param _ui16Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteUi16( uint16_t /*_ui16Value*/ ) { return false; }

		/**
		 * Writes a uint32_t to the stream.
		 *
		 * \param _ui32Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteUi32( uint32_t /*_ui32Value*/ ) { return false; }

		/**
		 * Writes a uint64_t to the stream.
		 *
		 * \param _ui64Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteUi64( uint64_t /*_ui64Value*/ ) { return false; }

		/**
		 * Writes an int8_t to the stream.
		 *
		 * \param _i8Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteI8( int8_t /*_i8Value*/ ) { return false; }

		/**
		 * Writes an int16_t to the stream.
		 *
		 * \param _i16Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteI16( int16_t /*_i16Value*/ ) { return false; }

		/**
		 * Writes an int32_t to the stream.
		 *
		 * \param _i32Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteI32( int32_t /*_i32Value*/ ) { return false; }

		/**
		 * Writes an int64_t to the stream.
		 *
		 * \param _i64Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteI64( int64_t /*_i64Value*/ ) { return false; }

		/**
		 * Writes an bool to the stream.
		 *
		 * \param _bValue The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		virtual bool								WriteBool( bool /*_bValue*/ ) { return false; }

		/**
		 * Writes a char string to the stream.
		 * 
		 * \param _sString The string to write.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		virtual bool								WriteString( const std::u8string &/*_sString*/ ) { return false; }

		/**
		 * Writes a u16char_t string to the stream.
		 * 
		 * \param _u16String The string to write.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		virtual bool								WriteStringU16( const std::u16string &/*_u16String*/ ) { return false; }

		/**
		 * Writes a u16char_t string to the stream.
		 * 
		 * \param _wsString The string to write.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		virtual bool								WriteStringU16( const std::wstring &/*_wsString*/ ) { return false; }


		// ========
		// BASE
		// ========
		/**
		 * Reads data from the array.
		 * 
		 * \param _pui8Dst A pointer to the destination buffer.  If nullptr, _sSize bytes are skipped in the stream.
		 * \param _sSize The total number of bytes to copy or skip.
		 * \return Returns the total number of bytes copied.
		 **/
		virtual size_t								Read( uint8_t * /*_pui8Dst*/, size_t /*_sSize*/ ) const { return false; }

		/**
		 * Writes data to the array.
		 * 
		 * \param _pui8Src A pointer to the source buffer.  If nullptr, 0's are written.
		 * \param _sSize The total number of bytes to write.
		 * \return Returns the total number of bytes written.
		 **/
		virtual size_t								Write( const uint8_t * /*_pui8Src*/, size_t /*_sSize*/ ) { return false; }

		/**
		 * Moves the file pointer from the current position and returns the new position.
		 * 
		 * \param _i64Offset Amount by which to move the file pointer.
		 * \return Returns the new line position.
		 **/
		virtual uint64_t							MovePointerBy( int64_t /*_i64Offset*/ ) const { return 0; }

		/**
		 * Moves the file pointer to the given file position.
		 * 
		 * \param _ui64Pos The new file position to set.
		 * \param _bFromEnd Whether _ui64Pos is from the end of the file or not. 
		 * \return Returns the new file position.
		 **/
		virtual uint64_t							MovePointerTo( uint64_t /*_ui64Pos*/, bool /*_bFromEnd*/ = false ) const { return 0; }

		/**
		 * Gets the pointer to the current data in the stream.
		 *
		 * \return Returns a pointer to the current position in the vector.
		 **/
		inline uint8_t *							Data() { return nullptr; }

		/**
		 * Gets the number of bytes remaining in the buffer.
		 *
		 * \return Returns the number of bytes remaining in the buffer.
		 **/
		virtual size_t								Remaining() const { return 0; }

		/**
		 * Gets the current position of the stream.
		 *
		 * \return Returns the current position of the stream.
		 **/
		virtual size_t								Pos() const { return 0; }



	protected :
		// == Members.
	};

}	// namespace lsn
