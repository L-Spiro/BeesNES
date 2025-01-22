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
		 * Gathers the file names in the archive into an array.
		 *
		 * \param _s16File The name of the file to extract.
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was extracted successfully.
		 */
		virtual bool										ExtractToMemory( const std::u16string &_s16File, std::vector<uint8_t> &_vResult ) const;


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
