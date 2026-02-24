/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A class for reading and applying IPS patches to a stream.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Utilities/LSNStreamBase.h"
#include "../Utilities/LSNUtilities.h"

#include <vector>


namespace lsn {

	/**
	 * Class CIpsFile
	 * \brief class for applying IPS patches.
	 *
	 * Description: Parses an IPS patch stream and applies its records to a target stream.
	 */
	class CIpsFile {
	public :
		CIpsFile( const CStreamBase * _psbStream ) :
			m_psbStream( _psbStream ) {
		}


		// == Types.
		/**
		 * Structure LSN_IPS_RECORD
		 * \brief A single record from an IPS patch.
		 *
		 * Description: Contains the offset, size, and payload (or RLE data) for a single IPS patch operation.
		 */
		struct LSN_IPS_RECORD {
			uint32_t											ui32Offset;							/**< The 24-bit offset where the data should be written. */
			uint16_t											ui16Size;							/**< The size of the payload. 0 indicates an RLE record. */
			uint16_t											ui16RleSize;						/**< The RLE size if ui16Size is 0. */
			uint8_t												ui8RleValue;						/**< The RLE value if ui16Size is 0. */
			std::vector<uint8_t>								vData;								/**< The payload data if ui16Size > 0. */
		};
		
		
		// == Functions.
		/**
		 * Verifies the IPS header ("PATCH").
		 *
		 * \return Returns true if the stream begins with "PATCH".
		 * \throws std::runtime_error if the stream is too short to contain the header or a read error occurs.
		 */
		bool													VerifyHeader() {
			if ( !m_psbStream ) { throw std::runtime_error( "CIpsFile::VerifyHeader: No stream provided." ); }

			uint8_t ui8Header[5];
			for ( size_t I = 0; I < 5; ++I ) {
				if ( !m_psbStream->ReadUi8( ui8Header[I] ) ) {
					throw std::runtime_error( "CIpsFile::VerifyHeader: Unexpected end of file while reading IPS header." );
				}
			}
			
			if ( ui8Header[0] != 'P' || ui8Header[1] != 'A' || ui8Header[2] != 'T' || ui8Header[3] != 'C' || ui8Header[4] != 'H' ) {
				return false;
			}

			return true;
		}

