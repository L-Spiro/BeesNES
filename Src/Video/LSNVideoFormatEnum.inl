/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The video formats that can be exported via FFmpeg.  Define LSN_VF_ENUM( ENUM, TXT ) before including this file.
 *	ENUM is the LSN_VIDEO_FORMAT value and TXT is its display string.  The order here is the order in which the formats are listed in the UI
 *	and must match the order of the format database in LSNVideoFormat.cpp.
 */

LSN_VF_ENUM( LSN_VF_H264,						LSN_LSTR( LSN_STR_H_264_X264 ) )								/**< H.264/AVC via libx264 in MP4. */
LSN_VF_ENUM( LSN_VF_H264_NVENC,					LSN_LSTR( LSN_STR_H_264_NVENC ) )								/**< H.264/AVC via NVIDIA NVENC in MP4. */
LSN_VF_ENUM( LSN_VF_H264_AMF,					LSN_LSTR( LSN_STR_H_264_AMF ) )									/**< H.264/AVC via AMD AMF in MP4. */
LSN_VF_ENUM( LSN_VF_H264_QSV,					LSN_LSTR( LSN_STR_H_264_QSV ) )									/**< H.264/AVC via Intel Quick Sync in MP4. */
LSN_VF_ENUM( LSN_VF_HEVC,						LSN_LSTR( LSN_STR_HEVC_X265 ) )									/**< H.265/HEVC via libx265 in MP4. */
LSN_VF_ENUM( LSN_VF_HEVC_NVENC,					LSN_LSTR( LSN_STR_HEVC_NVENC ) )								/**< H.265/HEVC via NVIDIA NVENC in MP4. */
LSN_VF_ENUM( LSN_VF_HEVC_AMF,					LSN_LSTR( LSN_STR_HEVC_AMF ) )									/**< H.265/HEVC via AMD AMF in MP4. */
LSN_VF_ENUM( LSN_VF_HEVC_QSV,					LSN_LSTR( LSN_STR_HEVC_QSV ) )									/**< H.265/HEVC via Intel Quick Sync in MP4. */
LSN_VF_ENUM( LSN_VF_AV1,						LSN_LSTR( LSN_STR_AV1_SVT ) )									/**< AV1 via SVT-AV1 in MP4. */
LSN_VF_ENUM( LSN_VF_AV1_NVENC,					LSN_LSTR( LSN_STR_AV1_NVENC ) )									/**< AV1 via NVIDIA NVENC in MP4. */
LSN_VF_ENUM( LSN_VF_VP9,						LSN_LSTR( LSN_STR_VP9_LIBVPX ) )								/**< VP9 via libvpx in WebM. */
LSN_VF_ENUM( LSN_VF_PRORES,						LSN_LSTR( LSN_STR_APPLE_PRORES ) )								/**< Apple ProRes via prores_ks in QuickTime MOV. */
LSN_VF_ENUM( LSN_VF_FFV1,						LSN_LSTR( LSN_STR_FFV1_LOSSLESS ) )								/**< FFV1 (lossless) in Matroska. */
LSN_VF_ENUM( LSN_VF_UTVIDEO,					LSN_LSTR( LSN_STR_UT_VIDEO_LOSSLESS ) )							/**< Ut Video (lossless) in AVI. */
LSN_VF_ENUM( LSN_VF_MPEG4,						LSN_LSTR( LSN_STR_MPEG_4_PART_2 ) )								/**< MPEG-4 Part 2 via FFmpeg's native encoder in MP4. */
