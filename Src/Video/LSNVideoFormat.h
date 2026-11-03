/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The video formats that can be exported via FFmpeg, along with a database of their properties.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Display/LSNCaptureBuffer.h"
#include "../Localization/LSNLocalization.h"

#include <string>


namespace lsn {

	/**
	 * Enumeration LSN_VIDEO_FORMAT
	 * \brief The video formats that can be exported via FFmpeg.
	 *
	 * Description: The video formats that can be exported via FFmpeg.  Each is a combination of an FFmpeg encoder and a container.
	 */
	enum LSN_VIDEO_FORMAT : uint16_t {
#define LSN_VF_ENUM( ENUM, TXT )							ENUM,
#include "LSNVideoFormatEnum.inl"
#undef LSN_VF_ENUM
		LSN_VF_TOTAL,																										/**< The number of video formats. */
	};

	/**
	 * Enumeration LSN_VIDEO_PIXEL_FORMAT
	 * \brief The pixel formats in which video can be encoded.
	 *
	 * Description: The pixel formats in which video can be encoded.  FFmpeg converts the captured frames to these.
	 */
	enum LSN_VIDEO_PIXEL_FORMAT : uint8_t {
		LSN_VPF_YUV420P,																									/**< yuv420p:  YUV 4:2:0, 8 bits. */
		LSN_VPF_YUV422P,																									/**< yuv422p:  YUV 4:2:2, 8 bits. */
		LSN_VPF_YUV444P,																									/**< yuv444p:  YUV 4:4:4, 8 bits. */
		LSN_VPF_NV12,																										/**< nv12:  YUV 4:2:0, 8 bits, interleaved UV. */
		LSN_VPF_P010,																										/**< p010le:  YUV 4:2:0, 10 bits, interleaved UV. */
		LSN_VPF_YUV420P10,																									/**< yuv420p10le:  YUV 4:2:0, 10 bits. */
		LSN_VPF_YUV422P10,																									/**< yuv422p10le:  YUV 4:2:2, 10 bits. */
		LSN_VPF_YUV444P10,																									/**< yuv444p10le:  YUV 4:4:4, 10 bits. */
		LSN_VPF_YUV444P16,																									/**< yuv444p16le:  YUV 4:4:4, 16 bits (NVENC uses this for 10-bit 4:4:4). */
		LSN_VPF_GBRP,																										/**< gbrp:  Planar RGB, 8 bits. */
		LSN_VPF_GBRP10,																										/**< gbrp10le:  Planar RGB, 10 bits. */
		LSN_VPF_TOTAL,																										/**< The number of pixel formats. */
	};

	/**
	 * Enumeration LSN_VIDEO_AUDIO_CODEC
	 * \brief The audio codecs that can accompany exported video.
	 *
	 * Description: The audio codecs that can accompany exported video.
	 */
	enum LSN_VIDEO_AUDIO_CODEC : uint8_t {
		LSN_VAC_AAC,																										/**< AAC (FFmpeg's native encoder). */
		LSN_VAC_ALAC,																										/**< Apple Lossless. */
		LSN_VAC_FLAC,																										/**< FLAC. */
		LSN_VAC_OPUS,																										/**< Opus (libopus). */
		LSN_VAC_MP3,																										/**< MP3 (LAME). */
		LSN_VAC_PCM_S16,																									/**< 16-bit signed PCM. */
		LSN_VAC_PCM_S24,																									/**< 24-bit signed PCM. */
		LSN_VAC_PCM_F32,																									/**< 32-bit floating-point PCM. */
		LSN_VAC_TOTAL,																										/**< The number of audio codecs. */
	};

	/**
	 * Enumeration LSN_VIDEO_QUALITY_MODE
	 * \brief The ways in which the quality of a video format can be controlled.
	 *
	 * Description: The ways in which the quality of a video format can be controlled.  Determines how a quality value is interpreted.
	 */
	enum LSN_VIDEO_QUALITY_MODE : uint8_t {
		LSN_VQM_NONE,																										/**< The format has no quality setting (lossless formats). */
		LSN_VQM_CRF,																										/**< Constant Rate Factor (x264, x265, SVT-AV1, libvpx).  Lower is better. */
		LSN_VQM_QP,																											/**< Constant quantizer (NVENC, AMF).  Lower is better. */
		LSN_VQM_CQ,																											/**< Constant quality (NVENC VBR).  Lower is better. */
		LSN_VQM_ICQ,																										/**< Intelligent Constant Quality (Quick Sync).  Lower is better. */
		LSN_VQM_QSCALE,																										/**< Quantizer scale (MPEG-4 Part 2).  Lower is better. */
		LSN_VQM_PROFILE,																									/**< An enumerated profile (ProRes).  Higher is better. */
		LSN_VQM_BITRATE,																									/**< A target bitrate in kilobits per second.  Higher is better. */
	};

