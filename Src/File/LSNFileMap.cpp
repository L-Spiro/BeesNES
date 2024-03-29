/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A file-mapping.
 */

#include "LSNFileMap.h"


namespace lsn {

#ifdef LSN_WINDOWS
	CFileMap::CFileMap() :
		m_hFile( INVALID_HANDLE_VALUE ),
		m_hMap( INVALID_HANDLE_VALUE ),
		m_pbMapBuffer( nullptr ),
		m_bIsEmpty( TRUE ),
		m_bWritable( TRUE ),
		m_ui64Size( 0 ),
		m_ui64MapStart( MAXUINT64 ),
		m_dwMapSize( 0 ) {
	}
	CFileMap::~CFileMap() {
		Close();
	}
#else
#endif	// #ifdef LSN_WINDOWS

	// == Functions.
#ifdef LSN_WINDOWS
	/**
	 * Opens a file.  The path is given in UTF-16.
	 *
	 * \param _pcPath Path to the file to open.
	 * \return Returns true if the file was opened, false otherwise.
	 */
	bool CFileMap::Open( const char16_t * _pcFile ) {
		Close();
		m_hFile = ::CreateFileW( reinterpret_cast<LPCWSTR>(_pcFile),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

		if ( !m_hFile.Valid() ) {
			Close();
			return false;
		}
		m_bWritable = true;
		return CreateFileMap();
	}

	/**
	 * Creates a file.  The path is given in UTF-16.
	 *
	 * \param _pcPath Path to the file to create.
	 * \return Returns true if the file was created, false otherwise.
	 */
	bool CFileMap::Create( const char16_t * _pcFile ) {
		Close();
		m_hFile = ::CreateFileW( reinterpret_cast<LPCWSTR>(_pcFile),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

		if ( !m_hFile.Valid() ) {
			Close();
			return false;
		}
		m_bWritable = true;


		LARGE_INTEGER largeSize;
		largeSize.QuadPart = 4 * 1024;
		if ( !::SetFilePointerEx( m_hFile.hHandle, largeSize, NULL, FILE_BEGIN ) || !::SetEndOfFile( m_hFile.hHandle ) ) {
			Close();
			return false;
		}

		return CreateFileMap();
	}

	/**
	 * Closes the opened file.
	 */
	void CFileMap::Close() {
		if ( m_pbMapBuffer ) {
			::UnmapViewOfFile( m_pbMapBuffer );
			m_pbMapBuffer = nullptr;
		}
		m_hMap.Reset();
		m_hFile.Reset();
		m_bIsEmpty = TRUE;
		m_ui64Size = 0;
		m_ui64MapStart = MAXUINT64;
		m_dwMapSize = 0;
	}

	/**
	 * Gets the size of the file.
	 * 
	 * \return Returns the size of the file.
	 **/
	uint64_t CFileMap::Size() const {
		if ( !m_ui64Size ) {
			LARGE_INTEGER liInt;
			if ( ::GetFileSizeEx( m_hFile.hHandle, &liInt ) ) { m_ui64Size = liInt.QuadPart; }
		}
		return m_ui64Size;
	}

	/**
	 * Creates the file map.
	 * 
	 * \return Returns true if the file mapping was successfully created.
	 **/
	bool CFileMap::CreateFileMap() {
		if ( !m_hFile.Valid() ) { return false; }
		// Can't open 0-sized files.  Emulate the successful mapping of such a file.
		m_bIsEmpty = Size() == 0;
		if ( m_bIsEmpty ) { return true; }
		m_hMap = ::CreateFileMappingW( m_hFile.hHandle,
			NULL,
			m_bWritable ? PAGE_READWRITE : PAGE_READONLY,
			0,
			0,
			NULL );
		if ( !m_hMap.Valid() ) {
			Close();
			return false;
		}
		m_ui64MapStart = MAXUINT64;
		m_dwMapSize = 0;
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
		return false;
	}

	/**
	 * Creates a file.  The path is given in UTF-8.
	 *
	 * \param _pcPath Path to the file to create.
	 * \return Returns true if the file was created, false otherwise.
	 */
	bool CStdFile::Create( const char8_t * _pcFile ) {
		return false;
	}
#endif	// #ifdef LSN_WINDOWS

	

}	// namespace lsn
