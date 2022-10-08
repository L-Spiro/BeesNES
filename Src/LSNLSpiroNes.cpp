#include "LSNLSpiroNes.h"
#include "File/LSNZipFile.h"
#include "OS/LSNOs.h"

int wmain( int /*_iArgC*/, wchar_t * /*_pwcArgv*/[] ) {
	return 0;
}

int main() {
	return 0;
}

int wWinMain( HINSTANCE /*_hInstance*/, HINSTANCE /*_hPrevInstance*/, LPWSTR /*_lpCmdLine*/, int /*_nCmdShow*/ ) {
	lsn::CZipFile fbFile;
	fbFile.Open( u"J:\\水\\cpu_dummy_writes.zip" );
	/*std::vector<uint8_t> vData;
	fbFile.LoadToMemory( vData );*/
	std::vector<std::u16string> vFiles;
	fbFile.GatherArchiveFiles( vFiles );
	std::vector<uint8_t> vExtracted;
	for ( auto I = vFiles.size(); I--; ) {
		fbFile.ExtractToMemory( vFiles[I], vExtracted );
		if ( vExtracted.size() ) {
			int gjhg = 0;
		}
	}
	return 0;
}