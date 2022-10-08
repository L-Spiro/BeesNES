/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A class for opening files using the standard C library (FILE).
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../OS/LSNOs.h"
#include "LSNFileBase.h"

namespace lsn {

	/**
	 * Class CStdFile
	 * \brief A class for opening files using the standard C library (FILE).
	 *
	 * Description: A class for opening files using the standard C library (FILE).
	 */
	class CStdFile : public CFileBase {
	public :
		CStdFile();
		virtual ~CStdFile();


		// == Functions.
#ifdef LSN_WINDOWS
		/**
		 * Opens a file.  The path is given in UTF-8.
		 *
		 * \param _pcPath Path to the file to open.
		 * \return Returns true if the file was opened, false otherwise.
		 */
		virtual bool										Open( const char8_t * _pcFile ) { return CFileBase::Open( _pcFile ); }

		/**
		 * Opens a file.  The path is given in UTF-16.
		 *
		 * \param _pcPath Path to the file to open.
		 * \return Returns true if the file was opened, false otherwise.
		 */
		virtual bool										Open( const char16_t * _pcFile );
#else
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
		virtual bool										Open( const char16_t * _pcFile ) { return CFileBase::Open( _pcFile ); }
#endif	// #ifdef LSN_WINDOWS

		/**
		 * Closes the opened file.
		 */
		virtual void										Close();

		/**
		 * Loads the opened file to memory, storing the result in _vResult.
		 *
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was successfully loaded into memory.
		 */
		virtual bool										LoadToMemory( std::vector<uint8_t> &_vResult ) const;


	protected :
		// == Members.
		FILE *												m_pfFile;							/**< The FILE object to maintain. */
		uint64_t											m_ui64Size;							/**< The file size. */


		// == Functions.
		/**
		 * Performs post-loading operations after a successful loading of the file.  m_pfFile will be valid when this is called.  Override to perform additional loading operations on m_pfFile.
		 */
		virtual void										PostLoad();
	};

}	// namespace lsn
