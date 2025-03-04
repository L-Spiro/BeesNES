/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A class for opening files using the standard C library (FILE).
 */


#include "LSNStdFile.h"

namespace lsn {

	CStdFile::CStdFile() :
		m_pfFile( nullptr ),
		m_ui64Size( 0 ) {
	}
	CStdFile::~CStdFile() {
		Close();
	}

	// == Functions.
#ifdef LSN_WINDOWS
	/**
	 * Opens a file.  The path is given in UTF-16.
	 *
	 * \param _pcPath Path to the file to open.
	 * \return Returns true if the file was opened, false otherwise.
	 */
	bool CStdFile::Open( const char16_t * _pcFile ) {
		Close();

		FILE * pfFile = nullptr;
		errno_t enOpenResult = ::_wfopen_s( &pfFile, reinterpret_cast<const wchar_t *>(_pcFile), L"rb" );
		if ( nullptr == pfFile || enOpenResult != 0 ) { return false; }

		::_fseeki64( pfFile, 0, SEEK_END );
		m_ui64Size = ::_ftelli64( pfFile );
		std::rewind( pfFile );

		m_pfFile = pfFile;
		PostLoad();
		return true;
	}

	/**
	 * Creates a file.  The path is given in UTF-16.
	 *
	 * \param _pcPath Path to the file to create.
	 * \return Returns true if the file was created, false otherwise.
	 */
	bool CStdFile::Create( const char16_t * _pcFile ) {
		Close();

		FILE * pfFile = nullptr;
		errno_t enOpenResult = ::_wfopen_s( &pfFile, reinterpret_cast<const wchar_t *>(_pcFile), L"wb" );
		if ( nullptr == pfFile || enOpenResult != 0 ) { return false; }

		m_ui64Size = 0;

		m_pfFile = pfFile;
		PostLoad();
		return true;
	}
#else
	/**
	 * Opens a file.  The path is given in UTF-8.
	 *
	 * \param _pcPath Path to the file to open.
	 * \return Returns true if the file was opened, false otherwise.
	 */
	bool CStdFile::Open( const char8_t * _pcFile ) {
		FILE * pfFile = std::fopen( reinterpret_cast<const char *>(_pcFile), "rb" );
		if ( nullptr == pfFile ) { return false; }

		std::fseek( pfFile, 0, SEEK_END );
		m_ui64Size = std::ftell( pfFile );
		std::rewind( pfFile );

		m_pfFile = pfFile;
		PostLoad();
		return true;
	}

	/**
	 * Creates a file.  The path is given in UTF-8.
	 *
	 * \param _pcPath Path to the file to create.
	 * \return Returns true if the file was created, false otherwise.
	 */
	bool CStdFile::Create( const char8_t * _pcFile ) {
		FILE * pfFile = std::fopen( reinterpret_cast<const char *>(_pcFile), "wb" );
		if ( nullptr == pfFile ) { return false; }

		m_ui64Size = 0;

		m_pfFile = pfFile;
		PostLoad();
		return true;
	}
#endif	// #ifdef LSN_WINDOWS

	/**
	 * Closes the opened file.
	 */
	void CStdFile::Close() {
		if LSN_LIKELY( m_pfFile != nullptr ) {
			::fclose( m_pfFile );
			m_pfFile = nullptr;
			m_ui64Size = 0;
		}
	}

