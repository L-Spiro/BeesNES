/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A captured frame (software output, a GPU texture, or the swap chain) in whatever format it was captured.
 */


#pragma once

#include "../LSNLSpiroNes.h"

#include <cstring>
#include <vector>


namespace lsn {

	/**
	 * Enumeration LSN_CAPTURE_FORMAT
	 * \brief The pixel format of a captured frame.
	 *
	 * Description: The pixel format of a captured frame.  Channel names are listed in memory (byte) order unless noted otherwise.
	 *	The packed formats (LSN_CF_B5G5R5X1, LSN_CF_B5G6R5, LSN_CF_R10G10B10A2) are little-endian words with the first-named channel in the
	 *	least-significant bits.  The _SRGB formats are the same bytes as their counterparts but are tagged as sRGB-encoded by the API they
	 *	came from.
	 */
	enum LSN_CAPTURE_FORMAT : uint32_t {
		LSN_CF_UNKNOWN,											/**< Unknown/no format.  The buffer cannot be used. */
		LSN_CF_B5G5R5X1,										/**< 16-bit:  B in bits 0-4, G in bits 5-9, R in bits 10-14 (a 16-bit Windows DIB). */
		LSN_CF_B5G6R5,											/**< 16-bit:  B in bits 0-4, G in bits 5-10, R in bits 11-15. */
		LSN_CF_RGB8,											/**< 24-bit:  R, G, B. */
		LSN_CF_BGR8,											/**< 24-bit:  B, G, R (a 24-bit Windows DIB). */
		LSN_CF_RGBA8,											/**< 32-bit:  R, G, B, A. */
		LSN_CF_BGRA8,											/**< 32-bit:  B, G, R, A. */
		LSN_CF_RGBX8,											/**< 32-bit:  R, G, B, unused. */
		LSN_CF_BGRX8,											/**< 32-bit:  B, G, R, unused (a 32-bit Windows DIB, D3DFMT_X8R8G8B8). */
		LSN_CF_RGBA8_SRGB,										/**< 32-bit:  R, G, B, A, sRGB-encoded. */
		LSN_CF_BGRA8_SRGB,										/**< 32-bit:  B, G, R, A, sRGB-encoded. */
		LSN_CF_R10G10B10A2,										/**< 32-bit:  R in bits 0-9, G in bits 10-19, B in bits 20-29, A in bits 30-31. */
		LSN_CF_RGBA16F,											/**< 64-bit:  R, G, B, A as 16-bit floats. */
		LSN_CF_RGBA32F,											/**< 128-bit:  R, G, B, A as 32-bit floats. */
	};

	/**
	 * Struct LSN_CAPTURE_BUFFER
	 * \brief A captured frame.
	 *
	 * Description: A captured frame.  Filled once per rendered frame while recording.  The rows are always stored top row first, and the
	 *	pixels are in the format in which they were captured (no conversion is done).
	 */
	struct LSN_CAPTURE_BUFFER {
		std::vector<uint8_t>									vBuffer;										/**< The pixels, top row first.  At least ui32Pitch * ui32Height bytes. */
		uint32_t												ui32Width = 0;									/**< The width of the image in pixels. */
		uint32_t												ui32Height = 0;									/**< The height of the image in pixels. */
		uint32_t												ui32Pitch = 0;									/**< The number of bytes from the start of one row to the start of the next. */
		LSN_CAPTURE_FORMAT										cfFormat = LSN_CF_UNKNOWN;						/**< The format of the pixels. */
		uint64_t												ui64PpuFrame = 0;								/**< The PPU frame that was captured. */
		uint64_t												ui64Captures = 0;								/**< Incremented each time a new frame is captured into this buffer. */


		// == Functions.
		/**
		 * Sizes the buffer for an image and sets its dimensions and format.  The pixels are left as they are.
		 *
		 * \param _ui32Width The width of the image in pixels.
		 * \param _ui32Height The height of the image in pixels.
		 * \param _ui32Pitch The number of bytes from the start of one row to the start of the next.  Must be at least _ui32Width * BytesPerPixel( _cfFormat ).
		 * \param _cfFormat The format of the pixels.
		 * \return Returns true if the buffer could be allocated.
		 */
		bool													Allocate( uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Pitch, LSN_CAPTURE_FORMAT _cfFormat ) {
			if ( !_ui32Width || !_ui32Height || _cfFormat == LSN_CF_UNKNOWN || _ui32Pitch < _ui32Width * BytesPerPixel( _cfFormat ) ) { return false; }
			try {
				vBuffer.resize( size_t( _ui32Pitch ) * _ui32Height );
			}
			catch ( ... ) {
				Reset();
				return false;
			}
			ui32Width = _ui32Width;
			ui32Height = _ui32Height;
			ui32Pitch = _ui32Pitch;
			cfFormat = _cfFormat;
			return true;
		}

