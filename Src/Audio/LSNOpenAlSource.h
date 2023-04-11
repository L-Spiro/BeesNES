/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wraps an ALuint source ID.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNOpenAlDevice.h"
#include "LSNOpenAlInclude.h"

namespace lsn {

	/**
	 * Class COpenAlSource
	 * \brief Wraps an ALuint source ID.
	 *
	 * Description: Wraps an ALuint source ID.
	 */
	class COpenAlSource {
	public :
		COpenAlSource();
		~COpenAlSource();


		// == Functions.
		/**
		 * Gets the source ID.
		 * 
		 * \return Returns the source ID.
		 **/
		inline ALuint						Id() { return m_uiId; }

		/**
		 * Creates a source.
		 * 
		 * \return Returns true if the source was created.
		 **/
		bool								CreateSource();

		/**
		 * Destroys the source.
		 * 
		 * \return Returns true if the source was destroyed.
		 **/
		bool								Reset();

	protected :
		// == Members.
		/** The source ID. */
		ALuint								m_uiId;
	};

}	// namespace lsn
