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
		//wsDevice														fLpf			fHpf0						fHpf1						fHpf2					fVolume		fP1Volume	fP2Volume	fTVolume	fNVolume	fDmcVolume	fmFilterMode	bLpfEnable	bHpf0Enable	bHpf1Enable	bHpf2Enable	bInvert	bNoise	bRp2A02	bSunsoft5b
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N0260073 ),				18000.0f,		119.01335153588215f,		39.629543326166626f,		39.600337986929674f,	2.601f,		1.0f,		1.0f,		0.933f,		0.744f,		1.0f,		LSN_FM_NORMAL,	false,		true,		true,		true,		true,	true,	true,	 false },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N1151667 ),				17000.0f,		91.670577848853171f,		37.490016890980982f,		7.9671720814703768f,	0.360f,		1.0f,		1.0f,		0.924f,		0.883f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N20914398 ),				0.0f,			224.20062289111536f,		125.06316358912643f,		51.287540390847823f,	4.525f,		1.0f,		1.0f,		0.924f,		0.906f,		1.0f,		LSN_FM_NORMAL,	false,		true,		true,		true,		false,	true,	false,	 true },	// 4.52856215666421402232799664488993585109710693359375

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N34169630 ),				17000.0f,		285.17092929859564f,		85.509330674952423f,		7.3617262313390981f,	0.395f,		1.0f,		1.0f,		1.0f,		0.934f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N8869725 ),				26243.0f,		83.459640336709199f,		55.985366776125886f,		8.5714225234124299f,	0.446f,		1.0f,		1.0f,		0.966f,		0.924f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N8869725_RCA ),			0.0f,			96.678039344736646f,		36.696956603852406f,		8.3012408410527119f,	0.344f,		1.0f,		1.0f,		0.892f,		0.855f,		1.0f,		LSN_FM_NORMAL,	false,		true,		true,		true,		true,	true,	false,	 false },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_TL_NN101255729 ),				0.0f,			80.308463849114986f,		52.357757591613563f,		7.9206051188443904f,	0.957f,		1.0f,		1.0f,		0.731f,		0.693f,		1.0f,		LSN_FM_NORMAL,	false,		true,		true,		true,		true,	true,	false,	 false },
		/*7872.899902f*/
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_TWIN_FAMI_475711 ),				7000.0f,		172.03311180028109f,		38.037422049578915f,		38.037208628936021f,	0.402f,		0.938f,		0.938f,		1.0f,		0.869f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_TWIN_FAMI_475711_SCART ),			0.0f,			302.96765178343128f,		30.586358931812008f,		4.0903076481735425f,	0.735f,		0.975f,		1.0f,		0.841f,		0.827f,		1.0f,		LSN_FM_NORMAL,	false,		true,		true,		true,		true,	true,	false,	 false },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_PAL_V7 ),						64000.0f,		94.986785805568559f,		8.851913371856071f,			8.8518991011365298f,	1.57f,		1.0f,		1.0f,		0.957f,		1.0f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },
	};


	// == Functions.
	/**
	 * Gets audio-format details by enumeration.
	 * 
	 * \param _sfFormat The format whose details are to be found.
	 * \return Returns a pointer to the specified format or nullptr.
	 **/
	const LSN_AUDIO_FORMATS * LSN_AUDIO_OPTIONS::FormatByEnum( LSN_SAMPLE_FORMAT _sfFormat ) {
		for ( auto I = std::size( s_afFormats ); I--; ) {
			if ( s_afFormats[I].sfFormat == _sfFormat ) { return &s_afFormats[I]; }
		}
		return nullptr;
	}

	/**
	 * Gets total audio formats.
	 * 
	 * \return Returns the total values in s_afFormats.
	 **/
	size_t LSN_AUDIO_OPTIONS::FormatTotal() { return std::size( s_afFormats ); }

	/**
	 * Gets total presets.
	 * 
	 * \return Returns the total values in s_apProfiles.
	 **/
	size_t LSN_AUDIO_OPTIONS::PresetTotal() { return std::size( s_apProfiles ); }
 
}	// namespace lsn