	/**
	 * A function that prepares a captured frame for FFmpeg.  Returns either _cbSrc itself (when FFmpeg can take the frame as-is) or
	 *	_cbScratch after converting _cbSrc into it.  Returns nullptr on failure.
	 *
	 * \param _cbSrc The captured frame.
	 * \param _cbScratch A buffer into which to convert the frame if a conversion is needed.  Keep it around between frames to avoid reallocations.
	 * \return Returns the buffer to send to FFmpeg or nullptr.
	 */
	typedef const LSN_CAPTURE_BUFFER *						(* PfConversionFunc)( const LSN_CAPTURE_BUFFER &_cbSrc, LSN_CAPTURE_BUFFER &_cbScratch );

	/**
	 * Struct LSN_VIDEO_QUALITY_DATA
	 * \brief Describes one of the ways in which the quality of a video format can be set.
	 *
	 * Description: Describes one of the ways in which the quality of a video format can be set.  Provides the range for a slider/edit
	 *	control and the FFmpeg arguments that apply the setting.
	 */
	struct LSN_VIDEO_QUALITY_DATA {
		const char *										pcArgs;										/**< The FFmpeg arguments that apply the setting.  Each "{Q}" is replaced by the value.  nullptr if unused. */
		const wchar_t * const *								ppwcValueNames;								/**< Display names for each value from i32Min to i32Max, or nullptr if the values are shown as numbers. */
		int32_t												i32Min;										/**< The minimum value. */
		int32_t												i32Max;										/**< The maximum value. */
		int32_t												i32Best;									/**< The value that gives the highest quality.  This is the default. */
		int32_t												i32Recommended;								/**< A value that gives a good balance between quality and size. */
		int32_t												i32Step;									/**< The step between values in the UI. */
		LSN_VIDEO_QUALITY_MODE								vqmMode;									/**< How the value is interpreted. */
	};

	/**
	 * Struct LSN_VIDEO_PRESET
	 * \brief An encoder speed preset.
	 *
	 * Description: An encoder speed preset.  Presets trade encoding speed for compression efficiency.
	 */
	struct LSN_VIDEO_PRESET {
		const char *										pcArgs;										/**< The FFmpeg arguments that select the preset. */
		const char *										pcName;										/**< The preset's name as the encoder knows it.  Suitable for display. */
	};

	/**
	 * Struct LSN_VIDEO_PIXEL_FORMAT_DATA
	 * \brief Information about a pixel format.
	 *
	 * Description: Information about a pixel format.
	 */
	struct LSN_VIDEO_PIXEL_FORMAT_DATA {
		const char *										pcFfmpegName;								/**< The FFmpeg name (-pix_fmt). */
		const wchar_t *										pwcName;									/**< The display name. */
		uint8_t												ui8BitDepth;								/**< Bits per component. */
		uint8_t												ui8ChromaShiftX;							/**< log2 of the horizontal chroma subsampling.  The width must be a multiple of (1 << ui8ChromaShiftX). */
		uint8_t												ui8ChromaShiftY;							/**< log2 of the vertical chroma subsampling.  The height must be a multiple of (1 << ui8ChromaShiftY). */
		bool												bRgb;										/**< If true, the format stores RGB rather than YUV and so needs no color-space conversion. */
	};

	/**
	 * Struct LSN_VIDEO_AUDIO_CODEC_DATA
	 * \brief Information about an audio codec.
	 *
	 * Description: Information about an audio codec.  Bitrates are in kilobits per second and are 0 for lossless codecs.
	 */
	struct LSN_VIDEO_AUDIO_CODEC_DATA {
		const char *										pcEncoder;									/**< The FFmpeg encoder (-c:a). */
		const wchar_t *										pwcName;									/**< The display name. */
		uint32_t											ui32MinBitrate;								/**< The minimum bitrate. */
		uint32_t											ui32MaxBitrate;								/**< The maximum bitrate. */
		uint32_t											ui32BestBitrate;							/**< The bitrate that gives the highest quality.  This is the default. */
		uint32_t											ui32RecommendedBitrate;						/**< A bitrate that gives a good balance between quality and size. */
		bool												bLossless;									/**< If true, the codec is lossless and has no bitrate setting. */
	};

