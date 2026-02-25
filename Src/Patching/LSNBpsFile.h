/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A class for reading and applying BPS patches to a stream.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Utilities/LSNStreamBase.h"
#include "../Utilities/LSNUtilities.h"

#include <stdexcept>
#include <vector>


namespace lsn {

	/**
	 * Class CBpsFile
	 * \brief A class for applying BPS patches.
	 *
	 * Description: Parses a BPS patch stream and applies its commands to a target stream using a source stream.
	 */
	class CBpsFile {
	public :
		// == Various constructors.
		/**
		 * Constructor.
		 *
		 * \param _psbStream A pointer to the stream containing the BPS patch data.
		 */
		CBpsFile( const CStreamBase * _psbStream ) :
			m_psbStream( _psbStream ) {
		}


		// == Functions.
		/**
		 * Applies the BPS patch.
		 *
		 * \param _psbSourceStream A pointer to the original, unmodified source stream (ROM).
		 * \param _psbTargetStream A pointer to the target stream where the patched data will be written.
		 * \return Returns true if the patch is valid and successfully applied.
		 * \throws std::runtime_error if stream operations encounter unexpected EOFs.
		 */
		bool													ApplyPatch( const CStreamBase * _psbSourceStream, CStreamBase * _psbTargetStream ) {
			if ( !_psbSourceStream || !_psbTargetStream || !m_psbStream ) { return false; }

			//if ( !VerifyHeader() ) { return false; }

			uint64_t ui64SourceSize, ui64TargetSize, ui64MetadataSize;
			if ( !DecodeVlv( ui64SourceSize ) ) { return false; }
			if ( !DecodeVlv( ui64TargetSize ) ) { return false; }
			if ( !DecodeVlv( ui64MetadataSize ) ) { return false; }

			// Skip metadata.
			m_psbStream->MovePointerBy( static_cast<int64_t>(ui64MetadataSize) );

			uint64_t ui64SourceOffset = 0;
			uint64_t ui64TargetOffset = 0;
			int64_t i64SourceRelativeOffset = 0;
			int64_t i64TargetRelativeOffset = 0;

			while ( ui64TargetOffset < ui64TargetSize ) {
				uint64_t ui64Command;
				if ( !DecodeVlv( ui64Command ) ) { return false; }

				uint64_t ui64CommandType = ui64Command & 3;
				uint64_t ui64Length = (ui64Command >> 2) + 1;

				switch ( ui64CommandType ) {
					case 0 : {	// SourceRead
						std::vector<uint8_t> vBuffer( ui64Length );
						_psbSourceStream->MovePointerTo( ui64SourceOffset );
						for ( uint64_t I = 0; I < ui64Length; ++I ) {
							if ( !_psbSourceStream->ReadUi8( vBuffer[I] ) ) { throw std::runtime_error( "CBpsFile::ApplyPatch: Unexpected EOF in source stream." ); }
						}
						_psbTargetStream->MovePointerTo( ui64TargetOffset );
						if ( _psbTargetStream->Write( vBuffer.data(), ui64Length ) != ui64Length ) { return false; }
						
						ui64SourceOffset += ui64Length;
						ui64TargetOffset += ui64Length;
						break;
					}
					case 1 : {	// TargetRead
						std::vector<uint8_t> vBuffer( ui64Length );
						for ( uint64_t I = 0; I < ui64Length; ++I ) {
							if ( !m_psbStream->ReadUi8( vBuffer[I] ) ) { throw std::runtime_error( "CBpsFile::ApplyPatch: Unexpected EOF in patch stream." ); }
						}
						_psbTargetStream->MovePointerTo( ui64TargetOffset );
						if ( _psbTargetStream->Write( vBuffer.data(), ui64Length ) != ui64Length ) { return false; }
						
						ui64TargetOffset += ui64Length;
						break;
					}
					case 2 : {	// SourceCopy
						int64_t i64Offset;
						if ( !DecodeOffset( i64Offset ) ) { return false; }
						i64SourceRelativeOffset += i64Offset;

						std::vector<uint8_t> vBuffer( ui64Length );
						_psbSourceStream->MovePointerTo( static_cast<uint64_t>(i64SourceRelativeOffset) );
						for ( uint64_t I = 0; I < ui64Length; ++I ) {
							if ( !_psbSourceStream->ReadUi8( vBuffer[I] ) ) { throw std::runtime_error( "CBpsFile::ApplyPatch: Unexpected EOF in source stream (copy)." ); }
						}
						_psbTargetStream->MovePointerTo( ui64TargetOffset );
						if ( _psbTargetStream->Write( vBuffer.data(), ui64Length ) != ui64Length ) { return false; }
						
						i64SourceRelativeOffset += ui64Length;
						ui64TargetOffset += ui64Length;
						break;
					}
					case 3 : {	// TargetCopy
						int64_t i64Offset;
						if ( !DecodeOffset( i64Offset ) ) { return false; }
						i64TargetRelativeOffset += i64Offset;

						// TargetCopy can overlap with the current write pointer to create repeating patterns (RLE).
						// Byte-by-byte processing is strictly required to read the bytes that were just written.
						for ( uint64_t I = 0; I < ui64Length; ++I ) {
							uint8_t ui8Byte;
							_psbTargetStream->MovePointerTo( static_cast<uint64_t>(i64TargetRelativeOffset++) );
							if ( !_psbTargetStream->ReadUi8( ui8Byte ) ) { throw std::runtime_error( "CBpsFile::ApplyPatch: Unexpected EOF in target stream (copy)." ); }
							
							_psbTargetStream->MovePointerTo( ui64TargetOffset++ );
							if ( _psbTargetStream->Write( &ui8Byte, 1 ) != 1 ) { return false; }
						}
						break;
					}
				}
			}

			// Note: The final 12 bytes of the BPS stream contain CRC32 hashes for Source, Target, and Patch.
			// The stream pointer is perfectly positioned to read them if CRC validation is integrated later.
			return true;
		}

