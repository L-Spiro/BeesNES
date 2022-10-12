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
	lsn::CZipFile fbFile;
	fbFile.Open( u8"J:\\水\\cpu_dummy_writes.zip" );
	/*std::vector<uint8_t> vData;
	fbFile.LoadToMemory( vData );*/
	std::vector<std::u16string> vFiles;
	fbFile.GatherArchiveFiles( vFiles );
	std::vector<uint8_t> vExtracted;
	for ( auto I = vFiles.size(); I--; ) {
		std::u16string s16Ext = lsn::CUtilities::GetFileExtension( vFiles[I] );
		std::u16string s16File = lsn::CUtilities::GetFileName( vFiles[I] );
		fbFile.ExtractToMemory( vFiles[I], vExtracted );
	}
	
	lsn::CNtscSystem nsSystem;
	return 0;
}