	/**
	 * Struct LSN_VIDEO_FORMAT_DATA
	 * \brief Everything there is to know about a video format.
	 *
	 * Description: Everything there is to know about a video format, both for driving FFmpeg and for building an options dialog.  Members are
	 *	sorted by size.
	 */
	struct LSN_VIDEO_FORMAT_DATA {
		PfConversionFunc									pfConversionFunc[LSN_CF_TOTAL];				/**< Per capture format, the function that prepares a captured frame for FFmpeg.  nullptr if the capture format cannot be used. */
		LSN_VIDEO_QUALITY_DATA								vqdQuality;									/**< The main quality setting (CRF, QP, etc.) */
		LSN_VIDEO_QUALITY_DATA								vqdAltQuality;								/**< The alternative quality setting (usually a bitrate). */
		const LSN_VIDEO_PRESET *							pvpPresets;									/**< The encoder speed presets, fastest to slowest, or nullptr. */
		const wchar_t *										pwcFileExtension;							/**< The file extension, including the period. */
		const char *										pcEncoder;									/**< The FFmpeg encoder (-c:v). */
		const char *										pcMuxer;									/**< The FFmpeg muxer (-f). */
		const char *										pcExtraArgs;								/**< Extra FFmpeg arguments always passed with this format, or nullptr. */
		const char *										pcLosslessArgs;								/**< The FFmpeg arguments that enable lossless encoding (replacing the quality arguments), or nullptr if not supported. */
		uint32_t											ui32PixelFormats;							/**< The supported pixel formats ((1 << LSN_VIDEO_PIXEL_FORMAT) bits). */
		uint32_t											ui32AudioCodecs;							/**< The supported audio codecs ((1 << LSN_VIDEO_AUDIO_CODEC) bits). */
		uint32_t											ui32MaxWidth;								/**< The maximum width in pixels. */
		uint32_t											ui32MaxHeight;								/**< The maximum height in pixels. */
		uint32_t											ui32MaxTimeBaseDen;							/**< The largest time-base denominator the codec accepts, or 0 for no limit.  Frame rates must be expressible within it. */
		uint32_t											ui32TotalPresets;							/**< The number of presets in pvpPresets. */
		uint32_t											ui32DefaultPreset;							/**< The default preset index. */
		LSN_VIDEO_FORMAT									vfType;										/**< The format this describes. */
		LSN_VIDEO_PIXEL_FORMAT								vpfBest;									/**< The highest-quality pixel format.  This is the default. */
		LSN_VIDEO_AUDIO_CODEC								vacBest;									/**< The highest-quality audio codec.  This is the default. */
		bool												bIsHd;										/**< If true, the format is meant for HD (and larger) resolutions. */
		bool												bAllowsFractions;							/**< If true, non-integer frame rates (such as 60.0988) are supported. */
		bool												bVariableFrameRate;							/**< If true, each frame carries its own timestamp so the frame rate can change mid-stream. */
		bool												bAlwaysLossless;							/**< If true, the format is always lossless and has no quality settings. */
		bool												bHardware;									/**< If true, the encoder runs on the GPU and is available only with the matching hardware and drivers. */
	};


	/**
	 * Class CVideoFormat
	 * \brief The database of video formats.
	 *
	 * Description: The database of video formats, along with the functions that prepare captured frames for FFmpeg.
	 */
	class CVideoFormat {
	public :
		// == Functions.
		/**
		 * Gets the display string for a video format.
		 *
		 * \param _vfFormat The video format whose display string is to be returned.
		 * \return Returns the display string for the given video format or an empty string.
		 */
		static const wchar_t *								ToStrPWStr( LSN_VIDEO_FORMAT _vfFormat ) {
			switch ( _vfFormat ) {
#define LSN_VF_ENUM( ENUM, TXT )							case ENUM : { return TXT; }
#include "LSNVideoFormatEnum.inl"
#undef LSN_VF_ENUM
				default : {}
			}
			return L"";
		}

