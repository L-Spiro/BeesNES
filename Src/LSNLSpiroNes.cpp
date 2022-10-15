#include "LSNLSpiroNes.h"
#include "File/LSNZipFile.h"
#include "OS/LSNOs.h"
#include "System/LSNSystem.h"
#include "Utilities/LSNUtilities.h"

int wmain( int /*_iArgC*/, wchar_t * /*_pwcArgv*/[] ) {
	return 0;
}

int main() {
	return 0;
}

int wWinMain( HINSTANCE /*_hInstance*/, HINSTANCE /*_hPrevInstance*/, LPWSTR /*_lpCmdLine*/, int /*_nCmdShow*/ ) {
#define LSN_PATH				u"J:\\My Projects\\L. Spiro NES\\Tests\\nestest.nes"
	lsn::CNtscSystem nsSystem;
	std::vector<uint8_t> vExtracted;
	std::u16string s16Path;
	{
		lsn::CZipFile fbFile;
		fbFile.Open( LSN_PATH );
		/*std::vector<uint8_t> vData;
		fbFile.LoadToMemory( vData );*/
		std::vector<std::u16string> vFiles;
		fbFile.GatherArchiveFiles( vFiles );
		if ( vFiles.size() ) {
			for ( auto I = vFiles.size(); I--; ) {
				std::u16string s16Ext = lsn::CUtilities::GetFileExtension( vFiles[I] );
				if ( ::_wcsicmp( reinterpret_cast<const wchar_t *>(s16Ext.c_str()), L"NES" ) == 0 ) {
					s16Path = lsn::CUtilities::GetFileName( vFiles[I] );
					fbFile.ExtractToMemory( vFiles[I], vExtracted );
					break;
				}
			}
		}
		else {
			s16Path = LSN_PATH;
			fbFile.LoadToMemory( vExtracted );
		}
	}
#undef LSN_PATH
	lsn::CClock cClock;
	nsSystem.LoadRom( vExtracted, s16Path );
	nsSystem.ResetState( false );
	uint64_t ui64TickCount = 0;
	while ( nsSystem.GetAccumulatedRealTime() / nsSystem.GetClockResolution() < 1 * (60 * 60 * 2) ) {
		nsSystem.Tick();
		++ui64TickCount;
	}
	uint64_t ui64Time = cClock.GetRealTick() - cClock.GetStartTick();
	double dTime = ui64Time / double( cClock.GetResolution() );
	char szBuffer[256];
	::sprintf_s( szBuffer, "Ticks: %llu. Time: %.8f.\r\n"
		"Master Cycles: %llu (%.8f per second; expected %.8f).\r\n"
		"%.8f cycles per Tick().\r\n",
		ui64TickCount, dTime,
		nsSystem.GetMasterCounter(), nsSystem.GetMasterCounter() / dTime, double( nsSystem.MasterHz() ) / nsSystem.MasterDiv(),
		nsSystem.GetMasterCounter() / double( ui64TickCount )
		);
	::OutputDebugStringA( szBuffer );
	return 0;
}
