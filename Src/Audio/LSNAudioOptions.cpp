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
		//sfFormat				ui16Channels		ui16BitsPerChannel	pwcName
		{ LSN_SF_MONO_8,		1,					8,					LSN_LSTR( LSN_AUDIO_FORMAT_MONO8 ) },
		{ LSN_SF_MONO_16,		1,					16,					LSN_LSTR( LSN_AUDIO_FORMAT_MONO16 ) },
		{ LSN_SF_MONO_24,		1,					24,					LSN_LSTR( LSN_AUDIO_FORMAT_MONO24 ) },
		{ LSN_SF_MONO_F32,		1,					32,					LSN_LSTR( LSN_AUDIO_FORMAT_MONO32F ) },
	};

	LSN_AUDIO_PROFILE LSN_AUDIO_OPTIONS::s_apProfiles[] = {
		//wsDevice														fLpf			fHpf0			fHpf1		fHpf2		fVolume		fP1Volume	fP2Volume	fTVolume	fNVolume	fDmcVolume	fmFilterMode	bLpfEnable	bHpf0Enable	bHpf1Enable	bHpf2Enable	bInvert	bNoise
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N1151667 ),				17000.0f,		102.5f,			18.0f,		18.0f,		0.360f,		1.0f,		1.0f,		0.924f,		0.883f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true },
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N34169630 ),				17000.0f,		302.8f,			40.0f,		40.0f,		0.395f,		1.0f,		1.0f,		0.980f,		0.934f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N8869725 ),				26243.0f,		125.9664f,		20.0f,		20.0f,		0.446f,		1.0f,		1.0f,		0.966f,		0.924f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true },
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N8869725_RCA ),			0.0f,			100.0f,			20.0f,		20.0f,		0.344f,		1.0f,		1.0f,		0.892f,		0.855f,		1.0f,		LSN_FM_NORMAL,	false,		true,		true,		true,		true,	true },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_TL_NN101255729 ),				0.0f,			98.41125f,		20.0f,		30.0f,		0.957f,		1.0f,		1.0f,		0.731f,		0.693f,		1.0f,		LSN_FM_NORMAL,	false,		true,		true,		true,		true,	true },
		/*7872.899902f*/
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_TWIN_FAMI_475711 ),				7050.0f,		200.0f,			34.5f,		34.5f,		0.402f,		0.938f,		0.938f,		1.0f,		0.869f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true },
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_TWIN_FAMI_475711_SCART ),			0.0f,			300.0f,			20.0f,		20.0f,		0.735f,		0.975f,		1.0f,		0.841f,		0.827f,		1.0f,		LSN_FM_NORMAL,	false,		true,		true,		true,		true,	true },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_PAL_V7 ),						18000.0f,		89.6676f,		9.75f,		9.25f,		1.57f,		1.0f,		1.0f,		0.957f,		1.0f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true },
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
	size_t LSN_AUDIO_OPTIONS::FormatTotal() { return LSN_ELEMENTS( s_afFormats ); }

	/**
	 * Gets total presets.
	 * 
	 * \return Returns the total values in s_apProfiles.
	 **/
	size_t LSN_AUDIO_OPTIONS::PresetTotal() { return LSN_ELEMENTS( s_apProfiles ); }
 
}	// namespace lsn
