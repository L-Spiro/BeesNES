/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Wraps ALCcontext pointers.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "LSNOpenAlDevice.h"
#include "LSNOpenAlInclude.h"

namespace lsn {

	/**
	 * Class COpenAlContext
	 * \brief Wrapper for an OpenAL context.
	 *
	 * Description: Wrapper for an OpenAL context.
	 */
	class COpenAlContext {
	public :
		COpenAlContext();
		~COpenAlContext();


		// == Functions.
		/**
		 * Gets the raw context pointer we wrap.
		 * 
		 * \return Returns the raw context pointer we wrap.
		 **/
		ALCcontext *						Context() { return m_pcContext; }

		/**
		 * Creates a context given a context name.
		 * 
		 * \param _oadDevice The context name or nullptr to create a context using the default sound context.
		 * \param _piAttributeList The attribute list can be NULL, or a zero terminated list of integer pairs composed of valid ALC attribute tokens and requested values. 
		 * \return Returns true if the context was created.
		 **/
		bool								CreateContext( COpenAlDevice &_oadDevice, const ALCint * _piAttributeList );

		/**
		 * Destroys the context.
		 * 
		 * \return Returns true if the context was destroyed.
		 **/
		bool								Reset();

		/**
		 * Makes the context current.
		 * 
		 * \return Returns true if the context was made current.
		 **/
		bool								MakeCurrent();

		/**
		 * Suspends the context.
		 * 
		 * \return Returns true if the context was suspended successfully.
		 **/
		bool								Suspend();

		/**
		 * Processes the context.
		 * 
		 * \return Returns true if the context was processed successfully.
		 **/
		bool								Process();


		protected :
		// == Members.
		/** The OpenAL context we wrap. */
		ALCcontext *						m_pcContext;
		/** Pointer to the device used to create the context.  Must remain valid for the life of the context. */
		COpenAlDevice *						m_poalDevice;
	};

}	// namespace lsn
