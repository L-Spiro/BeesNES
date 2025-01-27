/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A class for working with ZIP files.
 */


#include "LSNZipFile.h"
#include "../Utilities/LSNUtilities.h"

#include <filesystem>

namespace lsn {

	CZipFile::CZipFile() {
		std::memset( &m_zaArchive, 0, sizeof( m_zaArchive ) );
	}
	CZipFile::~CZipFile() {
		Close();
	}


	// == Functions.
	/**
	 * Loads an in-memory file image.
	 * 
	 * \param _pvBuffer The file data to load.
	 * \param _sSize The number of bytes to which _pbBuffer points.
	 * \return Returns rtue if the file was loaded.
	 **/
	bool CZipFile::OpenMemory( const void * _pvBuffer, size_t _sSize ) {
		Close();
		if ( ::mz_zip_reader_init_mem( &m_zaArchive, _pvBuffer, _sSize, 0 ) ) {
			m_ui64Size = _sSize;
			return true;
		}
		return false;
	}

	/**
	 * Closes the opened file.
	 */
	void CZipFile::Close() {
		if ( m_pfFile != nullptr || m_zaArchive.m_archive_size != 0 ) {
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
		if ( m_pfFile != nullptr || m_zaArchive.m_archive_size != 0 ) {
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
	 * Gathers the file names of a specific type in the archive into an array.
	 *
	 * \param _vResult The location where to store the file names.
	 * \param _pcExt The extension of files to add to _vResult.
	 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
	 */
	bool CZipFile::GatherArchiveFiles( std::vector<std::u16string> &_vResult, const char16_t * _pcExt ) const {
		auto sExt = CUtilities::ToLower( std::u16string( _pcExt ) );
		if ( sExt.size() && sExt[0] == u'.' ) { sExt.erase( sExt.begin() ); }
		if ( m_pfFile != nullptr || m_zaArchive.m_archive_size != 0 ) {
			mz_uint uiTotal = ::mz_zip_reader_get_num_files( const_cast<mz_zip_archive *>(&m_zaArchive) );
			for ( mz_uint I = 0; I  < uiTotal; ++I ) {
				::mz_zip_archive_file_stat zafsStat;
				if ( !::mz_zip_reader_file_stat( const_cast<mz_zip_archive *>(&m_zaArchive), I, &zafsStat ) ) {
					return false;
				}
				auto aFileUtf16 = CUtilities::Utf8ToUtf16( reinterpret_cast<const char8_t *>(zafsStat.m_filename) );
				auto aTmp = CUtilities::ToLower( CUtilities::GetFileExtension( aFileUtf16 ) );
				if ( aTmp == sExt ) {
					try {
						_vResult.push_back( aFileUtf16 );
					}
					catch ( ... ) { return false; }
				}
			}
			return true;
		}
		return false;
	}

	/**
	 * Extracts a file into a memory buffer.
	 *
	 * \param _s16File The name of the file to extract.
	 * \param _vResult The location where to store the file in memory.
	 * \return Returns true if the file was extracted successfully.
	 */
	bool CZipFile::ExtractToMemory( const std::u16string &_s16File, std::vector<uint8_t> &_vResult ) const {
		if ( m_pfFile != nullptr || m_zaArchive.m_archive_size != 0 ) {
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

	/**
	 * Extracts all files of a given extension to buffers.
	 *
	 * \param _vResult The location where to store the file names.
	 * \param _vDataResult Holds the extracted files.
	 * \param _pcExt The extension of files to add to _vResult.
	 * \param _bExamineZipFiles If true, embedded ZIP files will also be extracted and recursively traversed.
	 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
	 */
	bool CZipFile::ExtractToMemory( std::vector<std::u16string> &_vResult, std::vector<std::vector<uint8_t>> &_vDataResult, const char16_t * _pcExt, uint32_t * _pui32FailedLoads, bool _bExamineZipFiles ) const {
		return ExtractToMemory( u"", u"", _vResult, _vDataResult, _pcExt, _pui32FailedLoads, _bExamineZipFiles );
	}

	/**
	 * Extracts all files of a given extension to buffers.
	 *
	 * \param _u16PathTo The path to this ZIP file.
	 * \param _u16PathAfter A postfix to append to the path.
	 * \param _vResult The location where to store the file names.
	 * \param _vDataResult Holds the extracted files.
	 * \param _pcExt The extension of files to add to _vResult.
	 * \param _bExamineZipFiles If true, embedded ZIP files will also be extracted and recursively traversed.
	 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
	 */
	bool CZipFile::ExtractToMemory( const std::u16string &_u16PathTo, const std::u16string &_u16PathAfter, std::vector<std::u16string> &_vResult, std::vector<std::vector<uint8_t>> &_vDataResult, const char16_t * _pcExt, uint32_t * _pui32FailedLoads, bool _bExamineZipFiles ) const{
		auto sExt = CUtilities::ToLower( std::u16string( _pcExt ) );
		if ( sExt.size() && sExt[0] == u'.' ) { sExt.erase( sExt.begin() ); }
		if ( m_pfFile != nullptr || m_zaArchive.m_archive_size != 0 ) {
			mz_uint uiTotal = ::mz_zip_reader_get_num_files( const_cast<mz_zip_archive *>(&m_zaArchive) );
			for ( mz_uint I = 0; I  < uiTotal; ++I ) {
				::mz_zip_archive_file_stat zafsStat;
				if ( !::mz_zip_reader_file_stat( const_cast<mz_zip_archive *>(&m_zaArchive), I, &zafsStat ) ) {
					return false;
				}
				auto aFileUtf16 = CUtilities::Utf8ToUtf16( reinterpret_cast<const char8_t *>(zafsStat.m_filename) );
				auto aTmp = CUtilities::ToLower( CUtilities::GetFileExtension( aFileUtf16 ) );
				if ( aTmp == sExt ) {
					try {
						
						std::vector<uint8_t> vTmp;
						if ( !ExtractToMemory( aFileUtf16, vTmp ) ) {
							if ( _pui32FailedLoads ) { (*_pui32FailedLoads)++; }
							continue;
						}
						_vResult.push_back( _u16PathTo + aFileUtf16 + _u16PathAfter );
						_vDataResult.push_back( std::move( vTmp ) );
					}
					catch ( ... ) {
						return false;
					}
				}
			}

			if ( _bExamineZipFiles ) {
				std::vector<std::u16string> vZips;
				if ( !GatherArchiveFiles( vZips, u".zip" ) ) {
					return false;
				}
				//auto aFullPath = _u16PathTo;
				size_t sInitialSize = _vResult.size();
				for ( size_t I = 0; I < vZips.size(); ++I ) {
					CZipFile zfTmp;
					std::vector<uint8_t> vTmp;
					if ( !ExtractToMemory( vZips[I], vTmp ) ) {
						if ( _pui32FailedLoads ) { (*_pui32FailedLoads)++; }
						continue;
					}
					if ( !zfTmp.OpenMemory( vTmp.data(), vTmp.size() ) ) { continue; }
					if ( !zfTmp.ExtractToMemory( /*aFullPath +*/ vZips[I] + u"{", u"}", _vResult, _vDataResult, _pcExt, _pui32FailedLoads, true ) ) {
						return false;
					}
				}
			}
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