		/**
		 * Gets the display string for a video format.
		 *
		 * \param _vfFormat The video format whose display string is to be returned.
		 * \return Returns the display string for the given video format or an empty string.
		 */
		static std::wstring									ToStrWStr( LSN_VIDEO_FORMAT _vfFormat ) {
			return std::wstring( ToStrPWStr( _vfFormat ) );
		}

		/**
		 * Gets the identifier for a video format.  This is the name of its enumeration value and is used to store settings in a way that
		 *	survives the reordering of the formats.
		 *
		 * \param _vfFormat The video format whose identifier is to be returned.
		 * \return Returns the identifier for the given video format or an empty string.
		 */
		static const char *									IdStr( LSN_VIDEO_FORMAT _vfFormat ) {
			switch ( _vfFormat ) {
#define LSN_VF_ENUM( ENUM, TXT )							case ENUM : { return #ENUM; }
#include "LSNVideoFormatEnum.inl"
#undef LSN_VF_ENUM
				default : {}
			}
			return "";
		}

		/**
		 * Gets a video format by its identifier.
		 *
		 * \param _pcId The identifier as returned by IdStr().
		 * \return Returns the video format with the given identifier or LSN_VF_TOTAL.
		 */
		static LSN_VIDEO_FORMAT								FromIdStr( const char * _pcId );

		/**
		 * Gets the display string for a quality mode.
		 *
		 * \param _vqmMode The quality mode whose display string is to be returned.
		 * \return Returns the display string for the given quality mode.
		 */
		static const wchar_t *								ToStrPWStr( LSN_VIDEO_QUALITY_MODE _vqmMode );

		/**
		 * Gets the information about a video format.
		 *
		 * \param _vfFormat The video format whose information is to be returned.
		 * \return Returns the information about the given video format or nullptr if _vfFormat is invalid.
		 */
		static const LSN_VIDEO_FORMAT_DATA *				Data( LSN_VIDEO_FORMAT _vfFormat );

		/**
		 * Gets the information about a pixel format.
		 *
		 * \param _vpfFormat The pixel format whose information is to be returned.
		 * \return Returns the information about the given pixel format or nullptr if _vpfFormat is invalid.
		 */
		static const LSN_VIDEO_PIXEL_FORMAT_DATA *			PixelFormatData( LSN_VIDEO_PIXEL_FORMAT _vpfFormat );

		/**
		 * Gets a pixel format by its FFmpeg name.
		 *
		 * \param _pcName The FFmpeg name of the pixel format.
		 * \return Returns the pixel format with the given name or LSN_VPF_TOTAL.
		 */
		static LSN_VIDEO_PIXEL_FORMAT						PixelFormatFromFfmpegName( const char * _pcName );

		/**
		 * Gets the information about an audio codec.
		 *
		 * \param _vacCodec The audio codec whose information is to be returned.
		 * \return Returns the information about the given audio codec or nullptr if _vacCodec is invalid.
		 */
		static const LSN_VIDEO_AUDIO_CODEC_DATA *			AudioCodecData( LSN_VIDEO_AUDIO_CODEC _vacCodec );

		/**
		 * Gets an audio codec by its FFmpeg encoder name.
		 *
		 * \param _pcName The FFmpeg encoder name of the audio codec.
		 * \return Returns the audio codec with the given name or LSN_VAC_TOTAL.
		 */
		static LSN_VIDEO_AUDIO_CODEC						AudioCodecFromFfmpegName( const char * _pcName );

		/**
		 * Determines whether a video format supports a given pixel format.
		 *
		 * \param _vfFormat The video format.
		 * \param _vpfFormat The pixel format.
		 * \return Returns true if _vfFormat can be encoded in _vpfFormat.
		 */
		static bool											SupportsPixelFormat( LSN_VIDEO_FORMAT _vfFormat, LSN_VIDEO_PIXEL_FORMAT _vpfFormat ) {
			const LSN_VIDEO_FORMAT_DATA * pvfdData = Data( _vfFormat );
			return pvfdData && _vpfFormat < LSN_VPF_TOTAL && ((*pvfdData).ui32PixelFormats & (1U << _vpfFormat));
		}

