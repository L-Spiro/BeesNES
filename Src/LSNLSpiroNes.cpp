#include "LSNLSpiroNes.h"

#ifdef LSN_WINDOWS
#include "Input/LSNDirectInput8.h"
#include "Audio/LSNAudio.h"
#endif	// #ifdef LSN_WINDOWS

#ifdef LSN_USE_WINDOWS
#include "Windows/Layout/LSNLayoutManager.h"
#include "Windows/MainWindow/LSNMainWindow.h"
#include "Windows/MainWindow/LSNMainWindowLayout.h"
#endif	// #ifdef LSN_USE_WINDOWS

#ifdef LSN_DX9
#include "GPU/DirectX9/LSNDirectX9.h"
#endif	// #ifdef LSN_DX9
#ifdef LSN_DX12
#include "GPU/DirectX12/LSNDirectX12.h"
#endif	// #ifdef LSN_DX12
#ifdef LSN_VULKAN1
#include "GPU/Vulkan/LSNVulkan.h"
#endif	// #ifdef LSN_VULKAN1

#ifdef LSN_CPU_VERIFY
#include "File/LSNStdFile.h"
#endif	// #ifdef LSN_CPU_VERIFY

#include "ColorSpace/LSNColorSpace.h"
#include "Time/LSNTimer.h"

int main() {
	return 0;
}