		/**
		 * Copies an image into the buffer, sizing the buffer to fit.  The copy keeps the tightly packed row size (_ui32Width * BytesPerPixel( _cfFormat )) as its pitch.
		 *
		 * \param _pui8Src The source pixels.
		 * \param _ui32Width The width of the image in pixels.
		 * \param _ui32Height The height of the image in pixels.
		 * \param _ui32SrcPitch The number of bytes from the start of one source row to the start of the next.
		 * \param _cfFormat The format of the pixels.
		 * \param _bBottomUp If true, the first row in _pui8Src is the bottom row of the image.
		 * \param _ui64PpuFrame The PPU frame being captured.
		 * \return Returns true if the buffer could be allocated.
		 */
		bool													Copy( const uint8_t * _pui8Src, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32SrcPitch, LSN_CAPTURE_FORMAT _cfFormat, bool _bBottomUp, uint64_t _ui64PpuFrame ) {
			if ( !_pui8Src ) { return false; }
			uint32_t ui32RowSize = _ui32Width * BytesPerPixel( _cfFormat );
			if ( _ui32SrcPitch < ui32RowSize ) { return false; }
			if ( !Allocate( _ui32Width, _ui32Height, ui32RowSize, _cfFormat ) ) { return false; }
			if ( !_bBottomUp && _ui32SrcPitch == ui32RowSize ) {
				std::memcpy( vBuffer.data(), _pui8Src, size_t( ui32RowSize ) * _ui32Height );
			}
			else {
				for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
					uint32_t ui32SrcRow = _bBottomUp ? (_ui32Height - 1 - Y) : Y;
					std::memcpy( &vBuffer[size_t( Y )*ui32RowSize], _pui8Src + size_t( ui32SrcRow ) * _ui32SrcPitch, ui32RowSize );
				}
			}
			ui64PpuFrame = _ui64PpuFrame;
			++ui64Captures;
			return true;
		}

		/**
		 * Determines whether the buffer holds an image.
		 *
		 * \return Returns true if the buffer holds an image.
		 */
		inline bool												Valid() const { return cfFormat != LSN_CF_UNKNOWN && ui32Width && ui32Height && vBuffer.size() >= size_t( ui32Pitch ) * ui32Height; }

		/**
		 * Resets the buffer to an empty state and frees its memory.
		 */
		void													Reset() {
			vBuffer = std::vector<uint8_t>();
			ui32Width = ui32Height = ui32Pitch = 0;
			cfFormat = LSN_CF_UNKNOWN;
			ui64PpuFrame = 0;
			ui64Captures = 0;
		}

		/**
		 * Gets the size of a pixel in bytes.
		 *
		 * \param _cfFormat The format whose pixel size is to be returned.
		 * \return Returns the size of a pixel in bytes, or 0 for LSN_CF_UNKNOWN.
		 */
		static inline uint32_t									BytesPerPixel( LSN_CAPTURE_FORMAT _cfFormat ) {
			switch ( _cfFormat ) {
				case LSN_CF_B5G5R5X1 : {}						LSN_FALLTHROUGH
				case LSN_CF_B5G6R5 : { return 2; }
				case LSN_CF_RGB8 : {}							LSN_FALLTHROUGH
				case LSN_CF_BGR8 : { return 3; }
				case LSN_CF_RGBA8 : {}							LSN_FALLTHROUGH
				case LSN_CF_BGRA8 : {}							LSN_FALLTHROUGH
				case LSN_CF_RGBX8 : {}							LSN_FALLTHROUGH
				case LSN_CF_BGRX8 : {}							LSN_FALLTHROUGH
				case LSN_CF_RGBA8_SRGB : {}						LSN_FALLTHROUGH
				case LSN_CF_BGRA8_SRGB : {}						LSN_FALLTHROUGH
				case LSN_CF_R10G10B10A2 : { return 4; }
				case LSN_CF_RGBA16F : { return 8; }
				case LSN_CF_RGBA32F : { return 16; }
				default : { return 0; }
			}
		}

		/**
		 * Gets the format of a Windows DIB (the software-rendering output) with the given bit depth.
		 *
		 * \param _ui32Bits The bits per pixel of the DIB (16, 24, or 32).
		 * \return Returns the format of the DIB or LSN_CF_UNKNOWN.
		 */
		static inline LSN_CAPTURE_FORMAT						DibFormat( uint32_t _ui32Bits ) {
			switch ( _ui32Bits ) {
				case 16 : { return LSN_CF_B5G5R5X1; }
				case 24 : { return LSN_CF_BGR8; }
				case 32 : { return LSN_CF_BGRX8; }
				default : { return LSN_CF_UNKNOWN; }
			}
		}
	};

}	// namespace lsn
