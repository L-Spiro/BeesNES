/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A file-mapping.
 */

#include "LSNFileMap.h"


namespace lsn {

	CFileMap::CFileMap() :
		m_hFile( FileMap_Null ),
		m_hMap( FileMap_Null ),
		m_pbMapBuffer( nullptr ),
		m_bIsEmpty( TRUE ),
		m_bWritable( TRUE ),
		m_ui64Size( 0 ),
		m_ui64MapStart( std::numeric_limits<uint64_t>::max() ),
		m_ui32MapSize( 0 ) {
	}
	CFileMap::~CFileMap() {
		Close();
	}

	// == Functions.
#ifdef LSN_WINDOWS
	/**
	 * Opens a file.
	 *
	 * \param _pFile Path to the file to open.
	 * \return Returns true if the file was opened, false otherwise.
	 */
	bool CFileMap::Open( const std::filesystem::path &_pFile ) {
		Close();
		m_hFile = ::CreateFileW( _pFile.native().c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

		if ( !(m_hFile && m_hFile != FileMap_Null) ) {
			Close();
			return false;
		}
		m_bWritable = true;
		return CreateFileMap();
	}

	/**
	 * Creates a file.
	 *
	 * \param _pFile Path to the file to create.
	 * \return Returns true if the file was created, false otherwise.
	 */
	bool CFileMap::Create( const std::filesystem::path &_pFile ) {
		Close();
		m_hFile = ::CreateFileW( _pFile.native().c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

		if ( !(m_hFile && m_hFile != FileMap_Null) ) {
			Close();
			return false;
		}
		m_bWritable = true;


		LARGE_INTEGER largeSize;
		largeSize.QuadPart = 4 * 1024;
		if ( !::SetFilePointerEx( m_hFile, largeSize, NULL, FILE_BEGIN ) || !::SetEndOfFile( m_hFile ) ) {
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
		if ( (m_hMap && m_hMap != FileMap_Null) ) {
			::CloseHandle( m_hMap );
			m_hMap = NULL;
		}
		if ( (m_hFile && m_hFile != FileMap_Null) ) {
			::CloseHandle( m_hFile );
			m_hFile = NULL;
		}
		m_bIsEmpty = TRUE;
		m_ui64Size = 0;
		m_ui64MapStart = std::numeric_limits<uint64_t>::max();
		m_ui32MapSize = 0;
	}

	/**
	 * Gets the size of the file.
	 * 
	 * \return Returns the size of the file.
	 **/
	uint64_t CFileMap::Size() const {
		if ( !m_ui64Size ) {
			LARGE_INTEGER liInt;
			if ( ::GetFileSizeEx( m_hFile, &liInt ) ) { m_ui64Size = liInt.QuadPart; }
		}
		return m_ui64Size;
	}

	/**
	 * Creates the file map.
	 * 
	 * \return Returns true if the file mapping was successfully created.
	 **/
	bool CFileMap::CreateFileMap() {
		if ( !(m_hFile && m_hFile != FileMap_Null) ) {
			return false;
		}
		
		LARGE_INTEGER liInt;
		if ( !::GetFileSizeEx( m_hFile, &liInt ) ) {
			Close();
			return false;
		}
		m_ui64Size = static_cast<uint64_t>(liInt.QuadPart);

		// Can't open 0-sized files.  Emulate the successful mapping of such a file.
		m_bIsEmpty = Size() == 0;
		if ( m_bIsEmpty ) { return true; }
		m_hMap = ::CreateFileMappingW( m_hFile,
			NULL,
			m_bWritable ? PAGE_READWRITE : PAGE_READONLY,
			0,
			0,
			NULL );

		if ( NULL == m_hMap ) {
			Close();
			return false;
		}
		m_ui64MapStart = std::numeric_limits<uint64_t>::max();
		m_ui32MapSize = 0;
		return true;
	}
#else
	/**
	 * Opens a file.
	 *
	 * \param _pFile Path to the file to open.
	 * \return Returns true if the file was opened, false otherwise.
	 */
	bool CFileMap::Open( const std::filesystem::path &_pFile ) {
		Close();
		try {
			auto sPath = _pFile.native();
			m_hFile = ::open( sPath.c_str(), O_RDONLY );
			if ( m_hFile == FileMap_Null ) {
				Close();
				return false;
			}
			m_bWritable = false;
		}
		catch ( ... ) { return false; }		// _pFile.native() can fail if out of memory.

		return CreateFileMap();
	}

	/**
	 * Creates a file.
	 *
	 * \param _pFile Path to the file to create.
	 * \return Returns true if the file was created, false otherwise.
	 */
	bool CFileMap::Create( const std::filesystem::path &_pFile ) {
		Close();
		try {
			auto sPath = _pFile.native();
			m_hFile = ::open( sPath.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644 );
			if ( m_hFile == FileMap_Null ) {
				Close();
				return false;
			}
			m_bWritable = true;
		}
		catch ( ... ) { return false; }		// _pFile.native() can fail if out of memory.

		if ( ::ftruncate( m_hFile, 4 * 1024 ) != 0 ) {
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
			if ( m_ui32MapSize ) { ::munmap( m_pbMapBuffer, static_cast<size_t>(m_ui32MapSize) ); }
			m_pbMapBuffer = nullptr;
		}
		if ( m_hMap != FileMap_Null ) {
			::close( m_hMap );
			m_hMap = FileMap_Null;
		}
		if ( m_hFile != FileMap_Null ) {
			::close( m_hFile );
			m_hFile = FileMap_Null;
		}
		m_bIsEmpty = true;
		m_ui64Size = 0;
		m_ui64MapStart = std::numeric_limits<uint64_t>::max();
		m_ui32MapSize = 0;
	}

	/**
	 * Gets the size of the file.
	 *
	 * \return Returns the size of the file.
	 **/
	uint64_t FileMap::Size() const {
		if ( !m_ui64Size ) {
			struct stat sStat;
			if ( ::fstat( m_hFile, &sStat ) == 0 ) { m_ui64Size = static_cast<uint64_t>(sStat.st_size); }
		}
		return m_ui64Size;
	}

	/**
	 * Creates the file map.
	 *
	 * \return Returns true if the file mapping was successfully created.
	 **/
	bool CFileMap::CreateFileMap() {
		if ( m_hFile == FileMap_Null ) { return false; }
		// Can't open 0-sized files.
		m_bIsEmpty = Size() == 0;
		if ( m_bIsEmpty ) { return LSN_E_FILE_TOO_SMALL; }
		m_hMap = ::dup( m_hFile );
		if ( m_hMap == FileMap_Null ) {
			Close();
			return false;
		}
		m_ui64MapStart = std::numeric_limits<uint64_t>::max();
		m_ui32MapSize = 0;
		return true;
	}

#endif	// #ifdef LSN_WINDOWS

}	// namespace lsn
