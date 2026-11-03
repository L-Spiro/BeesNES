/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The video formats that can be exported via FFmpeg, along with a database of their properties.
 */

#include "LSNVideoFormat.h"

#include <cstring>
#include <iterator>


/** A bit for a pixel format in LSN_VIDEO_FORMAT_DATA::ui32PixelFormats. */
#define LSN_VPF_BIT( NAME )										(1U << LSN_VPF_ ## NAME)

/** A bit for an audio codec in LSN_VIDEO_FORMAT_DATA::ui32AudioCodecs. */
#define LSN_VAC_BIT( NAME )										(1U << LSN_VAC_ ## NAME)

/** The audio codecs that can go into an MP4 file. */
#define LSN_VAC_MP4												(LSN_VAC_BIT( AAC ) | LSN_VAC_BIT( ALAC ) | LSN_VAC_BIT( FLAC ) | LSN_VAC_BIT( OPUS ) | LSN_VAC_BIT( MP3 ))

/**
 * The conversion functions used by most formats, one per LSN_CAPTURE_FORMAT.  FFmpeg takes almost every capture format directly (see
 *	CVideoFormat::FfmpegInputPixelFormat()) and converts it to the output pixel format itself; only the floating-point formats are converted here.
 */
#define LSN_STD_CONVERSIONS																										\
	{																															\
		nullptr,												/* LSN_CF_UNKNOWN */											\
		CVideoFormat::PassThrough,								/* LSN_CF_B5G5R5X1 */											\
		CVideoFormat::PassThrough,								/* LSN_CF_B5G6R5 */												\
		CVideoFormat::PassThrough,								/* LSN_CF_RGB8 */												\
		CVideoFormat::PassThrough,								/* LSN_CF_BGR8 */												\
		CVideoFormat::PassThrough,								/* LSN_CF_RGBA8 */												\
		CVideoFormat::PassThrough,								/* LSN_CF_BGRA8 */												\
		CVideoFormat::PassThrough,								/* LSN_CF_RGBX8 */												\
		CVideoFormat::PassThrough,								/* LSN_CF_BGRX8 */												\
		CVideoFormat::PassThrough,								/* LSN_CF_RGBA8_SRGB */											\
		CVideoFormat::PassThrough,								/* LSN_CF_BGRA8_SRGB */											\
		CVideoFormat::PassThrough,								/* LSN_CF_R10G10B10A2 */										\
		CVideoFormat::PassThrough,								/* LSN_CF_RGBA16 */												\
		CVideoFormat::Rgba16FToRgba16,							/* LSN_CF_RGBA16F */											\
		CVideoFormat::Rgba32FToRgba16,							/* LSN_CF_RGBA32F */											\
	}

namespace lsn {

	// If this fails, a capture format was added or removed.  Update LSN_STD_CONVERSIONS and m_pcInputFormats.
	static_assert( LSN_CF_TOTAL == 15, "LSN_CAPTURE_FORMAT changed; update LSN_STD_CONVERSIONS and the FFmpeg input formats." );

	namespace {

		/** The display names of the ProRes profiles (0 to 5). */
		constexpr const wchar_t * pwcProResProfiles[] = {
			LSN_LSTR( LSN_STR_PRORES_PROXY ),
			LSN_LSTR( LSN_STR_PRORES_LT ),
			LSN_LSTR( LSN_STR_PRORES_STANDARD ),
			LSN_LSTR( LSN_STR_PRORES_HQ ),
			LSN_LSTR( LSN_STR_PRORES_4444 ),
			LSN_LSTR( LSN_STR_PRORES_4444_XQ ),
		};

		/** The x264 and x265 presets, fastest to slowest. */
		constexpr LSN_VIDEO_PRESET vpX26xPresets[] = {
			{ "-preset ultrafast",		"ultrafast" },
			{ "-preset superfast",		"superfast" },
			{ "-preset veryfast",		"veryfast" },
			{ "-preset faster",			"faster" },
			{ "-preset fast",			"fast" },
			{ "-preset medium",			"medium" },
			{ "-preset slow",			"slow" },
			{ "-preset slower",			"slower" },
			{ "-preset veryslow",		"veryslow" },
			{ "-preset placebo",		"placebo" },
		};

		/** The NVENC presets, fastest to slowest. */
		constexpr LSN_VIDEO_PRESET vpNvencPresets[] = {
			{ "-preset p1",				"p1" },
			{ "-preset p2",				"p2" },
			{ "-preset p3",				"p3" },
			{ "-preset p4",				"p4" },
			{ "-preset p5",				"p5" },
			{ "-preset p6",				"p6" },
			{ "-preset p7",				"p7" },
		};

		/** The AMF presets, fastest to slowest. */
		constexpr LSN_VIDEO_PRESET vpAmfPresets[] = {
			{ "-quality speed",			"speed" },
			{ "-quality balanced",		"balanced" },
			{ "-quality quality",		"quality" },
		};

		/** The Quick Sync presets, fastest to slowest. */
		constexpr LSN_VIDEO_PRESET vpQsvPresets[] = {
			{ "-preset veryfast",		"veryfast" },
			{ "-preset faster",			"faster" },
			{ "-preset fast",			"fast" },
			{ "-preset medium",			"medium" },
			{ "-preset slow",			"slow" },
			{ "-preset slower",			"slower" },
			{ "-preset veryslow",		"veryslow" },
		};

		/** The SVT-AV1 presets, fastest to slowest. */
		constexpr LSN_VIDEO_PRESET vpSvtAv1Presets[] = {
			{ "-preset 13",				"13" },
			{ "-preset 12",				"12" },
			{ "-preset 11",				"11" },
			{ "-preset 10",				"10" },
			{ "-preset 9",				"9" },
			{ "-preset 8",				"8" },
			{ "-preset 7",				"7" },
			{ "-preset 6",				"6" },
			{ "-preset 5",				"5" },
			{ "-preset 4",				"4" },
			{ "-preset 3",				"3" },
			{ "-preset 2",				"2" },
			{ "-preset 1",				"1" },
			{ "-preset 0",				"0" },
		};

