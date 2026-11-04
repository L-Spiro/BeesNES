/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for the APU.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Audio/LSNAudio.h"
#include "../Mappers/LSNMapperBase.h"


namespace lsn {

	/**
	 * Class CApuBase
	 * \brief The base class for the APU.
	 *
	 * Description: The base class for the APU.
	 */
	class CApuBase {
	public :
		CApuBase() {
		}
		virtual ~CApuBase() {
		}


		// == Types.
		/**
		 * A function that receives each final output sample (after the sample box and all post-processing).
		 *
		 * \param _pvParm The parameter passed to SetSampleOutFunc().
		 * \param _fSample The output sample.
		 */
		typedef void (*							PfSampleOut)( void * _pvParm, float _fSample );


		// == Functions.
		/**
		 * Gets the sample box that band-passes and down-samples the APU output.  Its output Hz is the final audio rate.
		 * 
		 * \return Returns a reference to the sample box.
		 **/
		inline CSampleBox &						SampleBox() { return CAudio::SampleBox(); }

		/**
		 * Gets the sample box that band-passes and down-samples the APU output.  Its output Hz is the final audio rate.
		 * 
		 * \return Returns a constant reference to the sample box.
		 **/
		inline const CSampleBox &				SampleBox() const { return CAudio::SampleBox(); }

		/**
		 * Sets the function that receives each final output sample.  It is called on the emulation thread for every sample, so it must be
		 *	fast.  The pointer is never nullptr:  passing nullptr installs a function that does nothing, so the per-sample call needs no check.
		 * 
		 * \param _pfFunc The function to call for each output sample, or nullptr to stop receiving samples.
		 * \param _pvParm The parameter to pass to _pfFunc.
		 **/
		void									SetSampleOutFunc( PfSampleOut _pfFunc, void * _pvParm ) {
			m_pfSampleOut = _pfFunc ? _pfFunc : DummySampleOut;
			m_pvSampleOutParm = _pfFunc ? _pvParm : nullptr;
		}

		/**
		 * Gets the function that receives each final output sample.
		 * 
		 * \return Returns the function that receives each final output sample (DummySampleOut() if none was set).
		 **/
		inline PfSampleOut						GetSampleOutFunc() const { return m_pfSampleOut; }

		/**
		 * Gets the parameter passed to the function that receives each final output sample.
		 * 
		 * \return Returns the parameter passed to the function that receives each final output sample.
		 **/
		inline void *							GetSampleOutParm() const { return m_pvSampleOutParm; }

		/**
		 * Sets the mapper to use for extension audio.
		 * 
		 * \param _pmbMapper The pointer to the current mapper or nullptr.
		 **/
		void									SetMapper( CMapperBase * _pmbMapper ) {
			m_pmbMapper = _pmbMapper;
		}


	protected :
		// == Members.
		CMapperBase *							m_pmbMapper = nullptr;
		PfSampleOut								m_pfSampleOut = DummySampleOut;								/**< Receives each final output sample.  Never nullptr. */
		void *									m_pvSampleOutParm = nullptr;								/**< The parameter passed to m_pfSampleOut. */


		// == Functions.
		/**
		 * The output-sample function used when no other is set.  Does nothing.
		 *
		 * \param _pvParm Unused.
		 * \param _fSample Unused.
		 */
		static void								DummySampleOut( void * /*_pvParm*/, float /*_fSample*/ ) {}
	};

}	// namespace lsn
