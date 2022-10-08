/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for working with files.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include <vector>

namespace lsn {

	/**
	 * Class CFileBase
	 * \brief The base class for working with files.
	 *
	 * Description: The base class for working with files.
	 */
	class CFileBase {
	public :
		virtual ~CFileBase();


		// == Functions.
		/**
		 * Opens a file.  The path is given in UTF-8.
		 *
		 * \param _pcPath Path to the file to open.
		 * \return Returns true if the file was opened, false otherwise.
		 */
		virtual bool										Open( const char8_t * _pcFile );

		/**
		 * Opens a file.  The path is given in UTF-16.
		 *
		 * \param _pcPath Path to the file to open.
		 * \return Returns true if the file was opened, false otherwise.
		 */
		virtual bool										Open( const char16_t * _pcFile );

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
		 * Loads the opened file to memory, storing the result in _vResult.
		 *
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was successfully loaded into memory.
		 */
		virtual bool										LoadToMemory( std::vector<uint8_t> &_vResult ) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.

}	// namespace lsn
