/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Video-export options.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Video/LSNVideoFormat.h"

#include <string>
#include <vector>


namespace lsn {

	/**
	 * Struct LSN_VIDEO_EXPORT_OPTIONS
	 * \brief Video-export options.
	 *
	 * Description: Video-export options.  Global settings plus a set of settings for each video format, so that switching formats does not
	 *	lose the settings of the previous format.
	 */
	struct LSN_VIDEO_EXPORT_OPTIONS {
		LSN_VIDEO_EXPORT_OPTIONS() {
			for ( uint16_t I = 0; I < LSN_VF_TOTAL; ++I ) {
				foFormat[I] = LSN_FORMAT_OPTIONS::Defaults( LSN_VIDEO_FORMAT( I ) );
			}
		}


		// == Enumerations.
		/** When a recording starts. */
		enum LSN_START_CONDITION : uint8_t {
			LSN_VSC_NONE,																						/**< Recording starts immediately. */
			LSN_VSC_START_AT_FRAME,																				/**< Recording starts at the first frame whose PPU frame number is at least ui64StartFrame. */
			LSN_VSC_TOTAL,																						/**< The number of start conditions. */
		};

		/** When a recording stops by itself. */
		enum LSN_END_CONDITION : uint8_t {
			LSN_VEC_NONE,																						/**< Recording continues until it is stopped. */
			LSN_VEC_END_AT_FRAME,																				/**< Recording stops before the first frame whose PPU frame number is at least ui64EndFrame. */
			LSN_VEC_DURATION,																					/**< Recording stops once the movie is dEndSeconds seconds long. */
			LSN_VEC_TOTAL,																						/**< The number of end conditions. */
		};

		/** Limits. */
		enum LSN_LIMITS : uint32_t {
			LSN_MAX_FPS																		= 1000,				/**< The highest output frame rate allowed (frames per second). */
		};


		// == Types.
		/**
		 * Struct LSN_FORMAT_OPTIONS
		 * \brief The settings for a single video format.
		 *
		 * Description: The settings for a single video format.  The defaults come from the video-format database and are the
		 *	highest-quality settings the format offers.
		 */
		struct LSN_FORMAT_OPTIONS {
			uint32_t										ui32FrameRateNum = 0;						/**< The numerator of the output frame rate, or 0 to match the console exactly. */
			uint32_t										ui32FrameRateDen = 1;						/**< The denominator of the output frame rate. */
			int32_t											i32Quality = 0;								/**< The main quality value (see LSN_VIDEO_FORMAT_DATA::vqdQuality). */
			int32_t											i32AltQuality = 0;							/**< The alternative quality value (see LSN_VIDEO_FORMAT_DATA::vqdAltQuality). */
			uint32_t										ui32Preset = 0;								/**< The index of the encoder speed preset. */
			uint32_t										ui32AudioBitrate = 0;						/**< The audio bitrate in kilobits per second (ignored by lossless audio codecs). */
			LSN_VIDEO_PIXEL_FORMAT							vpfPixelFormat = LSN_VPF_YUV420P;			/**< The pixel format in which to encode. */
			LSN_VIDEO_AUDIO_CODEC							vacAudioCodec = LSN_VAC_AAC;				/**< The audio codec. */
			bool											bUseAltQuality = false;						/**< If true, i32AltQuality is used instead of i32Quality. */
			bool											bLossless = false;							/**< If true and the format supports it, encode losslessly (the quality values are ignored). */


			// == Functions.
			/**
			 * Gets the default (highest-quality) settings for a video format.
			 *
			 * \param _vfFormat The video format whose default settings are to be returned.
			 * \return Returns the default settings for the given video format.
			 */
			static LSN_FORMAT_OPTIONS						Defaults( LSN_VIDEO_FORMAT _vfFormat ) {
				LSN_FORMAT_OPTIONS foRet;
				const LSN_VIDEO_FORMAT_DATA * pvfdData = CVideoFormat::Data( _vfFormat );
				if ( !pvfdData ) { return foRet; }

				foRet.i32Quality = (*pvfdData).vqdQuality.i32Best;
				foRet.i32AltQuality = (*pvfdData).vqdAltQuality.i32Best;
				foRet.ui32Preset = (*pvfdData).ui32DefaultPreset;
				foRet.vpfPixelFormat = (*pvfdData).vpfBest;
				foRet.vacAudioCodec = (*pvfdData).vacBest;
				const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdAudio = CVideoFormat::AudioCodecData( foRet.vacAudioCodec );
				foRet.ui32AudioBitrate = pvacdAudio ? (*pvacdAudio).ui32BestBitrate : 0;
				foRet.bLossless = (*pvfdData).pcLosslessArgs != nullptr;
				return foRet;
			}

