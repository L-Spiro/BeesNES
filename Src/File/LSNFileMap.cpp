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

#ifdef LSN_USE_WINDOWS
		if ( !m_hFile.Valid() ) {
#else
		if ( !(m_hFile && m_hFile != INVALID_HANDLE_VALUE) ) {
#endif	// #ifdef LSN_USE_WINDOWS
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

#ifdef LSN_USE_WINDOWS
		if ( !m_hFile.Valid() ) {
#else
		if ( !(m_hFile && m_hFile != INVALID_HANDLE_VALUE) ) {
#endif	// #ifdef LSN_USE_WINDOWS
			Close();
			return false;
		}
		m_bWritable = true;


		LARGE_INTEGER largeSize;
		largeSize.QuadPart = 4 * 1024;
		if ( !::SetFilePointerEx(
#ifdef LSN_USE_WINDOWS
			m_hFile.hHandle
#else
			m_hFile
#endif	// #ifdef LSN_USE_WINDOWS
			, largeSize, NULL, FILE_BEGIN ) || !::SetEndOfFile(
#ifdef LSN_USE_WINDOWS
			m_hFile.hHandle
#else
			m_hFile
#endif	// #ifdef LSN_USE_WINDOWS
			) ) {
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
#ifdef LSN_USE_WINDOWS
		m_hMap.Reset();
		m_hFile.Reset();
#else
		if ( (m_hMap && m_hMap != INVALID_HANDLE_VALUE) ) {
			::CloseHandle( m_hMap );
			m_hMap = NULL;
		}
		if ( (m_hFile && m_hFile != INVALID_HANDLE_VALUE) ) {
			::CloseHandle( m_hFile );
			m_hFile = NULL;
		}
#endif	// #ifdef LSN_USE_WINDOWS
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
			if ( ::GetFileSizeEx(
#ifdef LSN_USE_WINDOWS
				m_hFile.hHandle
#else
				m_hFile
#endif	// #ifdef LSN_USE_WINDOWS
				, &liInt ) ) { m_ui64Size = liInt.QuadPart; }
		}
		return m_ui64Size;
	}

	/**
	 * Creates the file map.
	 * 
	 * \return Returns true if the file mapping was successfully created.
	 **/
	bool CFileMap::CreateFileMap() {
#ifdef LSN_USE_WINDOWS
		if ( !m_hFile.Valid() ) {
#else
		if ( !(m_hFile && m_hFile != INVALID_HANDLE_VALUE) ) {
#endif	// #ifdef LSN_USE_WINDOWS
			return false;
		}
		// Can't open 0-sized files.  Emulate the successful mapping of such a file.
		m_bIsEmpty = Size() == 0;
		if ( m_bIsEmpty ) { return true; }
		m_hMap = ::CreateFileMappingW(
#ifdef LSN_USE_WINDOWS
			m_hFile.hHandle
#else
			m_hFile
#endif	// #ifdef LSN_USE_WINDOWS
			,
			NULL,
			m_bWritable ? PAGE_READWRITE : PAGE_READONLY,
			0,
			0,
			NULL );

#ifdef LSN_USE_WINDOWS
		if ( !m_hMap.Valid() ) {
#else
		if ( !(m_hMap && m_hMap != INVALID_HANDLE_VALUE) ) {
#endif	// #ifdef LSN_USE_WINDOWS
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
	 * \param _pcFile Path to the file to open.
	 * \return Returns true if the file was opened, false otherwise.
	 */
	bool CFileMap::Open( const char8_t * _pcFile ) {
		return false;
	}

	/**
	 * Creates a file.  The path is given in UTF-8.
	 *
	 * \param _pcFile Path to the file to create.
	 * \return Returns true if the file was created, false otherwise.
	 */
	bool CFileMap::Create( const char8_t * _pcFile ) {
		return false;
	}
#endif	// #ifdef LSN_WINDOWS

}	// namespace lsn