		/**
		 * Verifies the BPS header ("BPS1").
		 *
		 * \return Returns true if the stream begins with "BPS1".
		 * \throws std::runtime_error if the stream is too short to contain the header.
		 */
		bool													VerifyHeader() {
			if ( !m_psbStream ) { throw std::runtime_error( "CBpsFile::VerifyHeader: No stream provided." ); }

			uint8_t ui8Header[4];
			for ( size_t I = 0; I < 4; ++I ) {
				if ( !m_psbStream->ReadUi8( ui8Header[I] ) ) {
					throw std::runtime_error( "CBpsFile::VerifyHeader: Unexpected end of file while reading BPS header." );
				}
			}
			
			if ( ui8Header[0] != 'B' || ui8Header[1] != 'P' || ui8Header[2] != 'S' || ui8Header[3] != '1' ) {
				return false;
			}

			return true;
		}


	protected :
		// == Members.
		const CStreamBase *										m_psbStream;						/**< The input stream. */


		// == Functions.
		/**
		 * Decodes a BPS Variable Length Value (VLV).
		 *
		 * \param _ui64Value Holds the returned decoded value.
		 * \return Returns true if the VLV was successfully read.
		 */
		bool													DecodeVlv( uint64_t &_ui64Value ) {
			_ui64Value = 0;
			uint64_t ui64Shift = 1;
			while ( true ) {
				uint8_t ui8Byte;
				if ( !m_psbStream->ReadUi8( ui8Byte ) ) { return false; }
				
				_ui64Value += (ui8Byte & 0x7F) * ui64Shift;
				if ( ui8Byte & 0x80 ) { break; }
				
				ui64Shift <<= 7;
				_ui64Value += ui64Shift;
			}
			return true;
		}

		/**
		 * Decodes a BPS offset value.
		 *
		 * \param _i64Value Holds the returned decoded offset.
		 * \return Returns true if the offset was successfully read.
		 */
		bool													DecodeOffset( int64_t &_i64Value ) {
			uint64_t ui64Data;
			if ( !DecodeVlv( ui64Data ) ) { return false; }
			
			_i64Value = static_cast<int64_t>(ui64Data >> 1);
			if ( ui64Data & 1 ) {
				_i64Value = -_i64Value;
			}
			return true;
		}
	};

}	// namespace lsn