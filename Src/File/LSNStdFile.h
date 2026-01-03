/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A class for opening files using the standard C library (FILE).
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../OS/LSNOs.h"
#include "LSNFileBase.h"

namespace lsn {

	/**
	 * Class CStdFile
	 * \brief A class for opening files using the standard C library (FILE).
	 *
	 * Description: A class for opening files using the standard C library (FILE).
	 */
	class CStdFile : public CFileBase {
	public :
		CStdFile();
		virtual ~CStdFile();


		// == Functions.
		/**
		 * Opens a file.
		 *
		 * \param _pFile Path to the file to open.
		 * \return Returns true if the file was opened, false otherwise.
		 */
		virtual bool										Open( const std::filesystem::path &_pFile ) override;

		/**
		 * Creates a file.
		 *
		 * \param _pFile Path to the file to create.
		 * \return Returns true if the file was created, false otherwise.
		 */
		virtual bool										Create( const std::filesystem::path &_pFile ) override;

		/**
		 * Opens a file for appending.  If it does not exist it is created.
		 *
		 * \param _pFile Path to the file to open for appending.
		 * \return Returns true if the given file was opened for appending.
		 */
		virtual bool										Append( const std::filesystem::path &_pFile ) override;

		/**
		 * Closes the opened file.
		 */
		virtual void										Close()override;

		/**
		 * Determines whether the file is open for read or write or not.
		 * 
		 * \return returns true if the file is open, false otherwise.
		 **/
		virtual bool										IsOpen() const { return m_pfFile != NULL; }

		/**
		 * Loads the opened file to memory, storing the result in _vResult.
		 *
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was successfully loaded into memory.
		 */
		virtual bool										LoadToMemory( std::vector<uint8_t> &_vResult ) const override;

		/**
		 * Reads from the file.
		 * 
		 * \param _pvDst The destination for the read.  Must be sized appropriately to contain _sSize bytes.
		 * \param _sSize The number of bytes to read.
		 * \return Returns true if the read succeeded.  The file must be opened for read and the read operation must not extend beyond the end of the file.
		 **/
		virtual bool										Read( void * _pvDst, size_t _sSize ) override;

		/**
		 * Writes the given data to the created file.  File must have been cerated with Create().
		 *
		 * \param _vData The data to write to the file.
		 * \return Returns true if the data was successfully written to the file.
		 */
		virtual bool										WriteToFile( const std::vector<uint8_t> &_vData );

		/**
		 * Writes the given data to the created file.  File must have been cerated with Create().
		 *
		 * \param _pui8Data The data to write to the file.
		 * \param _sSize The size of the buffer to which _pui8Data points.
		 * \return Returns true if the data was successfully written to the file.
		 */
		virtual bool										WriteToFile( const uint8_t * _pui8Data, size_t _sSize );

		/**
		 * Writes to the file.
		 * 
		 * \param _pvSrc The source for the write.
		 * \param _sSize The number of bytes to write.
		 * \return Returns true if the write succeeded.  The file must be opened for write and there must be enough room to extend the file size.
		 **/
		virtual bool										Write( const void * _pvSrc, size_t _sSize ) override { return WriteToFile( static_cast<const uint8_t *>(_pvSrc), _sSize ); }

		/**
		 * Writes arbitrarily typed data to the created file.
		 * 
		 * \param _tVal The value to write.
		 * \return Returns true if the data was successfully written to the file.
		 **/
		template <typename _tType>
		inline bool											Write( const _tType &_tVal ) {
			return WriteToFile( reinterpret_cast<const uint8_t *>(&_tVal), sizeof( _tVal ) );
		}

		/**
		 * Writes an int32_t to the created file.
		 * 
		 * \param _i32Val The value to write.
		 * \return Returns true if the data was successfully written to the file.
		 **/
		inline bool											WriteUi32( int32_t _i32Val ) {
			return WriteToFile( reinterpret_cast<const uint8_t *>(&_i32Val), sizeof( _i32Val ) );
		}

		/**
		 * Writes a uint32_t to the created file.
		 * 
		 * \param _ui32Val The value to write.
		 * \return Returns true if the data was successfully written to the file.
		 **/
		inline bool											WriteUi32( uint32_t _ui32Val ) {
			return WriteToFile( reinterpret_cast<const uint8_t *>(&_ui32Val), sizeof( _ui32Val ) );
		}

