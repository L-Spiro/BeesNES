/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A file-mapping.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNFileBase.h"

#include <string>
#include <vector>

#ifdef LSN_USE_WINDOWS
#include <Helpers/LSWHelpers.h>
#endif

namespace lsn {

	/**
	 * Class CFileMap
	 * \brief A file-mapping.
	 *
	 * Description: A file-mapping.
	 */
	class CFileMap : public CFileBase {
	public :
		CFileMap();
		virtual ~CFileMap();


		// == Functions.
#ifdef LSN_USE_WINDOWS
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
#endif	// #ifdef LSN_USE_WINDOWS

		/**
		 * Closes the opened file.
		 */
		virtual void										Close();

		/**
		 * Gets the size of the file.
		 * 
		 * \return Returns the size of the file.
		 **/
		virtual uint64_t									Size() const;


	protected :
		// == Members.
#ifdef LSN_USE_WINDOWS
		lsw::LSW_HANDLE										m_hFile;						/**< The file handle. */
		lsw::LSW_HANDLE										m_hMap;							/**< The file-mapping handle. */
		mutable PBYTE										m_pbMapBuffer;					/**< Mapped bytes. */
		bool												m_bIsEmpty;						/**< Is the file 0-sized? */
		bool												m_bWritable;					/**< Read-only or read-write? */
		mutable uint64_t									m_ui64Size;						/**< Size of the file. */
		mutable uint64_t									m_ui64MapStart;					/**< Map start. */
		mutable DWORD										m_dwMapSize;					/**< Mapped size. */
#else
#endif	// #ifdef LSN_USE_WINDOWS


		// == Functions.
		/**
		 * Creates the file map.
		 * 
		 * \return Returns true if the file mapping was successfully created.
		 **/
		bool												CreateFileMap();

	};

}	// namespace lsn
