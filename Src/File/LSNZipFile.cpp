/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A class for working with ZIP files.
 */


#include "LSNZipFile.h"
#include "../Utilities/LSNUtilities.h"

namespace lsn {

	CZipFile::CZipFile() {
		std::memset( &m_zaArchive, 0, sizeof( m_zaArchive ) );
	}
	CZipFile::~CZipFile() {
		Close();
	}


	// == Functions.
	/**
	 * Closes the opened file.
	 */
	void CZipFile::Close() {
		if ( m_pfFile != nullptr ) {
			::mz_zip_reader_end( &m_zaArchive );
			std::memset( &m_zaArchive, 0, sizeof( m_zaArchive ) );
		}
		CStdFile::Close();
	}

	/**
	 * If true, the file is an archive containing more files.
	 *
	 * \return Returns true if the file is an archive, false otherwise.
	 */
	bool CZipFile::IsArchive() const { return m_zaArchive.m_zip_mode != MZ_ZIP_MODE_INVALID; }

	/**
	 * Gathers the file names in the archive into an array.
	 *
	 * \param _vResult The location where to store the file names.
	 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
	 */
	bool CZipFile::GatherArchiveFiles( std::vector<std::u16string> &_vResult ) const {
		if ( m_pfFile != nullptr ) {
			mz_uint uiTotal = ::mz_zip_reader_get_num_files( const_cast<mz_zip_archive *>(&m_zaArchive) );
			for ( mz_uint I = 0; I  < uiTotal; ++I ) {
				::mz_zip_archive_file_stat zafsStat;
				if ( !::mz_zip_reader_file_stat( const_cast<mz_zip_archive *>(&m_zaArchive), I, &zafsStat ) ) {
					return false;
				}
				_vResult.push_back( CUtilities::Utf8ToUtf16( reinterpret_cast<const char8_t *>(zafsStat.m_filename) ) );
			}
			return true;
		}
		return false;
	}

	/**
	 * Gathers the file names in the archive into an array.
	 *
	 * \param _s16File The name of the file to extract.
	 * \param _vResult The location where to store the file in memory.
	 * \return Returns true if the file was extracted successfully.
	 */
	bool CZipFile::ExtractToMemory( const std::u16string &_s16File, std::vector<uint8_t> &_vResult ) const {
		if ( m_pfFile != nullptr ) {
			bool bError;
			std::string sUtf8 = CUtilities::Utf16ToUtf8( _s16File.c_str(), &bError );
			if ( bError ) { return false; }
			size_t stSize;
			void * pvData = ::mz_zip_reader_extract_file_to_heap( const_cast<mz_zip_archive *>(&m_zaArchive), sUtf8.c_str(), &stSize, 0 );
			if ( pvData == nullptr ) { return false; }
			try {
				_vResult = std::vector<uint8_t>( static_cast<uint8_t *>(pvData), static_cast<uint8_t *>(pvData) + stSize );
			}
			catch ( ... ) {
				::mz_free( pvData );
				return false;
			}
			::mz_free( pvData );
			return true;
		}
		return false;
	}

	// == Functions.
	/**
	 * Performs post-loading operations after a successful loading of the file.  m_pfFile will be valid when this is called.  Override to perform additional loading operations on m_pfFile.
	 */
	void CZipFile::PostLoad() {
		::mz_zip_reader_init_cfile( &m_zaArchive, m_pfFile, m_ui64Size, 0 );
	}

}	// namespace lsn
