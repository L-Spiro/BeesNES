/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: System base class.  Provides the interface for all regional systems.
 */

#include "LSNSystemBase.h"
#include "../Crc/LSNCrc.h"
#include "../Database/LSNDatabase.h"
#include "../Roms/LSNNesHeader.h"
#include "../Utilities/LSNUtilities.h"


namespace lsn {

	// == Functions.
	/**
	 * Loads a ROM into the given LSN_ROM object.
	 *
	 * \param _vRom The in-memory ROM fille.
	 * \param _rRom The LSN_ROM target object.
	 * \param _s16Path The ROM file path.
	 * \return Returns true if the ROM was loaded.
	 */
	bool CSystemBase::LoadRom( const std::vector<uint8_t> &_vRom, LSN_ROM &_rRom, const std::u16string &_s16Path ) {
		_rRom = LSN_ROM();
		_rRom.riInfo.s16File = _s16Path;
		_rRom.riInfo.s16RomName = CUtilities::GetFileName( _s16Path );
			

		if ( _vRom.size() >= 4 ) {
			const uint8_t ui8NesHeader[] = {
				0x4E, 0x45, 0x53, 0x1A
			};
			if ( std::memcmp( _vRom.data(), ui8NesHeader, sizeof( ui8NesHeader ) ) == 0 ) {
				// .NES.
				if ( !LoadNes( _vRom, _rRom ) ) { return false; }
				//return true;
			}
			else {
				_rRom = LSN_ROM();
				return false;
			}

			// ROM loaded.
			_rRom.riInfo.ui32Crc = CCrc::GetCrc( _rRom.vPrgRom.data(), _rRom.vPrgRom.size() );

			// Apply overrides from the database.
			{
				auto aEntry = CDatabase::m_mDatabase.find( _rRom.riInfo.ui32Crc );
				if ( aEntry != CDatabase::m_mDatabase.end() ) {
					if ( aEntry->second.mmMirrorOverride != LSN_MM_NO_OVERRIDE ) {
						_rRom.riInfo.mmMirroring = aEntry->second.mmMirrorOverride;
					}
					if ( aEntry->second.pmRegion != LSN_PM_UNKNOWN ) {
						_rRom.riInfo.pmConsoleRegion = aEntry->second.pmRegion;
					}
					if ( aEntry->second.ui16Mapper != 0xFFFF ) {
						_rRom.riInfo.ui16Mapper = aEntry->second.ui16Mapper;
					}
				}
			}

			return true;
		}

		_rRom = LSN_ROM();
		return false;
	}

	/**
	 * Loads a ROM image in .NES format.
	 *
	 * \param _vRom The ROM image to load.
	 * \return Returns true if the image was loaded, false otherwise.
	 */
	bool CSystemBase::LoadNes( const std::vector<uint8_t> &_vRom, LSN_ROM &_rRom ) {
		if ( _vRom.size() >= sizeof( LSN_NES_HEADER ) ) {
			size_t stDataSize = _vRom.size() - sizeof( LSN_NES_HEADER );
			const uint8_t * pui8Data = _vRom.data() + sizeof( LSN_NES_HEADER );
			const LSN_NES_HEADER * pnhHeader = reinterpret_cast<const LSN_NES_HEADER *>(_vRom.data());
			_rRom.riInfo.ui16Mapper = pnhHeader->GetMapper();
			_rRom.riInfo.ui16SubMapper = pnhHeader->GetSubMapper();
			_rRom.riInfo.mmMirroring = pnhHeader->GetMirrorMode();
			_rRom.riInfo.pmConsoleRegion = pnhHeader->GetGameRegion();
			_rRom.i32ChrRamSize = pnhHeader->GetChrRomSize();
			_rRom.i32SaveChrRamSize = pnhHeader->GetSaveChrRamSize();
			_rRom.i32WorkRamSize = pnhHeader->GetWorkRamSize();
			_rRom.i32SaveRamSize = pnhHeader->GetSaveRamSize();
				

			if ( pnhHeader->HasTrainer() ) {
				if ( stDataSize < 512 ) { return false; }
				// Load trainer.
				stDataSize -= 512;
				pui8Data += 512;
			}

			uint32_t ui32PrgSize = pnhHeader->GetPgmRomSize();
			uint32_t ui32ChrSize = pnhHeader->GetChrRomSize();
			if ( size_t( ui32PrgSize ) + size_t( ui32ChrSize ) > stDataSize ) { return false; }
			_rRom.vPrgRom.insert( _rRom.vPrgRom.end(), pui8Data, pui8Data + ui32PrgSize );
			pui8Data += ui32PrgSize;
			_rRom.vChrRom.insert( _rRom.vChrRom.end(), pui8Data, pui8Data + ui32ChrSize );

			return true;
		}
		return false;
	}

}	// namespace lsn