		/**
		 * Determines whether a video format supports a given audio codec.
		 *
		 * \param _vfFormat The video format.
		 * \param _vacCodec The audio codec.
		 * \return Returns true if _vfFormat can be paired with _vacCodec.
		 */
		static bool											SupportsAudioCodec( LSN_VIDEO_FORMAT _vfFormat, LSN_VIDEO_AUDIO_CODEC _vacCodec ) {
			const LSN_VIDEO_FORMAT_DATA * pvfdData = Data( _vfFormat );
			return pvfdData && _vacCodec < LSN_VAC_TOTAL && ((*pvfdData).ui32AudioCodecs & (1U << _vacCodec));
		}

		/**
		 * Gets the FFmpeg pixel format (the -pix_fmt of the rawvideo input) that matches a capture format.
		 *
		 * \param _cfFormat The capture format.
		 * \return Returns the FFmpeg name of the pixel format or nullptr if FFmpeg has no matching format.
		 */
		static const char *									FfmpegInputPixelFormat( LSN_CAPTURE_FORMAT _cfFormat );

		/**
		 * Prepares a captured frame for FFmpeg using the conversion function of the given video format.
		 *
		 * \param _vfFormat The video format being exported.
		 * \param _cbSrc The captured frame.
		 * \param _cbScratch A buffer into which to convert the frame if a conversion is needed.
		 * \return Returns the buffer to send to FFmpeg (either &_cbSrc or &_cbScratch) or nullptr.
		 */
		static const LSN_CAPTURE_BUFFER *					Convert( LSN_VIDEO_FORMAT _vfFormat, const LSN_CAPTURE_BUFFER &_cbSrc, LSN_CAPTURE_BUFFER &_cbScratch );


		// == Conversion functions.
		/**
		 * Passes a frame to FFmpeg as-is.  FFmpeg handles the conversion to the output pixel format.
		 *
		 * \param _cbSrc The captured frame.
		 * \param _cbScratch Unused.
		 * \return Returns &_cbSrc, or nullptr if _cbSrc is not valid.
		 */
		static const LSN_CAPTURE_BUFFER *					PassThrough( const LSN_CAPTURE_BUFFER &_cbSrc, LSN_CAPTURE_BUFFER &_cbScratch );

		/**
		 * Converts an LSN_CF_RGBA16F frame to LSN_CF_RGBA16.  Values are clamped to [0..1]; no transfer function is applied.
		 *
		 * \param _cbSrc The captured frame.
		 * \param _cbScratch Receives the converted frame.
		 * \return Returns &_cbScratch, or nullptr if _cbSrc is not an LSN_CF_RGBA16F frame or memory could not be allocated.
		 */
		static const LSN_CAPTURE_BUFFER *					Rgba16FToRgba16( const LSN_CAPTURE_BUFFER &_cbSrc, LSN_CAPTURE_BUFFER &_cbScratch );

		/**
		 * Converts an LSN_CF_RGBA32F frame to LSN_CF_RGBA16.  Values are clamped to [0..1]; no transfer function is applied.
		 *
		 * \param _cbSrc The captured frame.
		 * \param _cbScratch Receives the converted frame.
		 * \return Returns &_cbScratch, or nullptr if _cbSrc is not an LSN_CF_RGBA32F frame or memory could not be allocated.
		 */
		static const LSN_CAPTURE_BUFFER *					Rgba32FToRgba16( const LSN_CAPTURE_BUFFER &_cbSrc, LSN_CAPTURE_BUFFER &_cbScratch );

		/**
		 * Converts a 16-bit float to a 32-bit float.
		 *
		 * \param _ui16Value The bits of the 16-bit float.
		 * \return Returns the 32-bit float.
		 */
		static float										Float16ToFloat32( uint16_t _ui16Value );

		/**
		 * Converts a float in [0..1] to a 16-bit unsigned normalized integer, clamping out-of-range values (NaN becomes 0).
		 *
		 * \param _fValue The value to convert.
		 * \return Returns the 16-bit unsigned normalized value.
		 */
		static inline uint16_t								FloatToUnorm16( float _fValue ) {
			if ( !(_fValue > 0.0f) ) { return 0; }
			if ( _fValue >= 1.0f ) { return 0xFFFF; }
			return static_cast<uint16_t>(_fValue * 65535.0f + 0.5f);
		}
	};

}	// namespace lsn