#ifdef LSN_USE_WINDOWS
#if !defined( LSN_CPU_VERIFY )
int WINAPI wWinMain( _In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE /*_hPrevInstance*/, _In_ LPWSTR /*_lpCmdLine*/, _In_ int /*_nCmdShow*/ ) {
	lsw::CBase::Initialize( _hInstance, new lsn::CLayoutManager(),
		L"LSNDOCK",
		L"LSNSPLITTER",
		L"LSNMULTISPLITTER",
		L"LSNTREEVIEW",
		L"LSNTREELISTVIEW",
		L"LSNCHILDWINDOW" );
	lsn::CDatabase::Init();
	lsn::CDirectInput8::CreateDirectInput8();
	lsn::CAudio::InitializeAudio();

	std::atomic_bool abIsAlive = false;
	lsn::CMainWindow * pwMainWindow = static_cast<lsn::CMainWindow *>(lsn::CMainWindowLayout::CreateMainWindow( &abIsAlive ));
	{
		lsn::CWavFile wfFile;
		wfFile.Open( u"C:\\My Projects\\BeesNES\\Research\\MDFourierRefs\\mdf4kv9_dc\\mdf4kv9_dc.wav", 0xFFFFFFFF/*, 199408534 - 52*/ );
	}

	//auto mMat =
	//lsn::CColorSpace::ComputeRgbToXyzMatrix( 0.630f, 0.340f,
	//	0.310f, 0.595f,
	//	0.155f, 0.070f,
	//	0.3127f, 0.3290f );

	//auto a1953 = lsn::CColorSpace::ComputeRgbToXyzMatrix( lsn::CColorSpace::m_fSmpte );
	//auto aSrgb = lsn::CColorSpace::ComputeXyzToRgbMatrix( lsn::CColorSpace::m_fsRgb );
	//auto aTmpA_Mat = aSrgb * a1953;
	//auto aTmpB_Mat = lsn::CMatrix4x4<4>::MultiplyMatrix( a1953, aSrgb );

	//lsn::CVector4<0> vColor0( 0.5f, 0.5f, 0.5f, 1.0f );
	//lsn::CVector4<4> vColor1( 0.5f, 0.5f, 0.5f, 1.0f );

	//auto vColor0_Mul = aTmpA_Mat * vColor0;
	//auto vColor1_Mul = aSrgb * (a1953 * vColor1);

	//lsn::CVector4<0> vGamma = vColor0_Mul.XyzFunc( lsn::CUtilities::LinearTosRGB );

	//lsn::CVector4<0> vArray[4] = {
	//	vColor0,
	//	vColor1,
	//	vColor0,
	//	vColor1,
	//};
	//LSN_ALIGN( 64 )
	//float fOut[16];

	//constexpr size_t sTotal = 256 * 8 * 240 * 4;
	//std::vector<lsn::CVector4<0>> vArrayOut( sTotal );
	//std::vector<lsn::CVector4<0>> vArrayIn( sTotal );
	//lsn::CMatrix4x4<4>::MultiplyVec4_2_XYZ( aTmpA_Mat, vArray, vArrayOut.data() );

	//for ( size_t I = 0; I < sTotal; ++I ) {
	//	vArrayIn[I] = vArray[I&3];
	//}
	//
	//lsn::CTimer tTime;
	//auto aAddr = vArrayOut.data();
 //   
	//float fSum = 0.0f;
	//tTime.Start();
	//for ( size_t I = 0; I < sTotal; ++I ) {
	//	(*reinterpret_cast<lsn::CVector4<0> *>(aAddr++)) = lsn::CMatrix4x4<0>::MultiplyVec4_XYZ( aTmpA_Mat, vArrayIn[I] );
	//}
	//::OutputDebugStringA( (std::string( "lsn::CMatrix4x4<0>::MultiplyVec4_XYZ " ) + std::to_string( tTime.ElapsedSeconds() ) + "s. " + std::to_string( fSum ) + "\r\n").c_str() );

	//fSum = 0.0f;
	//aAddr = vArrayOut.data();
	//tTime.Reset();
	//tTime.Start();
	//for ( size_t I = 0; I < sTotal; ++I ) {
	//	(*reinterpret_cast<lsn::CVector4<4> *>(aAddr++)) = lsn::CMatrix4x4<4>::MultiplyVec4_XYZ( aTmpA_Mat, vArrayIn[I] );
	//}
	//::OutputDebugStringA( (std::string( "lsn::CMatrix4x4<4>::MultiplyVec4_XYZ " ) + std::to_string( tTime.ElapsedSeconds() ) + "s. " + std::to_string( fSum ) + "\r\n").c_str() );




	//aAddr = vArrayOut.data();
	//for ( size_t I = 0; I < sTotal; I += 2, aAddr += 2 ) {
	//	lsn::CMatrix4x4<0>::MultiplyVec4_2( aTmpA_Mat, &vArrayIn[I], aAddr );
	//}
	//fSum = 0.0f;
	//aAddr = vArrayOut.data();
	//tTime.Reset();
	//tTime.Start();
	//for ( size_t I = 0; I < sTotal; I += 2, aAddr += 2 ) {
	//	lsn::CMatrix4x4<4>::MultiplyVec4_2( aTmpA_Mat, &vArrayIn[I], aAddr );
	//	fSum++;
	//	//fSum += vArrayOut[0].X() + vArrayOut[1].X();
	//	/*if ( (I & 3) == 0 ) {
	//		LSN_PREFETCH_LINE( aAddr + 4 );
	//	}*/
	//}
	//::OutputDebugStringA( (std::string( "lsn::CMatrix4x4<4>::MultiplyVec4_2 " ) + std::to_string( tTime.ElapsedSeconds() ) + "s. " + std::to_string( fSum ) + "\r\n").c_str() );

	//fSum = 0.0f;
	//aAddr = vArrayOut.data();
	//tTime.Reset();
	//tTime.Start();
	//for ( size_t I = 0; I < sTotal; I += 2, aAddr += 2 ) {
	//	lsn::CMatrix4x4<4>::MultiplyVec4_2_XYZ( aTmpB_Mat, &vArrayIn[I], aAddr );
	//	fSum++;
	//	//fSum += vArrayOut[0].X();// + vArrayOut[1].X();
	//}
	//::OutputDebugStringA( (std::string( "lsn::CMatrix4x4<4>::MultiplyVec4_2_XYZ " ) + std::to_string( tTime.ElapsedSeconds() ) + "s. " + std::to_string( fSum ) + "\r\n").c_str() );

	//fSum = 0.0f;
	//aAddr = vArrayOut.data();
	//tTime.Reset();
	//tTime.Start();
	//for ( size_t I = 0; I < sTotal; I += 4, aAddr += 4 ) {
	//	lsn::CMatrix4x4<4>::MultiplyVec4_4( aTmpB_Mat, &vArrayIn[I], aAddr );
	//	fSum++;
	//	//fSum += vArrayOut[0].X();// + vArrayOut[1].X();
	//}
	//::OutputDebugStringA( (std::string( "lsn::CMatrix4x4<4>::MultiplyVec4_4 " ) + std::to_string( tTime.ElapsedSeconds() ) + "s. " + std::to_string( fSum ) + "\r\n").c_str() );

	//fSum = 0.0f;
	//aAddr = vArrayOut.data();
	//tTime.Reset();
	//tTime.Start();
	//for ( size_t I = 0; I < sTotal; I += 4, aAddr += 4 ) {
	//	lsn::CMatrix4x4<4>::MultiplyVec4_4_XYZ( aTmpB_Mat, &vArrayIn[I], aAddr );
	//	fSum++;
	//	//fSum += vArrayOut[0].X();// + vArrayOut[1].X();
	//}
	//::OutputDebugStringA( (std::string( "lsn::CMatrix4x4<4>::MultiplyVec4_4_XYZ " ) + std::to_string( tTime.ElapsedSeconds() ) + "s. " + std::to_string( fSum ) + "\r\n").c_str() );

	// Controls seconds_since_start(), milliseconds_since_start(), etc., Expression Evaluator.
	// We move it up as close to the start of the loop as possible so that these values most closely mark the actual time that meaningful execution
	//	takes place (clock() returns the time since the EXE actually started (before main() is even called), so we don't need more tickers from that
	//	time).
	// In a way, this allows (clock() - milliseconds_since_start()) to print the time it takes to initialize.
	ee::CExpEval::InitializeExpressionEvaluatorLibrary();
	MSG mMsg = {};
	::PeekMessageW( &mMsg, NULL, 0U, 0U, PM_NOREMOVE );

	while ( mMsg.message != WM_QUIT ) {
		// Use ::PeekMessage() so we can use idle time to render the scene.
		while ( (::PeekMessageW( &mMsg, NULL, 0U, 0U, PM_REMOVE ) != 0) ) {
			// Translate and dispatch the message.
			if ( !abIsAlive ) { break; }
			if ( ::TranslateAcceleratorW( pwMainWindow->Wnd(), NULL, &mMsg ) == 0 ) {
				::TranslateMessage( &mMsg );
				::DispatchMessageW( &mMsg );
			}
		}
		if ( !abIsAlive ) { break; }
		if ( mMsg.message != WM_QUIT ) {
			pwMainWindow->Tick();
		}
	}

#ifdef LSN_DX9
	lsn::CDirectX9::ShutDown();
#endif	// #ifdef LSN_DX9
	#ifdef LSN_DX12
	lsn::CDirectX12::ShutDown();
#endif	// #ifdef LSN_DX12
#ifdef LSN_VULKAN1
	lsn::CVulkan::ShutDown();
#endif	// #ifdef LSN_VULKAN1

	lsn::CAudio::ShutdownAudio();
	lsw::CBase::ShutDown();
	lsn::CDatabase::Reset();
	lsn::CDirectInput8::Release();
	return static_cast<int>(mMsg.wParam);
}
#else	// #if !defined( LSN_CPU_VERIFY )
#include "LSONJson.h"
int WINAPI wWinMain( _In_ HINSTANCE /*_hInstance*/, _In_opt_ HINSTANCE /*_hPrevInstance*/, _In_ LPWSTR /*_lpCmdLine*/, _In_ int /*_nCmdShow*/ ) {
	std::unique_ptr<lsn::CCpuBus> pbBus = std::make_unique<lsn::CCpuBus>();
	pbBus->ApplyMap();
	std::unique_ptr<lsn::CCpu6502> pcCpu = std::make_unique<lsn::CCpu6502>( pbBus.get() );

	std::wstring wsBuffer;
	const DWORD dwSize = 0xFFFF;
	wsBuffer.resize( dwSize + 1 ); 
	::GetModuleFileNameW( NULL, wsBuffer.data(), dwSize );
	PWSTR pwsEnd = std::wcsrchr( wsBuffer.data(), L'\\' ) + 1;
	std::wstring wsRoot = wsBuffer.substr( 0, pwsEnd - wsBuffer.data() );
	{
		for ( uint32_t I = 0x00; I < 256; ++I ) {
			std::wstring wsFile;
			lson::CJson jSon;
			std::vector<uint8_t> vBytes;
			lsn::CStdFile sfFile;
			wchar_t wcFile[64];
			std::swprintf( wcFile, L"..\\..\\Research\\nes6502\\v1\\%.2X.json", I );
			if ( sfFile.Open( reinterpret_cast<const char16_t *>((wsRoot + wcFile).c_str()) ) ) {
				sfFile.LoadToMemory( vBytes );
				vBytes.push_back( 0 );

				if ( !jSon.SetJson( reinterpret_cast<const char *>(vBytes.data()) ) ) {
					::OutputDebugStringA( "JSON FAIL\r\n" );
				}
				else {
					pcCpu->ResetToKnown();
					pbBus->ApplyMap();

					const lson::CJsonContainer::LSON_JSON_VALUE & jvRoot = jSon.GetContainer()->GetValue( jSon.GetContainer()->GetRoot() );
					for ( size_t J = 0; J < jvRoot.vArray.size(); ++J ) {
						const lson::CJsonContainer::LSON_JSON_VALUE & jvThis = jSon.GetContainer()->GetValue( jvRoot.vArray[J] );
						if ( !pcCpu->RunJsonTest( jSon, jvThis ) ) {
							//volatile int hkhj = 0;
						}
					}

					::OutputDebugStringA( "JSON NOT FAIL\r\n" );
					::OutputDebugStringW( wcFile );
					::OutputDebugStringA( "\r\n" );
				}
			}
		}
	}
	return 0;
}
#endif	// #if !defined( LSN_CPU_VERIFY )
#else
int wmain( int /*_iArgC*/, wchar_t * /*_pwcArgv*/[] ) {
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
#define LSN_TIME								(1ULL * 10)
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
		"Master Cycles: %llu (%.8f per second; ideal: %.8f).\r\n"
		"%.8f cycles per Tick().\r\n"
		"%.8f FPS.\r\n",
		ui64TickCount, dTime,
		pnsSystem->GetMasterCounter(), pnsSystem->GetMasterCounter() / dTime, double( pnsSystem->MasterHz() ) / pnsSystem->MasterDiv(),
		pnsSystem->GetMasterCounter() / double( ui64TickCount ),
		pnsSystem->GetPpu().FrameCount() / dTime
		);
	::OutputDebugStringA( szBuffer );
	return 0;
}
#endif	// #ifdef LSN_USE_WINDOWS
