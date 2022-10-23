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

int WINAPI wWinMain( _In_ HINSTANCE /*_hInstance*/, _In_opt_ HINSTANCE /*_hPrevInstance*/, _In_ LPWSTR /*_lpCmdLine*/, _In_ int /*_nCmdShow*/ ) {
#define LSN_PATH				u"J:\\My Projects\\L. Spiro NES\\Tests\\nestest.nes"
	std::unique_ptr<lsn::CNtscSystem> pnsSystem = std::make_unique<lsn::CNtscSystem>();
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
	pnsSystem->LoadRom( vExtracted, s16Path );
	pnsSystem->ResetState( false );
	uint64_t ui64TickCount = 0;
#define LSN_TIME								(1ULL * 60)
	while ( pnsSystem->GetAccumulatedRealTime() / pnsSystem->GetClockResolution() < LSN_TIME ) {
		pnsSystem->Tick();
		++ui64TickCount;
	}
	uint64_t ui64Time = cClock.GetRealTick() - cClock.GetStartTick();
	// If there are more Tick()'s than cycles then we definitely didn't go slower than the real system, so we can shave off excess time.

	double dTime = (ui64TickCount >= pnsSystem->GetMasterCounter()) ?
		double( LSN_TIME ) :
		ui64Time / double( cClock.GetResolution() );
	char szBuffer[256];
	::sprintf_s( szBuffer, "Ticks: %llu. Time: %.8f.\r\n"
		"Master Cycles: %llu (%.8f per second; expected %.8f).\r\n"
		"%.8f cycles per Tick().\r\n"
		"%.8f FPS.\r\n",
		ui64TickCount, dTime,
		pnsSystem->GetMasterCounter(), pnsSystem->GetMasterCounter() / dTime, double( pnsSystem->MasterHz() ) / pnsSystem->MasterDiv(),
		pnsSystem->GetMasterCounter() / double( ui64TickCount ),
		pnsSystem->GetPpu().GetFrameCount() / dTime
		);
	::OutputDebugStringA( szBuffer );
	return 0;
}
