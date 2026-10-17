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
		//wsDevice														fLpf						fZero						fHpf0						fHpf1						fHpf2					fVolume		fP1Volume	fP2Volume	fTVolume	fNVolume	fDmcVolume	fmFilterMode	bLpfEnable	bHpf0Enable	bHpf1Enable	bHpf2Enable	bInvert	bNoise	bRp2A02	bSunsoft5b
		// 1789806.67721439967863261699676513671875 Hz
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N0260073 ),				18383.331849143484f,		0.0f,						128.203310801000f,			36.579855150267f,			36.579848133916f,		2.601f,		1.0f,		1.0f,		0.933f,		0.744f,		0.938f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	true,	 false },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N1151667 ),				16596.500112773516f,		0.0f,						91.670577848853171f,		37.490016890980982f,		7.9671720814703768f,	0.360f,		1.0f,		1.0f,		0.924f,		0.883f,		0.943f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },

		// 1789730.4387488900683820247650146484375 Hz.
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N13721267 ),				17524.208150165094f,		144384.174617951474f,		97.341173341196f,			61.825863515323f,			6.059893000666f,		1.161f,		1.0f,		0.994f,		0.975f,		0.952f,		0.966f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N20914398 ),				0.0f,						0.0f,						224.20062289111536f,		125.06316358912643f,		51.287540390847823f,	4.525f,		1.0f,		1.0f,		0.924f,		0.906f,		1.0f,		LSN_FM_NORMAL,	false,		true,		true,		true,		false,	true,	false,	 true },	// 4.52856215666421402232799664488993585109710693359375

		// 1789740.8619818985462188720703125 Hz
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N32670105 ),				15851.963185973395f,		0.0f,						99.407819166312f,			7.934473497640f,			7.934471615916f,		1.064f,		1.0f,		0.975f,		0.952f,		0.933f,		0.975f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },

		// 1789763.0466916109435260295867919921875 Hz
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N34169630 ),				16879.431344679029f,		145249.618967504910f,		307.866180500731f,			77.224662166419f,			9.329261475967f,		0.404f,		0.975f,		0.975f,		0.975f,		0.910f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N8869725 ),				14482.132075089257f,		0.0f,						83.459640336709199f,		55.985366776125886f,		8.5714225234124299f,	0.446f,		1.0f,		1.0f,		0.966f,		0.924f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_FL_N8869725_RCA ),			16003.975697663334f,		0.0f,						96.678039344736646f,		36.696956603852406f,		8.3012408410527119f,	0.344f,		1.0f,		1.0f,		0.892f,		0.855f,		0.924f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_TL_NN101255729 ),				14893.677244647428f,		0.0f,						80.308463849114986f,		52.357757591613563f,		7.9206051188443904f,	0.957f,		1.0f,		1.0f,		0.731f,		0.693f,		0.744f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },

		// 1789777.78998585441149771213531494140625 Hz.
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_TWIN_FAMI_441850 ),				3719.752299817048f,			0.0f,						206.591994496089f,			24.535811985325f,			24.535802584134f,		0.721f,		0.906f,		0.883f,		1.0f,		0.929f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },

		{ LSN_LSTR( LSN_AUDIO_OPTIONS_TWIN_FAMI_475711 ),				6813.7368995f,				0.0f,						202.359654532700f,			34.967573649161f,			34.967565533357f,		0.402f,		0.938f,		0.938f,		1.0f,		0.869f,		0.929f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_TWIN_FAMI_475711_SCART ),			0.0f,						0.0f,						302.96765178343128f,		30.586358931812008f,		4.0903076481735425f,	0.735f,		0.975f,		1.0f,		0.841f,		0.827f,		0.896f,		LSN_FM_NORMAL,	false,		true,		true,		true,		true,	true,	false,	 false },

		// 1662577.1085126209072768688201904296875 Hz.
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_PAL_PM313006 ),				14912.561817246404f,		188764.928831997269f,		99.713059858228f,			64.057801563356f,			8.189826234573f,		1.062f,		0.882f,		0.859f,		0.814f,		0.771f,		0.808f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },
		{ LSN_LSTR( LSN_AUDIO_OPTIONS_NES_PAL_V7 ),						64000.0f,					0.0f,						94.986785805568559f,		8.851913371856071f,			8.8518991011365298f,	1.57f,		1.0f,		1.0f,		0.957f,		1.0f,		1.0f,		LSN_FM_NORMAL,	true,		true,		true,		true,		true,	true,	false,	 false },
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