		/** The libvpx VP9 presets (deadline and cpu-used), fastest to slowest. */
		constexpr LSN_VIDEO_PRESET vpVp9Presets[] = {
			{ "-deadline realtime -cpu-used 8",			"realtime 8" },
			{ "-deadline realtime -cpu-used 7",			"realtime 7" },
			{ "-deadline realtime -cpu-used 6",			"realtime 6" },
			{ "-deadline realtime -cpu-used 5",			"realtime 5" },
			{ "-deadline good -cpu-used 5",				"good 5" },
			{ "-deadline good -cpu-used 4",				"good 4" },
			{ "-deadline good -cpu-used 3",				"good 3" },
			{ "-deadline good -cpu-used 2",				"good 2" },
			{ "-deadline good -cpu-used 1",				"good 1" },
			{ "-deadline good -cpu-used 0",				"good 0" },
			{ "-deadline best -cpu-used 0",				"best" },
		};

		/** The quality setting of formats that do not have one. */
		constexpr LSN_VIDEO_QUALITY_DATA vqdNone = {
			.pcArgs = nullptr,
			.ppwcValueNames = nullptr,
			.i32Min = 0,
			.i32Max = 0,
			.i32Best = 0,
			.i32Recommended = 0,
			.i32Step = 0,
			.vqmMode = LSN_VQM_NONE,
		};

		/** A bitrate setting for formats whose alternative quality setting is a target bitrate. */
		constexpr LSN_VIDEO_QUALITY_DATA vqdBitrate = {
			.pcArgs = "-b:v {Q}k",
			.ppwcValueNames = nullptr,
			.i32Min = 500,
			.i32Max = 150000,
			.i32Best = 150000,
			.i32Recommended = 12000,
			.i32Step = 500,
			.vqmMode = LSN_VQM_BITRATE,
		};

