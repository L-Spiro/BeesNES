/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A class for working with ZIP files.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../MiniZ/miniz.h"
#include "LSNStdFile.h"

namespace lsn {

	/**
	 * Class CZipFile
	 * \brief A class for working with ZIP files.
	 *
	 * Description: A class for working with ZIP files.
	 */
	class CZipFile : public CStdFile {
	public :
		CZipFile();
		virtual ~CZipFile();


		// == Functions.
		/**
		 * Loads an in-memory file image.
		 * 
		 * \param _pvBuffer The file data to load.
		 * \param _sSize The number of bytes to which _pbBuffer points.
		 * \return Returns rtue if the file was loaded.
		 **/
		bool												OpenMemory( const void * _pvBuffer, size_t _sSize );

		/**
		 * Closes the opened file.
		 */
		virtual void										Close();

		/**
		 * If true, the file is an archive containing more files.
		 *
		 * \return Returns true if the file is an archive, false otherwise.
		 */
		virtual bool										IsArchive() const;

		/**
		 * Gathers the file names in the archive into an array.
		 *
		 * \param _vResult The location where to store the file names.
		 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
		 */
		virtual bool										GatherArchiveFiles( std::vector<std::u16string> &_vResult ) const;

		/**
		 * Gathers the file names of a specific type in the archive into an array.
		 *
		 * \param _vResult The location where to store the file names.
		 * \param _pcExt The extension of files to add to _vResult.
		 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
		 */
		virtual bool										GatherArchiveFiles( std::vector<std::u16string> &_vResult, const char16_t * _pcExt ) const;

		/**
		 * Extracts a file into a memory buffer.
		 *
		 * \param _s16File The name of the file to extract.
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was extracted successfully.
		 */
		virtual bool										ExtractToMemory( const std::u16string &_s16File, std::vector<uint8_t> &_vResult ) const;

		/**
		 * Extracts all files of a given extension to buffers.
		 *
		 * \param _vResult The location where to store the file names.
		 * \param _vDataResult Holds the extracted files.
		 * \param _pcExt The extension of files to add to _vResult.
		 * \param _bExamineZipFiles If true, embedded ZIP files will also be extracted and recursively traversed.
		 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
		 */
		virtual bool										ExtractToMemory( std::vector<std::u16string> &_vResult, std::vector<std::vector<uint8_t>> &_vDataResult, const char16_t * _pcExt, uint32_t * _pui32FailedLoads = nullptr, bool _bExamineZipFiles = true ) const;

		/**
		 * Extracts all files of a given extension to buffers.
		 *
		 * \param _u16PathTo The path to this ZIP file.
		 * \param _u16PathAfter A postfix to append to the path..
		 * \param _vResult The location where to store the file names.
		 * \param _vDataResult Holds the extracted files.
		 * \param _pcExt The extension of files to add to _vResult.
		 * \param _bExamineZipFiles If true, embedded ZIP files will also be extracted and recursively traversed.
		 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
		 */
		virtual bool										ExtractToMemory( const std::u16string &_u16PathTo, const std::u16string &_u16PathAfter, std::vector<std::u16string> &_vResult, std::vector<std::vector<uint8_t>> &_vDataResult, const char16_t * _pcExt, uint32_t * _pui32FailedLoads = nullptr, bool _bExamineZipFiles = true ) const;


	protected :
		// == Members.
		::mz_zip_archive									m_zaArchive;							/**< The miniz archive object. */


		// == Functions.
		/**
		 * Performs post-loading operations after a successful loading of the file.  m_pfFile will be valid when this is called.  Override to perform additional loading operations on m_pfFile.
		 */
		virtual void										PostLoad();

	};

}	// namespace lsn