		/**
		 * Gets the size of the file.
		 * 
		 * \return Returns the size of the file.
		 **/
		virtual uint64_t									Size() const override { return m_ui64Size; }

		/**
		 * Gets the current position inside the file.
		 * 
		 * \return Returns the current position inside the file.
		 **/
		virtual uint64_t									GetPos() const override;

		/**
		 * Moves the file pointer from the current position and returns the new position.
		 * 
		 * \param _i64Offset Amount by which to move the file pointer.
		 * \return Returns the new line position.
		 **/
		virtual uint64_t									MovePointerBy( int64_t _i64Offset ) const override;

		/**
		 * Moves the file pointer to the given file position.
		 * 
		 * \param _ui64Pos The new file position to set.
		 * \param _bFromEnd Whether _ui64Pos is from the end of the file or not. 
		 * \return Returns the new file position.
		 **/
		virtual uint64_t									MovePointerTo( uint64_t _ui64Pos, bool _bFromEnd = false ) const override;

		/**
		 * Loads the opened file to memory, storing the result in _vResult.
		 *
		 * \param _pcFile The file to open.
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was successfully loaded into memory.
		 **/
		template <typename _tStrType>
		static inline bool									LoadToMemory( const _tStrType * _pcFile, std::vector<uint8_t> &_vResult );

		/**
		 * Writes the given data to the created file.
		 *
		 * \param _pcFile The file to create.
		 * \param _pui8Data The data to write to the file.
		 * \param _sSize The size of the buffer to which _pui8Data points.
		 * \return Returns true if the data was successfully written to the file.
		 */
		template <typename _tStrType>
		static inline bool									WriteToFile( const _tStrType * _pcFile, const uint8_t * _pui8Data, size_t _sSize );

		/**
		 * Writes the given data to the created file.
		 *
		 * \param _pcFile The file to create.
		 * \param _vData The data to write to the file.
		 * \return Returns true if the data was successfully written to the file.
		 */
		template <typename _tStrType>
		static inline bool									WriteToFile( const _tStrType * _pcFile, const std::vector<uint8_t> &_vData );


	protected :
		// == Members.
		FILE *												m_pfFile;							/**< The FILE object to maintain. */
		uint64_t											m_ui64Size;							/**< The file size. */


		// == Functions.
		/**
		 * Performs post-loading operations after a successful loading of the file.  m_pfFile will be valid when this is called.  Override to perform additional loading operations on m_pfFile.
		 */
		virtual void										PostLoad();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Loads the opened file to memory, storing the result in _vResult.
	 *
	 * \param _pcFile The file to open.
	 * \param _vResult The location where to store the file in memory.
	 * \return Returns true if the file was successfully loaded into memory.
	 **/
	template <typename _tStrType>
	inline bool CStdFile::LoadToMemory( const _tStrType * _pcFile, std::vector<uint8_t> &_vResult ) {
		CStdFile sfFile;
		if ( !sfFile.Open( _pcFile ) ) { return false; }
		return sfFile.LoadToMemory( _vResult );
	}

	/**
	 * Writes the given data to the created file.
	 *
	 * \param _pcFile The file to create.
	 * \param _pui8Data The data to write to the file.
	 * \param _sSize The size of the buffer to which _pui8Data points.
	 * \return Returns true if the data was successfully written to the file.
	 */
	template <typename _tStrType>
	inline bool CStdFile::WriteToFile( const _tStrType * _pcFile, const uint8_t * _pui8Data, size_t _sSize ) {
		CStdFile sfFile;
		if ( !sfFile.Create( _pcFile ) ) { return false; }
		return sfFile.WriteToFile( _pui8Data, _sSize );
	}

	/**
	 * Writes the given data to the created file.
	 *
	 * \param _pcFile The file to create.
	 * \param _vData The data to write to the file.
	 * \return Returns true if the data was successfully written to the file.
	 */
	template <typename _tStrType>
	inline bool CStdFile::WriteToFile( const _tStrType * _pcFile, const std::vector<uint8_t> &_vData ) {
		return WriteToFile( _pcFile, _vData.data(), _vData.size() );
	}

}	// namespace lsn