		/**
		 * The video formats, in the same order as LSN_VIDEO_FORMAT.
		 *
		 * Software encoders default to fast presets so that they can keep up with real-time play; their quality settings are already at
		 *	their maximums (mostly lossless), where the preset only trades speed for file size.  Hardware encoders are fast enough to default
		 *	to their highest-quality presets.
		 */
		constexpr LSN_VIDEO_FORMAT_DATA vfdFormats[] = {
			// LSN_VF_H264
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-crf {Q}", .ppwcValueNames = nullptr, .i32Min = 0, .i32Max = 51, .i32Best = 0, .i32Recommended = 18, .i32Step = 1, .vqmMode = LSN_VQM_CRF },
				.vqdAltQuality = vqdBitrate,
				.pvpPresets = vpX26xPresets,
				.pwcFileExtension = L".mp4",
				.pcEncoder = "libx264",
				.pcMuxer = "mp4",
				.pcExtraArgs = nullptr,
				.pcLosslessArgs = "-qp 0",
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ) | LSN_VPF_BIT( YUV422P ) | LSN_VPF_BIT( YUV444P ) | LSN_VPF_BIT( YUV420P10 ) | LSN_VPF_BIT( YUV422P10 ) | LSN_VPF_BIT( YUV444P10 ),
				.ui32AudioCodecs = LSN_VAC_MP4,
				.ui32MaxWidth = 8192,
				.ui32MaxHeight = 4320,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = uint32_t( std::size( vpX26xPresets ) ),
				.ui32DefaultPreset = 2,										// veryfast.
				.vfType = LSN_VF_H264,
				.vpfBest = LSN_VPF_YUV444P,
				.vacBest = LSN_VAC_ALAC,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = false,
			},
			// LSN_VF_H264_NVENC
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-rc constqp -qp {Q}", .ppwcValueNames = nullptr, .i32Min = 0, .i32Max = 51, .i32Best = 0, .i32Recommended = 18, .i32Step = 1, .vqmMode = LSN_VQM_QP },
				.vqdAltQuality = { .pcArgs = "-rc vbr -b:v {Q}k", .ppwcValueNames = nullptr, .i32Min = 500, .i32Max = 150000, .i32Best = 150000, .i32Recommended = 12000, .i32Step = 500, .vqmMode = LSN_VQM_BITRATE },
				.pvpPresets = vpNvencPresets,
				.pwcFileExtension = L".mp4",
				.pcEncoder = "h264_nvenc",
				.pcMuxer = "mp4",
				.pcExtraArgs = nullptr,
				.pcLosslessArgs = "-tune lossless",
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ) | LSN_VPF_BIT( YUV444P ),
				.ui32AudioCodecs = LSN_VAC_MP4,
				.ui32MaxWidth = 4096,
				.ui32MaxHeight = 4096,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = uint32_t( std::size( vpNvencPresets ) ),
				.ui32DefaultPreset = 6,										// p7.
				.vfType = LSN_VF_H264_NVENC,
				.vpfBest = LSN_VPF_YUV444P,
				.vacBest = LSN_VAC_ALAC,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = true,
			},
			// LSN_VF_H264_AMF
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-rc cqp -qp_i {Q} -qp_p {Q} -qp_b {Q}", .ppwcValueNames = nullptr, .i32Min = 0, .i32Max = 51, .i32Best = 0, .i32Recommended = 18, .i32Step = 1, .vqmMode = LSN_VQM_QP },
				.vqdAltQuality = { .pcArgs = "-rc cbr -b:v {Q}k", .ppwcValueNames = nullptr, .i32Min = 500, .i32Max = 100000, .i32Best = 100000, .i32Recommended = 12000, .i32Step = 500, .vqmMode = LSN_VQM_BITRATE },
				.pvpPresets = vpAmfPresets,
				.pwcFileExtension = L".mp4",
				.pcEncoder = "h264_amf",
				.pcMuxer = "mp4",
				.pcExtraArgs = nullptr,
				.pcLosslessArgs = nullptr,
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ) | LSN_VPF_BIT( NV12 ),
				.ui32AudioCodecs = LSN_VAC_MP4,
				.ui32MaxWidth = 4096,
				.ui32MaxHeight = 2160,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = uint32_t( std::size( vpAmfPresets ) ),
				.ui32DefaultPreset = 2,										// quality.
				.vfType = LSN_VF_H264_AMF,
				.vpfBest = LSN_VPF_YUV420P,
				.vacBest = LSN_VAC_ALAC,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = true,
			},
			// LSN_VF_H264_QSV
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-global_quality {Q}", .ppwcValueNames = nullptr, .i32Min = 1, .i32Max = 51, .i32Best = 1, .i32Recommended = 20, .i32Step = 1, .vqmMode = LSN_VQM_ICQ },
				.vqdAltQuality = vqdBitrate,
				.pvpPresets = vpQsvPresets,
				.pwcFileExtension = L".mp4",
				.pcEncoder = "h264_qsv",
				.pcMuxer = "mp4",
				.pcExtraArgs = nullptr,
				.pcLosslessArgs = nullptr,
				.ui32PixelFormats = LSN_VPF_BIT( NV12 ),
				.ui32AudioCodecs = LSN_VAC_MP4,
				.ui32MaxWidth = 4096,
				.ui32MaxHeight = 4096,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = uint32_t( std::size( vpQsvPresets ) ),
				.ui32DefaultPreset = 6,										// veryslow.
				.vfType = LSN_VF_H264_QSV,
				.vpfBest = LSN_VPF_NV12,
				.vacBest = LSN_VAC_ALAC,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = true,
			},
			// LSN_VF_HEVC
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-crf {Q}", .ppwcValueNames = nullptr, .i32Min = 0, .i32Max = 51, .i32Best = 0, .i32Recommended = 20, .i32Step = 1, .vqmMode = LSN_VQM_CRF },
				.vqdAltQuality = vqdBitrate,
				.pvpPresets = vpX26xPresets,
				.pwcFileExtension = L".mp4",
				.pcEncoder = "libx265",
				.pcMuxer = "mp4",
				.pcExtraArgs = "-tag:v hvc1",
				.pcLosslessArgs = "-x265-params lossless=1",
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ) | LSN_VPF_BIT( YUV422P ) | LSN_VPF_BIT( YUV444P ) | LSN_VPF_BIT( YUV420P10 ) | LSN_VPF_BIT( YUV422P10 ) | LSN_VPF_BIT( YUV444P10 ),
				.ui32AudioCodecs = LSN_VAC_MP4,
				.ui32MaxWidth = 8192,
				.ui32MaxHeight = 4320,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = uint32_t( std::size( vpX26xPresets ) ),
				.ui32DefaultPreset = 0,										// ultrafast.
				.vfType = LSN_VF_HEVC,
				.vpfBest = LSN_VPF_YUV444P,
				.vacBest = LSN_VAC_ALAC,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = false,
			},
			// LSN_VF_HEVC_NVENC
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-rc constqp -qp {Q}", .ppwcValueNames = nullptr, .i32Min = 0, .i32Max = 51, .i32Best = 0, .i32Recommended = 20, .i32Step = 1, .vqmMode = LSN_VQM_QP },
				.vqdAltQuality = { .pcArgs = "-rc vbr -b:v {Q}k", .ppwcValueNames = nullptr, .i32Min = 500, .i32Max = 150000, .i32Best = 150000, .i32Recommended = 10000, .i32Step = 500, .vqmMode = LSN_VQM_BITRATE },
				.pvpPresets = vpNvencPresets,
				.pwcFileExtension = L".mp4",
				.pcEncoder = "hevc_nvenc",
				.pcMuxer = "mp4",
				.pcExtraArgs = "-tag:v hvc1",
				.pcLosslessArgs = "-tune lossless",
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ) | LSN_VPF_BIT( YUV444P ) | LSN_VPF_BIT( P010 ) | LSN_VPF_BIT( YUV444P16 ),
				.ui32AudioCodecs = LSN_VAC_MP4,
				.ui32MaxWidth = 8192,
				.ui32MaxHeight = 8192,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = uint32_t( std::size( vpNvencPresets ) ),
				.ui32DefaultPreset = 6,										// p7.
				.vfType = LSN_VF_HEVC_NVENC,
				.vpfBest = LSN_VPF_YUV444P,
				.vacBest = LSN_VAC_ALAC,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = true,
			},
			// LSN_VF_HEVC_AMF
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-rc cqp -qp_i {Q} -qp_p {Q}", .ppwcValueNames = nullptr, .i32Min = 0, .i32Max = 51, .i32Best = 0, .i32Recommended = 20, .i32Step = 1, .vqmMode = LSN_VQM_QP },
				.vqdAltQuality = { .pcArgs = "-rc cbr -b:v {Q}k", .ppwcValueNames = nullptr, .i32Min = 500, .i32Max = 100000, .i32Best = 100000, .i32Recommended = 10000, .i32Step = 500, .vqmMode = LSN_VQM_BITRATE },
				.pvpPresets = vpAmfPresets,
				.pwcFileExtension = L".mp4",
				.pcEncoder = "hevc_amf",
				.pcMuxer = "mp4",
				.pcExtraArgs = "-tag:v hvc1",
				.pcLosslessArgs = nullptr,
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ) | LSN_VPF_BIT( NV12 ) | LSN_VPF_BIT( P010 ),
				.ui32AudioCodecs = LSN_VAC_MP4,
				.ui32MaxWidth = 4096,
				.ui32MaxHeight = 2160,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = uint32_t( std::size( vpAmfPresets ) ),
				.ui32DefaultPreset = 2,										// quality.
				.vfType = LSN_VF_HEVC_AMF,
				.vpfBest = LSN_VPF_P010,
				.vacBest = LSN_VAC_ALAC,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = true,
			},
			// LSN_VF_HEVC_QSV
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-global_quality {Q}", .ppwcValueNames = nullptr, .i32Min = 1, .i32Max = 51, .i32Best = 1, .i32Recommended = 22, .i32Step = 1, .vqmMode = LSN_VQM_ICQ },
				.vqdAltQuality = vqdBitrate,
				.pvpPresets = vpQsvPresets,
				.pwcFileExtension = L".mp4",
				.pcEncoder = "hevc_qsv",
				.pcMuxer = "mp4",
				.pcExtraArgs = "-tag:v hvc1",
				.pcLosslessArgs = nullptr,
				.ui32PixelFormats = LSN_VPF_BIT( NV12 ) | LSN_VPF_BIT( P010 ),
				.ui32AudioCodecs = LSN_VAC_MP4,
				.ui32MaxWidth = 8192,
				.ui32MaxHeight = 8192,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = uint32_t( std::size( vpQsvPresets ) ),
				.ui32DefaultPreset = 6,										// veryslow.
				.vfType = LSN_VF_HEVC_QSV,
				.vpfBest = LSN_VPF_P010,
				.vacBest = LSN_VAC_ALAC,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = true,
			},
			// LSN_VF_AV1
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-crf {Q}", .ppwcValueNames = nullptr, .i32Min = 1, .i32Max = 63, .i32Best = 1, .i32Recommended = 30, .i32Step = 1, .vqmMode = LSN_VQM_CRF },
				.vqdAltQuality = vqdBitrate,
				.pvpPresets = vpSvtAv1Presets,
				.pwcFileExtension = L".mp4",
				.pcEncoder = "libsvtav1",
				.pcMuxer = "mp4",
				.pcExtraArgs = nullptr,
				.pcLosslessArgs = nullptr,
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ) | LSN_VPF_BIT( YUV420P10 ),
				.ui32AudioCodecs = LSN_VAC_MP4,
				.ui32MaxWidth = 16384,
				.ui32MaxHeight = 8704,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = uint32_t( std::size( vpSvtAv1Presets ) ),
				.ui32DefaultPreset = 3,										// 10.
				.vfType = LSN_VF_AV1,
				.vpfBest = LSN_VPF_YUV420P10,
				.vacBest = LSN_VAC_ALAC,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = false,
			},
			// LSN_VF_AV1_NVENC
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-rc vbr -cq {Q} -b:v 0", .ppwcValueNames = nullptr, .i32Min = 1, .i32Max = 63, .i32Best = 1, .i32Recommended = 30, .i32Step = 1, .vqmMode = LSN_VQM_CQ },
				.vqdAltQuality = { .pcArgs = "-rc vbr -b:v {Q}k", .ppwcValueNames = nullptr, .i32Min = 500, .i32Max = 150000, .i32Best = 150000, .i32Recommended = 8000, .i32Step = 500, .vqmMode = LSN_VQM_BITRATE },
				.pvpPresets = vpNvencPresets,
				.pwcFileExtension = L".mp4",
				.pcEncoder = "av1_nvenc",
				.pcMuxer = "mp4",
				.pcExtraArgs = nullptr,
				.pcLosslessArgs = nullptr,
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ) | LSN_VPF_BIT( P010 ),
				.ui32AudioCodecs = LSN_VAC_MP4,
				.ui32MaxWidth = 8192,
				.ui32MaxHeight = 8192,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = uint32_t( std::size( vpNvencPresets ) ),
				.ui32DefaultPreset = 6,										// p7.
				.vfType = LSN_VF_AV1_NVENC,
				.vpfBest = LSN_VPF_P010,
				.vacBest = LSN_VAC_ALAC,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = true,
			},
			// LSN_VF_VP9
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-crf {Q} -b:v 0", .ppwcValueNames = nullptr, .i32Min = 0, .i32Max = 63, .i32Best = 0, .i32Recommended = 31, .i32Step = 1, .vqmMode = LSN_VQM_CRF },
				.vqdAltQuality = vqdBitrate,
				.pvpPresets = vpVp9Presets,
				.pwcFileExtension = L".webm",
				.pcEncoder = "libvpx-vp9",
				.pcMuxer = "webm",
				.pcExtraArgs = "-row-mt 1",
				.pcLosslessArgs = "-lossless 1",
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ) | LSN_VPF_BIT( YUV422P ) | LSN_VPF_BIT( YUV444P ) | LSN_VPF_BIT( YUV420P10 ) | LSN_VPF_BIT( YUV422P10 ) | LSN_VPF_BIT( YUV444P10 ),
				.ui32AudioCodecs = LSN_VAC_BIT( OPUS ),
				.ui32MaxWidth = 16384,
				.ui32MaxHeight = 16384,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = uint32_t( std::size( vpVp9Presets ) ),
				.ui32DefaultPreset = 0,										// realtime 8.
				.vfType = LSN_VF_VP9,
				.vpfBest = LSN_VPF_YUV444P,
				.vacBest = LSN_VAC_OPUS,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = false,
			},
			// LSN_VF_PRORES
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-profile:v {Q}", .ppwcValueNames = pwcProResProfiles, .i32Min = 0, .i32Max = 5, .i32Best = 5, .i32Recommended = 3, .i32Step = 1, .vqmMode = LSN_VQM_PROFILE },
				.vqdAltQuality = vqdNone,
				.pvpPresets = nullptr,
				.pwcFileExtension = L".mov",
				.pcEncoder = "prores_ks",
				.pcMuxer = "mov",
				.pcExtraArgs = "-vendor apl0",
				.pcLosslessArgs = nullptr,
				.ui32PixelFormats = LSN_VPF_BIT( YUV422P10 ) | LSN_VPF_BIT( YUV444P10 ),			// Profiles 0-3 are 4:2:2; 4 and 5 are 4:4:4.
				.ui32AudioCodecs = LSN_VAC_BIT( AAC ) | LSN_VAC_BIT( ALAC ) | LSN_VAC_BIT( PCM_S16 ) | LSN_VAC_BIT( PCM_S24 ),
				.ui32MaxWidth = 8192,
				.ui32MaxHeight = 4320,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = 0,
				.ui32DefaultPreset = 0,
				.vfType = LSN_VF_PRORES,
				.vpfBest = LSN_VPF_YUV444P10,
				.vacBest = LSN_VAC_PCM_S24,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = false,
			},
			// LSN_VF_FFV1
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = vqdNone,
				.vqdAltQuality = vqdNone,
				.pvpPresets = nullptr,
				.pwcFileExtension = L".mkv",
				.pcEncoder = "ffv1",
				.pcMuxer = "matroska",
				.pcExtraArgs = "-level 3 -g 1 -slices 16 -slicecrc 1",
				.pcLosslessArgs = nullptr,
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ) | LSN_VPF_BIT( YUV422P ) | LSN_VPF_BIT( YUV444P ) | LSN_VPF_BIT( YUV420P10 ) | LSN_VPF_BIT( YUV422P10 ) | LSN_VPF_BIT( YUV444P10 ) | LSN_VPF_BIT( GBRP ) | LSN_VPF_BIT( GBRP10 ),
				.ui32AudioCodecs = LSN_VAC_BIT( FLAC ) | LSN_VAC_BIT( PCM_S16 ) | LSN_VAC_BIT( PCM_S24 ) | LSN_VAC_BIT( PCM_F32 ),
				.ui32MaxWidth = 16384,
				.ui32MaxHeight = 16384,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = 0,
				.ui32DefaultPreset = 0,
				.vfType = LSN_VF_FFV1,
				.vpfBest = LSN_VPF_GBRP,
				.vacBest = LSN_VAC_FLAC,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = true,
				.bHardware = false,
			},
			// LSN_VF_UTVIDEO
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = vqdNone,
				.vqdAltQuality = vqdNone,
				.pvpPresets = nullptr,
				.pwcFileExtension = L".avi",
				.pcEncoder = "utvideo",
				.pcMuxer = "avi",
				.pcExtraArgs = "-pred median",
				.pcLosslessArgs = nullptr,
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ) | LSN_VPF_BIT( YUV422P ) | LSN_VPF_BIT( YUV444P ) | LSN_VPF_BIT( GBRP ),
				.ui32AudioCodecs = LSN_VAC_BIT( PCM_S16 ) | LSN_VAC_BIT( PCM_S24 ),
				.ui32MaxWidth = 16384,
				.ui32MaxHeight = 16384,
				.ui32MaxTimeBaseDen = 0,
				.ui32TotalPresets = 0,
				.ui32DefaultPreset = 0,
				.vfType = LSN_VF_UTVIDEO,
				.vpfBest = LSN_VPF_GBRP,
				.vacBest = LSN_VAC_PCM_S24,
				.bIsHd = true,
				.bAllowsFractions = true,
				.bVariableFrameRate = false,								// AVI has a single fixed rate (though any fraction).
				.bAlwaysLossless = true,
				.bHardware = false,
			},
			// LSN_VF_MPEG4
			{
				.pfConversionFunc = LSN_STD_CONVERSIONS,
				.vqdQuality = { .pcArgs = "-q:v {Q}", .ppwcValueNames = nullptr, .i32Min = 1, .i32Max = 31, .i32Best = 1, .i32Recommended = 3, .i32Step = 1, .vqmMode = LSN_VQM_QSCALE },
				.vqdAltQuality = { .pcArgs = "-b:v {Q}k", .ppwcValueNames = nullptr, .i32Min = 300, .i32Max = 50000, .i32Best = 50000, .i32Recommended = 8000, .i32Step = 100, .vqmMode = LSN_VQM_BITRATE },
				.pvpPresets = nullptr,
				.pwcFileExtension = L".mp4",
				.pcEncoder = "mpeg4",
				.pcMuxer = "mp4",
				.pcExtraArgs = "-mbd rd -flags +mv4+aic -trellis 2",
				.pcLosslessArgs = nullptr,
				.ui32PixelFormats = LSN_VPF_BIT( YUV420P ),
				.ui32AudioCodecs = LSN_VAC_BIT( AAC ) | LSN_VAC_BIT( ALAC ) | LSN_VAC_BIT( MP3 ),
				.ui32MaxWidth = 8190,
				.ui32MaxHeight = 8190,
				.ui32MaxTimeBaseDen = 65535,								// The VOP time increment resolution is 16 bits.
				.ui32TotalPresets = 0,
				.ui32DefaultPreset = 0,
				.vfType = LSN_VF_MPEG4,
				.vpfBest = LSN_VPF_YUV420P,
				.vacBest = LSN_VAC_ALAC,
				.bIsHd = false,												// Its profiles top out at standard definition; larger frames work but many players reject them.
				.bAllowsFractions = true,
				.bVariableFrameRate = true,
				.bAlwaysLossless = false,
				.bHardware = false,
			},
		};

		/** The pixel formats, in the same order as LSN_VIDEO_PIXEL_FORMAT. */
		constexpr LSN_VIDEO_PIXEL_FORMAT_DATA vpfdPixelFormats[] = {
			{ "yuv420p",			LSN_LSTR( LSN_STR_VPF_YUV420P ),		8,		1,		1,		false },
			{ "yuv422p",			LSN_LSTR( LSN_STR_VPF_YUV422P ),		8,		1,		0,		false },
			{ "yuv444p",			LSN_LSTR( LSN_STR_VPF_YUV444P ),		8,		0,		0,		false },
			{ "nv12",				LSN_LSTR( LSN_STR_VPF_NV12 ),			8,		1,		1,		false },
			{ "p010le",				LSN_LSTR( LSN_STR_VPF_P010 ),			10,		1,		1,		false },
			{ "yuv420p10le",		LSN_LSTR( LSN_STR_VPF_YUV420P10 ),		10,		1,		1,		false },
			{ "yuv422p10le",		LSN_LSTR( LSN_STR_VPF_YUV422P10 ),		10,		1,		0,		false },
			{ "yuv444p10le",		LSN_LSTR( LSN_STR_VPF_YUV444P10 ),		10,		0,		0,		false },
			{ "yuv444p16le",		LSN_LSTR( LSN_STR_VPF_YUV444P16 ),		16,		0,		0,		false },
			{ "gbrp",				LSN_LSTR( LSN_STR_VPF_GBRP ),			8,		0,		0,		true },
			{ "gbrp10le",			LSN_LSTR( LSN_STR_VPF_GBRP10 ),			10,		0,		0,		true },
		};

		/** The audio codecs, in the same order as LSN_VIDEO_AUDIO_CODEC.  Bitrates are in kilobits per second. */
		constexpr LSN_VIDEO_AUDIO_CODEC_DATA vacdAudioCodecs[] = {
			{ "aac",				LSN_LSTR( LSN_STR_VAC_AAC ),			32,		320,	320,	192,	false },
			{ "alac",				LSN_LSTR( LSN_STR_VAC_ALAC ),			0,		0,		0,		0,		true },
			{ "flac",				LSN_LSTR( LSN_STR_VAC_FLAC ),			0,		0,		0,		0,		true },
			{ "libopus",			LSN_LSTR( LSN_STR_VAC_OPUS ),			32,		256,	256,	160,	false },				// libopus allows up to 256 kbps per channel; BeesNES is mono.
			{ "libmp3lame",			LSN_LSTR( LSN_STR_VAC_MP3 ),			32,		320,	320,	192,	false },
			{ "pcm_s16le",			LSN_LSTR( LSN_STR_VAC_PCM_S16 ),		0,		0,		0,		0,		true },
			{ "pcm_s24le",			LSN_LSTR( LSN_STR_VAC_PCM_S24 ),		0,		0,		0,		0,		true },
			{ "pcm_f32le",			LSN_LSTR( LSN_STR_VAC_PCM_F32 ),		0,		0,		0,		0,		true },
		};

		/** The FFmpeg input pixel format for each capture format, in the same order as LSN_CAPTURE_FORMAT. */
		constexpr const char * pcInputFormats[] = {
			nullptr,												// LSN_CF_UNKNOWN
			"rgb555le",												// LSN_CF_B5G5R5X1
			"rgb565le",												// LSN_CF_B5G6R5
			"rgb24",												// LSN_CF_RGB8
			"bgr24",												// LSN_CF_BGR8
			"rgba",													// LSN_CF_RGBA8
			"bgra",													// LSN_CF_BGRA8
			"rgb0",													// LSN_CF_RGBX8
			"bgr0",													// LSN_CF_BGRX8
			"rgba",													// LSN_CF_RGBA8_SRGB
			"bgra",													// LSN_CF_BGRA8_SRGB
			"x2bgr10le",											// LSN_CF_R10G10B10A2
			"rgba64le",												// LSN_CF_RGBA16
			"rgbaf16le",											// LSN_CF_RGBA16F (converted to LSN_CF_RGBA16 by default since older FFmpeg builds cannot read it).
			"rgbaf32le",											// LSN_CF_RGBA32F (converted to LSN_CF_RGBA16 by default since FFmpeg's scaler cannot read it).
		};

		/**
		 * Verifies that the tables are in the same order as their enumerations.
		 *
		 * \return Returns true if every entry is where it belongs.
		 */
		consteval bool										TablesAreOrdered() {
			for ( size_t I = 0; I < std::size( vfdFormats ); ++I ) {
				if ( vfdFormats[I].vfType != LSN_VIDEO_FORMAT( I ) ) { return false; }
				if ( !(vfdFormats[I].ui32PixelFormats & (1U << vfdFormats[I].vpfBest)) ) { return false; }
				if ( !(vfdFormats[I].ui32AudioCodecs & (1U << vfdFormats[I].vacBest)) ) { return false; }
				if ( vfdFormats[I].ui32TotalPresets && vfdFormats[I].ui32DefaultPreset >= vfdFormats[I].ui32TotalPresets ) { return false; }
				if ( !vfdFormats[I].ui32TotalPresets != !vfdFormats[I].pvpPresets ) { return false; }
			}
			return true;
		}

		static_assert( std::size( vfdFormats ) == LSN_VF_TOTAL, "The video-format table does not match LSN_VIDEO_FORMAT." );
		static_assert( std::size( vpfdPixelFormats ) == LSN_VPF_TOTAL, "The pixel-format table does not match LSN_VIDEO_PIXEL_FORMAT." );
		static_assert( std::size( vacdAudioCodecs ) == LSN_VAC_TOTAL, "The audio-codec table does not match LSN_VIDEO_AUDIO_CODEC." );
		static_assert( std::size( pcInputFormats ) == LSN_CF_TOTAL, "The FFmpeg input-format table does not match LSN_CAPTURE_FORMAT." );
		static_assert( TablesAreOrdered(), "The video-format table is out of order or has an invalid default." );

	}	// namespace


	// == Functions.
	/**
	 * Gets a video format by its identifier.
	 *
	 * \param _pcId The identifier as returned by IdStr().
	 * \return Returns the video format with the given identifier or LSN_VF_TOTAL.
	 */
	LSN_VIDEO_FORMAT CVideoFormat::FromIdStr( const char * _pcId ) {
		if ( !_pcId ) { return LSN_VF_TOTAL; }
		for ( uint16_t I = 0; I < LSN_VF_TOTAL; ++I ) {
			if ( std::strcmp( IdStr( LSN_VIDEO_FORMAT( I ) ), _pcId ) == 0 ) { return LSN_VIDEO_FORMAT( I ); }
		}
		return LSN_VF_TOTAL;
	}

	/**
	 * Gets the display string for a quality mode.
	 *
	 * \param _vqmMode The quality mode whose display string is to be returned.
	 * \return Returns the display string for the given quality mode.
	 */
	const wchar_t * CVideoFormat::ToStrPWStr( LSN_VIDEO_QUALITY_MODE _vqmMode ) {
		switch ( _vqmMode ) {
			case LSN_VQM_CRF : { return LSN_LSTR( LSN_STR_VQM_CRF ); }
			case LSN_VQM_QP : { return LSN_LSTR( LSN_STR_VQM_QP ); }
			case LSN_VQM_CQ : { return LSN_LSTR( LSN_STR_VQM_CQ ); }
			case LSN_VQM_ICQ : { return LSN_LSTR( LSN_STR_VQM_ICQ ); }
			case LSN_VQM_QSCALE : { return LSN_LSTR( LSN_STR_VQM_QSCALE ); }
			case LSN_VQM_PROFILE : { return LSN_LSTR( LSN_STR_VQM_PROFILE ); }
			case LSN_VQM_BITRATE : { return LSN_LSTR( LSN_STR_VQM_BITRATE ); }
			default : { return LSN_LSTR( LSN_STR_VQM_NONE ); }
		}
	}

	/**
	 * Gets the information about a video format.
	 *
	 * \param _vfFormat The video format whose information is to be returned.
	 * \return Returns the information about the given video format or nullptr if _vfFormat is invalid.
	 */
	const LSN_VIDEO_FORMAT_DATA * CVideoFormat::Data( LSN_VIDEO_FORMAT _vfFormat ) {
		if ( _vfFormat >= LSN_VF_TOTAL ) { return nullptr; }
		return &vfdFormats[_vfFormat];
	}

	/**
	 * Gets the information about a pixel format.
	 *
	 * \param _vpfFormat The pixel format whose information is to be returned.
	 * \return Returns the information about the given pixel format or nullptr if _vpfFormat is invalid.
	 */
	const LSN_VIDEO_PIXEL_FORMAT_DATA * CVideoFormat::PixelFormatData( LSN_VIDEO_PIXEL_FORMAT _vpfFormat ) {
		if ( _vpfFormat >= LSN_VPF_TOTAL ) { return nullptr; }
		return &vpfdPixelFormats[_vpfFormat];
	}

	/**
	 * Gets a pixel format by its FFmpeg name.
	 *
	 * \param _pcName The FFmpeg name of the pixel format.
	 * \return Returns the pixel format with the given name or LSN_VPF_TOTAL.
	 */
	LSN_VIDEO_PIXEL_FORMAT CVideoFormat::PixelFormatFromFfmpegName( const char * _pcName ) {
		if ( !_pcName ) { return LSN_VPF_TOTAL; }
		for ( size_t I = 0; I < std::size( vpfdPixelFormats ); ++I ) {
			if ( std::strcmp( vpfdPixelFormats[I].pcFfmpegName, _pcName ) == 0 ) { return LSN_VIDEO_PIXEL_FORMAT( I ); }
		}
		return LSN_VPF_TOTAL;
	}

	/**
	 * Gets the information about an audio codec.
	 *
	 * \param _vacCodec The audio codec whose information is to be returned.
	 * \return Returns the information about the given audio codec or nullptr if _vacCodec is invalid.
	 */
	const LSN_VIDEO_AUDIO_CODEC_DATA * CVideoFormat::AudioCodecData( LSN_VIDEO_AUDIO_CODEC _vacCodec ) {
		if ( _vacCodec >= LSN_VAC_TOTAL ) { return nullptr; }
		return &vacdAudioCodecs[_vacCodec];
	}

	/**
	 * Gets an audio codec by its FFmpeg encoder name.
	 *
	 * \param _pcName The FFmpeg encoder name of the audio codec.
	 * \return Returns the audio codec with the given name or LSN_VAC_TOTAL.
	 */
	LSN_VIDEO_AUDIO_CODEC CVideoFormat::AudioCodecFromFfmpegName( const char * _pcName ) {
		if ( !_pcName ) { return LSN_VAC_TOTAL; }
		for ( size_t I = 0; I < std::size( vacdAudioCodecs ); ++I ) {
			if ( std::strcmp( vacdAudioCodecs[I].pcEncoder, _pcName ) == 0 ) { return LSN_VIDEO_AUDIO_CODEC( I ); }
		}
		return LSN_VAC_TOTAL;
	}

	/**
	 * Gets the FFmpeg pixel format (the -pix_fmt of the rawvideo input) that matches a capture format.
	 *
	 * \param _cfFormat The capture format.
	 * \return Returns the FFmpeg name of the pixel format or nullptr if FFmpeg has no matching format.
	 */
	const char * CVideoFormat::FfmpegInputPixelFormat( LSN_CAPTURE_FORMAT _cfFormat ) {
		if ( _cfFormat >= LSN_CF_TOTAL ) { return nullptr; }
		return pcInputFormats[_cfFormat];
	}

	/**
	 * Prepares a captured frame for FFmpeg using the conversion function of the given video format.
	 *
	 * \param _vfFormat The video format being exported.
	 * \param _cbSrc The captured frame.
	 * \param _cbScratch A buffer into which to convert the frame if a conversion is needed.
	 * \return Returns the buffer to send to FFmpeg (either &_cbSrc or &_cbScratch) or nullptr.
	 */
	const LSN_CAPTURE_BUFFER * CVideoFormat::Convert( LSN_VIDEO_FORMAT _vfFormat, const LSN_CAPTURE_BUFFER &_cbSrc, LSN_CAPTURE_BUFFER &_cbScratch ) {
		const LSN_VIDEO_FORMAT_DATA * pvfdData = Data( _vfFormat );
		if ( !pvfdData || _cbSrc.cfFormat >= LSN_CF_TOTAL ) { return nullptr; }
		PfConversionFunc pfFunc = (*pvfdData).pfConversionFunc[_cbSrc.cfFormat];
		if ( !pfFunc ) { return nullptr; }
		return pfFunc( _cbSrc, _cbScratch );
	}

	/**
	 * Passes a frame to FFmpeg as-is.  FFmpeg handles the conversion to the output pixel format.
	 *
	 * \param _cbSrc The captured frame.
	 * \param _cbScratch Unused.
	 * \return Returns &_cbSrc, or nullptr if _cbSrc is not valid.
	 */
	const LSN_CAPTURE_BUFFER * CVideoFormat::PassThrough( const LSN_CAPTURE_BUFFER &_cbSrc, LSN_CAPTURE_BUFFER &/*_cbScratch*/ ) {
		return _cbSrc.Valid() ? &_cbSrc : nullptr;
	}

	/**
	 * Converts an LSN_CF_RGBA16F frame to LSN_CF_RGBA16.  Values are clamped to [0..1]; no transfer function is applied.
	 *
	 * \param _cbSrc The captured frame.
	 * \param _cbScratch Receives the converted frame.
	 * \return Returns &_cbScratch, or nullptr if _cbSrc is not an LSN_CF_RGBA16F frame or memory could not be allocated.
	 */
	const LSN_CAPTURE_BUFFER * CVideoFormat::Rgba16FToRgba16( const LSN_CAPTURE_BUFFER &_cbSrc, LSN_CAPTURE_BUFFER &_cbScratch ) {
		if ( !_cbSrc.Valid() || _cbSrc.cfFormat != LSN_CF_RGBA16F ) { return nullptr; }
		if ( !_cbScratch.Allocate( _cbSrc.ui32Width, _cbSrc.ui32Height, _cbSrc.ui32Width * 8, LSN_CF_RGBA16 ) ) { return nullptr; }
		size_t sComponents = size_t( _cbSrc.ui32Width ) * 4;
		for ( uint32_t Y = 0; Y < _cbSrc.ui32Height; ++Y ) {
			const uint16_t * pui16Src = reinterpret_cast<const uint16_t *>(&_cbSrc.vBuffer[size_t( Y )*_cbSrc.ui32Pitch]);
			uint16_t * pui16Dst = reinterpret_cast<uint16_t *>(&_cbScratch.vBuffer[size_t( Y )*_cbScratch.ui32Pitch]);
			for ( size_t I = 0; I < sComponents; ++I ) {
				pui16Dst[I] = FloatToUnorm16( Float16ToFloat32( pui16Src[I] ) );
			}
		}
		_cbScratch.ui64PpuFrame = _cbSrc.ui64PpuFrame;
		_cbScratch.ui64Captures = _cbSrc.ui64Captures;
		return &_cbScratch;
	}

	/**
	 * Converts an LSN_CF_RGBA32F frame to LSN_CF_RGBA16.  Values are clamped to [0..1]; no transfer function is applied.
	 *
	 * \param _cbSrc The captured frame.
	 * \param _cbScratch Receives the converted frame.
	 * \return Returns &_cbScratch, or nullptr if _cbSrc is not an LSN_CF_RGBA32F frame or memory could not be allocated.
	 */
	const LSN_CAPTURE_BUFFER * CVideoFormat::Rgba32FToRgba16( const LSN_CAPTURE_BUFFER &_cbSrc, LSN_CAPTURE_BUFFER &_cbScratch ) {
		if ( !_cbSrc.Valid() || _cbSrc.cfFormat != LSN_CF_RGBA32F ) { return nullptr; }
		if ( !_cbScratch.Allocate( _cbSrc.ui32Width, _cbSrc.ui32Height, _cbSrc.ui32Width * 8, LSN_CF_RGBA16 ) ) { return nullptr; }
		size_t sComponents = size_t( _cbSrc.ui32Width ) * 4;
		for ( uint32_t Y = 0; Y < _cbSrc.ui32Height; ++Y ) {
			const float * pfSrc = reinterpret_cast<const float *>(&_cbSrc.vBuffer[size_t( Y )*_cbSrc.ui32Pitch]);
			uint16_t * pui16Dst = reinterpret_cast<uint16_t *>(&_cbScratch.vBuffer[size_t( Y )*_cbScratch.ui32Pitch]);
			for ( size_t I = 0; I < sComponents; ++I ) {
				pui16Dst[I] = FloatToUnorm16( pfSrc[I] );
			}
		}
		_cbScratch.ui64PpuFrame = _cbSrc.ui64PpuFrame;
		_cbScratch.ui64Captures = _cbSrc.ui64Captures;
		return &_cbScratch;
	}

	/**
	 * Converts a 16-bit float to a 32-bit float.
	 *
	 * \param _ui16Value The bits of the 16-bit float.
	 * \return Returns the 32-bit float.
	 */
	float CVideoFormat::Float16ToFloat32( uint16_t _ui16Value ) {
		uint32_t ui32Sign = uint32_t( _ui16Value & 0x8000 ) << 16;
		uint32_t ui32Exp = (_ui16Value >> 10) & 0x1F;
		uint32_t ui32Mant = _ui16Value & 0x3FF;
		uint32_t ui32Bits;
		if ( ui32Exp == 0x1F ) {
			// Infinity or NaN.
			ui32Bits = ui32Sign | 0x7F800000 | (ui32Mant << 13);
		}
		else if ( ui32Exp != 0 ) {
			// Normal.
			ui32Bits = ui32Sign | ((ui32Exp + (127 - 15)) << 23) | (ui32Mant << 13);
		}
		else if ( ui32Mant == 0 ) {
			// Zero.
			ui32Bits = ui32Sign;
		}
		else {
			// Subnormal:  normalize it.
			ui32Exp = 127 - 15 + 1;
			while ( !(ui32Mant & 0x400) ) {
				ui32Mant <<= 1;
				--ui32Exp;
			}
			ui32Bits = ui32Sign | (ui32Exp << 23) | ((ui32Mant & 0x3FF) << 13);
		}
		float fRet;
		std::memcpy( &fRet, &ui32Bits, sizeof( fRet ) );
		return fRet;
	}

}	// namespace lsn