	/**
	 * Loads the opened file to memory, storing the result in _vResult.
	 *
	 * \param _vResult The location where to store the file in memory.
	 * \return Returns true if the file was successfully loaded into memory.
	 */
	bool CStdFile::LoadToMemory( std::vector<uint8_t> &_vResult ) const {
		if LSN_LIKELY( m_pfFile != nullptr ) {
#ifdef LSN_WINDOWS
			__int64 i64Pos = ::_ftelli64( m_pfFile );
			::_fseeki64( m_pfFile, 0, SEEK_END );
			__int64 i64Len = ::_ftelli64( m_pfFile );
			std::rewind( m_pfFile );
			try {
				_vResult.resize( size_t( i64Len ) );
			}
			catch ( ... ) {
				::_fseeki64( m_pfFile, i64Pos, SEEK_SET );
				return false;
			}
			if ( __int64( _vResult.size() ) != i64Len ) {
				::_fseeki64( m_pfFile, i64Pos, SEEK_SET );
				return false;
			}
			if ( std::fread( _vResult.data(), _vResult.size(), 1, m_pfFile ) != 1 ) {
				::_fseeki64( m_pfFile, i64Pos, SEEK_SET );
				return false;
			}
			::_fseeki64( m_pfFile, i64Pos, SEEK_SET );
#else
			long lPos = std::ftell( m_pfFile );
			std::fseek( m_pfFile, 0, SEEK_END );
			long lLen = std::ftell( m_pfFile );
			std::rewind( m_pfFile );
			try {
				_vResult.resize( size_t( lLen ) );
			}
			catch ( ... ) {
				std::fseek( m_pfFile, lPos, SEEK_SET );
				return false;
			}
			if ( _vResult.size() != lLen ) {
				std::fseek( m_pfFile, lPos, SEEK_SET );
				return false;
			}
			if ( std::fread( _vResult.data(), _vResult.size(), 1, m_pfFile ) != 1 ) {
				std::fseek( m_pfFile, lPos, SEEK_SET );
				return false;
			}
			std::fseek( m_pfFile, lPos, SEEK_SET );
#endif	// #ifdef LSN_WINDOWS
			return true;
		}
		return false;
	}

	/**
	 * Writes the given data to the created file.  File must have been cerated with Create().
	 *
	 * \param _vData The data to write to the file.
	 * \return Returns true if the data was successfully written to the file.
	 */
	bool CStdFile::WriteToFile( const std::vector<uint8_t> &_vData ) {
		return WriteToFile( _vData.data(), _vData.size() );
	}

	/**
	 * Gets the current position inside the file.
	 * 
	 * \return Returns the current position inside the file.
	 **/
	uint64_t CStdFile::GetPos() const {
#ifdef LSN_WINDOWS
		return ::_ftelli64( m_pfFile );
#else
		return uint64_t( std::ftell( m_pfFile ) );
#endif	// #ifdef LSN_WINDOWS
	}

	/**
	 * Writes the given data to the created file.  File must have been cerated with Create().
	 *
	 * \param _pui8Data The data to write to the file.
	 * \param _tsSize The size of the buffer to which _pui8Data points.
	 * \return Returns true if the data was successfully written to the file.
	 */
	bool CStdFile::WriteToFile( const uint8_t * _pui8Data, size_t _tsSize ) {
		if LSN_LIKELY( m_pfFile != nullptr ) {
			return std::fwrite( _pui8Data, _tsSize, 1, m_pfFile ) == 1;
		}
		return false;
	}

	/**
	 * Moves the file pointer from the current position and returns the new position.
	 * 
	 * \param _i64Offset Amount by which to move the file pointer.
	 * \return Returns the new line position.
	 **/
	uint64_t CStdFile::MovePointerBy( int64_t _i64Offset ) const {
#ifdef LSN_WINDOWS
		::_fseeki64( m_pfFile, _i64Offset, SEEK_CUR );
		return ::_ftelli64( m_pfFile );
#else
		std::fseek( m_pfFile, static_cast<long>(_i64Offset), SEEK_CUR );
		return uint64_t( std::ftell( m_pfFile ) );
#endif	// #ifdef LSN_WINDOWS
	}

	/**
	 * Moves the file pointer to the given file position.
	 * 
	 * \param _ui64Pos The new file position to set.
	 * \param _bFromEnd Whether _ui64Pos is from the end of the file or not. 
	 * \return Returns the new file position.
	 **/
	uint64_t CStdFile::MovePointerTo( uint64_t _ui64Pos, bool _bFromEnd ) const {
#ifdef LSN_WINDOWS
		::_fseeki64( m_pfFile, static_cast<long long>(_ui64Pos), _bFromEnd ? SEEK_END : SEEK_SET );
		return ::_ftelli64( m_pfFile );
#else
		::fseeko( m_pfFile, static_cast<off_t>(_ui64Pos), _bFromEnd ? SEEK_END : SEEK_SET );
		return ::ftello( m_pfFile );
#endif	// #ifdef LSN_WINDOWS
	}

	/**
	 * Performs post-loading operations after a successful loading of the file.  m_pfFile will be valid when this is called.  Override to perform additional loading operations on m_pfFile.
	 */
	void CStdFile::PostLoad() {}

}	// namespace lsn
