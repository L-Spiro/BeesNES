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


		// == Types.
		/**
		 * Struct LSN_FORMAT_OPTIONS
		 * \brief The settings for a single video format.
		 *
		 * Description: The settings for a single video format.  The defaults come from the video-format database and are the
		 *	highest-quality settings the format offers.
		 */
		struct LSN_FORMAT_OPTIONS {
			double											dFrameRate = 0.0;							/**< The output frame rate, or 0.0 to match the emulator exactly. */
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

				if ( !(dFrameRate >= 0.0 && dFrameRate <= 1000.0) ) { dFrameRate = foDefaults.dFrameRate; }
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
		};


		// == Members.
		LSN_FORMAT_OPTIONS									foFormat[LSN_VF_TOTAL];						/**< The settings for each video format. */
		LSN_VIDEO_FORMAT									vfFormat = LSN_VF_H264;						/**< The video format to which to export. */
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