		/**
		 * Reads the next record from the IPS stream.
		 *
		 * \param _irRecord Holds the returned record data if successful.
		 * \return Returns true if a record was successfully read, or false if the end of the patch (EOF) was reached.
		 * \throws std::runtime_error if an unexpected end of file or read error occurs during a record.
		 */
		bool													GetNextRecord( LSN_IPS_RECORD &_irRecord ) {
			if ( !m_psbStream ) { throw std::runtime_error( "CIpsFile::GetNextRecord: No stream provided." ); }

			uint8_t ui8Offset[3];
			
			// The first byte read is the only place a natural end-of-stream is acceptable.
			if ( !m_psbStream->ReadUi8( ui8Offset[0] ) ) { return false; }
			
			// If we read the first byte, we must be able to finish the offset or it's a file error.
			if ( !m_psbStream->ReadUi8( ui8Offset[1] ) ) { throw std::runtime_error( "CIpsFile::GetNextRecord: Unexpected end of file while reading IPS record offset." ); }
			if ( !m_psbStream->ReadUi8( ui8Offset[2] ) ) { throw std::runtime_error( "CIpsFile::GetNextRecord: Unexpected end of file while reading IPS record offset." ); }

			// Check for EOF (0x45, 0x4F, 0x46).
			if ( ui8Offset[0] == 0x45 && ui8Offset[1] == 0x4F && ui8Offset[2] == 0x46 ) {
				// We've reached the valid end of the patch records.
				return false;
			}

			// Clear previous data.
			_irRecord.vData.clear();

			// Offset is 24-bit big-endian.
			_irRecord.ui32Offset = (static_cast<uint32_t>(ui8Offset[0]) << 16) | (static_cast<uint32_t>(ui8Offset[1]) << 8) | ui8Offset[2];

			uint8_t ui8Size[2];
			if ( !m_psbStream->ReadUi8( ui8Size[0] ) ) { throw std::runtime_error( "CIpsFile::GetNextRecord: Unexpected end of file while reading IPS record size." ); }
			if ( !m_psbStream->ReadUi8( ui8Size[1] ) ) { throw std::runtime_error( "CIpsFile::GetNextRecord: Unexpected end of file while reading IPS record size." ); }
			
			// Size is 16-bit big-endian.
			_irRecord.ui16Size = (static_cast<uint16_t>(ui8Size[0]) << 8) | ui8Size[1];
			_irRecord.ui16RleSize = 0;
			_irRecord.ui8RleValue = 0;

			if ( _irRecord.ui16Size == 0 ) {
				// RLE Record.
				uint8_t ui8RleSize[2];
				if ( !m_psbStream->ReadUi8( ui8RleSize[0] ) ) { throw std::runtime_error( "CIpsFile::GetNextRecord: Unexpected end of file while reading IPS RLE size." ); }
				if ( !m_psbStream->ReadUi8( ui8RleSize[1] ) ) { throw std::runtime_error( "CIpsFile::GetNextRecord: Unexpected end of file while reading IPS RLE size." ); }
				
				_irRecord.ui16RleSize = (static_cast<uint16_t>(ui8RleSize[0]) << 8) | ui8RleSize[1];

				if ( !m_psbStream->ReadUi8( _irRecord.ui8RleValue ) ) { throw std::runtime_error( "CIpsFile::GetNextRecord: Unexpected end of file while reading IPS RLE value." ); }
			}
			else {
				// Normal Record.
				_irRecord.vData.resize( _irRecord.ui16Size );
				for ( uint16_t I = 0; I < _irRecord.ui16Size; ++I ) {
					if ( !m_psbStream->ReadUi8( _irRecord.vData[I] ) ) { throw std::runtime_error( "CIpsFile::GetNextRecord: Unexpected end of file while reading IPS record payload." ); }
				}
			}

			return true;
		}

		/**
		 * Applies the IPS patch to the given target stream.
		 *
		 * \param _psbTargetStream A pointer to the target stream (ROM) to which the patch will be applied.
		 * \return Returns true if the patch is valid and successfully applied.
		 */
		bool													ApplyPatch( CStreamBase * _psbTargetStream ) {
			if ( !_psbTargetStream || !m_psbStream ) { return false; }

			//if ( !VerifyHeader() ) { return false; }

			LSN_IPS_RECORD irRecord;
			
			// GetNextRecord will throw std::runtime_error on malformed data, which will propagate up.
			// It returns false naturally when the "EOF" marker is hit.
			while ( GetNextRecord( irRecord ) ) {
				_psbTargetStream->MovePointerTo( irRecord.ui32Offset );

				if ( irRecord.ui16Size == 0 ) {
					// RLE Record.
					std::vector<uint8_t> vRle( irRecord.ui16RleSize, irRecord.ui8RleValue );
					if ( _psbTargetStream->Write( vRle.data(), irRecord.ui16RleSize ) != irRecord.ui16RleSize ) { return false; }
				}
				else {
					// Normal Record.
					if ( _psbTargetStream->Write( irRecord.vData.data(), irRecord.ui16Size ) != irRecord.ui16Size ) { return false; }
				}
			}

			// Optional truncation extension: exactly 3 bytes after EOF dictate the final target file size.
			uint8_t ui8Trunc[3];
			if ( m_psbStream->ReadUi8( ui8Trunc[0] ) && m_psbStream->ReadUi8( ui8Trunc[1] ) && m_psbStream->ReadUi8( ui8Trunc[2] ) ) {
				uint32_t ui32TruncSize = (static_cast<uint32_t>(ui8Trunc[0]) << 16) | (static_cast<uint32_t>(ui8Trunc[1]) << 8) | ui8Trunc[2];
				// Moving the pointer past the current end of the stream should trigger an expansion/truncation in the stream handler.
				_psbTargetStream->MovePointerTo( ui32TruncSize );
			}

			return true;
		}

	protected :
		// == Members.
		const CStreamBase *										m_psbStream;						/**< The input stream. */
	};

}