			/**
			 * Replaces any values that are out of range for the given video format with its defaults.  Call after loading.
			 *
			 * \param _vfFormat The video format to which these settings belong.
			 */
			void											Validate( LSN_VIDEO_FORMAT _vfFormat ) {
				const LSN_VIDEO_FORMAT_DATA * pvfdData = CVideoFormat::Data( _vfFormat );
				if ( !pvfdData ) { return; }
				LSN_FORMAT_OPTIONS foDefaults = Defaults( _vfFormat );

				if ( !ValidFrameRate( ui32FrameRateNum, ui32FrameRateDen ) ) {
					ui32FrameRateNum = foDefaults.ui32FrameRateNum;
					ui32FrameRateDen = foDefaults.ui32FrameRateDen;
				}
				if ( i32Quality < (*pvfdData).vqdQuality.i32Min || i32Quality > (*pvfdData).vqdQuality.i32Max ) { i32Quality = foDefaults.i32Quality; }
				if ( i32AltQuality < (*pvfdData).vqdAltQuality.i32Min || i32AltQuality > (*pvfdData).vqdAltQuality.i32Max ) { i32AltQuality = foDefaults.i32AltQuality; }
				if ( ui32Preset >= (*pvfdData).ui32TotalPresets ) { ui32Preset = foDefaults.ui32Preset; }
				if ( !CVideoFormat::SupportsPixelFormat( _vfFormat, vpfPixelFormat ) ) { vpfPixelFormat = foDefaults.vpfPixelFormat; }
				if ( !CVideoFormat::SupportsAudioCodec( _vfFormat, vacAudioCodec ) ) {
					vacAudioCodec = foDefaults.vacAudioCodec;
					ui32AudioBitrate = foDefaults.ui32AudioBitrate;
				}
				const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdAudio = CVideoFormat::AudioCodecData( vacAudioCodec );
				if ( pvacdAudio && (ui32AudioBitrate < (*pvacdAudio).ui32MinBitrate || ui32AudioBitrate > (*pvacdAudio).ui32MaxBitrate) ) { ui32AudioBitrate = (*pvacdAudio).ui32BestBitrate; }
				if ( (*pvfdData).vqdAltQuality.vqmMode == LSN_VQM_NONE ) { bUseAltQuality = false; }
				if ( !(*pvfdData).pcLosslessArgs ) { bLossless = false; }
			}

			/**
			 * Determines whether the output frame rate matches the console exactly.
			 *
			 * \return Returns true if the console's frame rate is used.
			 */
			bool											MatchesConsole() const { return ui32FrameRateNum == 0; }

			/**
			 * Determines whether a frame rate can be used as the output frame rate.  0 (with any denominator) means "match the console" and
			 *	is valid; otherwise the rate must be above 0 and at most LSN_MAX_FPS frames per second.
			 *
			 * \param _ui32Num The numerator.
			 * \param _ui32Den The denominator.
			 * \return Returns true if the frame rate is valid.
			 */
			static bool										ValidFrameRate( uint32_t _ui32Num, uint32_t _ui32Den ) {
				if ( _ui32Num == 0 ) { return true; }
				return _ui32Den != 0 && uint64_t( _ui32Num ) <= uint64_t( LSN_MAX_FPS ) * _ui32Den;
			}
		};


		// == Members.


		LSN_FORMAT_OPTIONS									foFormat[LSN_VF_TOTAL];						/**< The settings for each video format. */
		std::wstring										wsOutputFolder;								/**< The folder into which movies are written, or an empty string for the Movies folder beside BeesNES. */
		std::wstring										wsFfmpegPath;								/**< The FFmpeg executable (or the folder containing it), or an empty string to search for it. */
		std::vector<std::wstring>							vStartHistory;								/**< Recently entered start-condition values. */
		std::vector<std::wstring>							vEndHistory;								/**< Recently entered end-condition values. */
		uint64_t											ui64StartFrame = 0;							/**< The PPU frame at which to start recording (LSN_VSC_START_AT_FRAME). */
		uint64_t											ui64EndFrame = 0;							/**< The PPU frame before which to stop recording (LSN_VEC_END_AT_FRAME). */
		double												dEndSeconds = 0.0;							/**< The length of the movie at which to stop recording (LSN_VEC_DURATION). */
		LSN_VIDEO_FORMAT									vfFormat = LSN_VF_H264;						/**< The video format to which to export. */
		LSN_START_CONDITION									scStart = LSN_VSC_NONE;						/**< When recording starts. */
		LSN_END_CONDITION									ecEnd = LSN_VEC_NONE;						/**< When recording stops by itself. */
		bool												bInterpolateFrames = true;					/**< If true, frames are blended when the output frame rate does not match the emulator's frame rate, for smooth playback. */


		// == Functions.
		/**
		 * Gets the settings for the current video format.
		 *
		 * \return Returns the settings for the current video format.
		 */
		LSN_FORMAT_OPTIONS &								Current() { return foFormat[vfFormat < LSN_VF_TOTAL ? vfFormat : LSN_VF_H264]; }

		/**
		 * Gets the settings for the current video format.
		 *
		 * \return Returns the settings for the current video format.
		 */
		const LSN_FORMAT_OPTIONS &							Current() const { return foFormat[vfFormat < LSN_VF_TOTAL ? vfFormat : LSN_VF_H264]; }
	};

}	// namespace lsn
