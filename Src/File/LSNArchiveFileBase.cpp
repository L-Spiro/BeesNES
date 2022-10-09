/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for opening/working with archived files.
 */


#include "LSNArchiveFileBase.h"

namespace lsn {

	CArchiveFileBase::~CArchiveFileBase() {
	}

	// == Functions.
	/**
	 * If true, the file is an archive containing more files.
	 *
	 * \return Returns true if the file is an archive, false otherwise.
	 */
	bool CArchiveFileBase::IsArchive() const { return true; }

	/**
	 * Gathers the file names in the archive into an array.
	 *
	 * \param _vResult The location where to store the file names.
	 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
	 */
	bool CArchiveFileBase::GatherArchiveFiles( std::vector<std::u16string> &/*_vResult*/ ) const { return false; }

	/**
	 * Gathers the file names in the archive into an array.
	 *
	 * \param _s16File The name of the file to extract.
	 * \param _vResult The location where to store the file in memory.
	 * \return Returns true if the file was extracted successfully.
	 */
	bool CArchiveFileBase::ExtractToMemory( const std::u16string &/*_s16File*/, std::vector<uint8_t> &/*_vResult*/ ) const { return false; }

}	// namespace lsn
