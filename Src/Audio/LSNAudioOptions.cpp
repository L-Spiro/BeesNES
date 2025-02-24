/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The implementation of the OpenAL audio system.
 */


#include "LSNAudioOptions.h"
#include "../Localization/LSNLocalization.h"
#include "../Utilities/LSNUtilities.h"


namespace lsn {

	LSN_AUDIO_FORMATS LSN_AUDIO_OPTIONS::s_afFormats[] = {
		//sfFormat				ui16Channels		ui16BitsPerChannel		pwcName
		{ LSN_SF_MONO_8,		1,					8,						LSN_LSTR( LSN_AUDIO_FORMAT_MONO8 ) },
		{ LSN_SF_MONO_16,		1,					16,						LSN_LSTR( LSN_AUDIO_FORMAT_MONO16 ) },
		{ LSN_SF_MONO_24,		1,					24,						LSN_LSTR( LSN_AUDIO_FORMAT_MONO24 ) },
		{ LSN_SF_MONO_F32,		1,					32,						LSN_LSTR( LSN_AUDIO_FORMAT_MONO32F ) },
	};

	// == Functions.
	/**
	 * Gets audio-format details by enumeration.
	 * 
	 * \param _sfFormat The format whose details are to be found.
	 * \return Returns a pointer to the specified format or nullptr.
	 **/
	const LSN_AUDIO_FORMATS * LSN_AUDIO_OPTIONS::FormatByEnum( LSN_SAMPLE_FORMAT _sfFormat ) {
		for ( auto I = LSN_ELEMENTS( s_afFormats ); I--; ) {
			if ( s_afFormats[I].sfFormat == _sfFormat ) { return &s_afFormats[I]; }
		}
		return nullptr;
	}

	/**
	 * Gets total audio formats.
	 * 
	 * \return Returns the total values in s_afFormats.
	 **/
	const size_t LSN_AUDIO_OPTIONS::FormatTotal() { return LSN_ELEMENTS( s_afFormats ); }
 
}	// namespace lsn
