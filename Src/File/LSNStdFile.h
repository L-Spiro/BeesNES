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

		/**
		 * Creates a file.  The path is given in UTF-8.
		 *
		 * \param _pcPath Path to the file to create.
		 * \return Returns true if the file was created, false otherwise.
		 */
		virtual bool										Create( const char8_t * _pcFile ) { return CFileBase::Create( _pcFile ); }

		/**
		 * Creates a file.  The path is given in UTF-16.
		 *
		 * \param _pcPath Path to the file to create.
		 * \return Returns true if the file was created, false otherwise.
		 */
		virtual bool										Create( const char16_t * _pcFile );
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

		/**
		 * Creates a file.  The path is given in UTF-8.
		 *
		 * \param _pcPath Path to the file to create.
		 * \return Returns true if the file was created, false otherwise.
		 */
		virtual bool										Create( const char8_t * _pcFile );

		/**
		 * Creates a file.  The path is given in UTF-16.
		 *
		 * \param _pcPath Path to the file to create.
		 * \return Returns true if the file was created, false otherwise.
		 */
		virtual bool										Create( const char16_t * _pcFile ) { return CFileBase::Create( _pcFile ); }
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

		/**
		 * Writes the given data to the created file.  File must have been cerated with Create().
		 *
		 * \param _vData The data to write to the file.
		 * \return Returns true if the data was successfully written to the file.
		 */
		virtual bool										WriteToFile( const std::vector<uint8_t> &_vData );

		/**
		 * Writes the given data to the created file.  File must have been cerated with Create().
		 *
		 * \param _pui8Data The data to write to the file.
		 * \param _tsSize The size of the buffer to which _pui8Data points.
		 * \return Returns true if the data was successfully written to the file.
		 */
		virtual bool										WriteToFile( const uint8_t * _pui8Data, size_t _tsSize );


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
