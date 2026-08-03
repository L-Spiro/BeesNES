/**
 * Copyright L. Spiro 2026
 *
 * Written by : Shawn (L. Spiro) Wilcoxen
 *
 * Description : Implementation of the CCpuTemperatureReader class.
 */

#include "LSNCpuTemperatureReader.h"

#include <cstring>

namespace lsn {

	CCpuTemperatureReader::CCpuTemperatureReader() :
		m_mGen( m_rdDev() ) {
#if defined( _WIN32 )
		HRESULT hRes = ::CoInitializeEx( 0, COINIT_MULTITHREADED );
		if ( SUCCEEDED( hRes ) ) {
			m_bComInitialized = true;
		
			hRes = ::CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL );
			// RPC_E_TOO_LATE indicates COM security already initialized; ignore.
			if ( SUCCEEDED( hRes ) || hRes == RPC_E_TOO_LATE ) {
				hRes = ::CoCreateInstance( CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<LPVOID *>( &m_pLocator ) );
				if ( SUCCEEDED( hRes ) ) {
					hRes = m_pLocator->ConnectServer( _bstr_t( L"ROOT\\WMI" ), NULL, NULL, 0, NULL, 0, 0, &m_pServices );
					if ( SUCCEEDED( hRes ) ) {
						hRes = ::CoSetProxyBlanket( m_pServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );
						if ( FAILED( hRes ) ) {
							m_pServices->Release();
							m_pServices = NULL;
						}
					}
				}
			}
		}
#elif defined( __linux__ )
		m_ifStream.open( "/sys/class/thermal/thermal_zone0/temp" );
#elif defined( __APPLE__ )
#if TARGET_OS_OSX
		m_icDataPort = 0;
		mach_port_t mpMasterPort;
		kern_return_t krResult = ::IOMainPort( MACH_PORT_NULL, &mpMasterPort );
	
		if ( krResult == KERN_SUCCESS ) {
			CFMutableDictionaryRef dictMatch = ::IOServiceMatching( "AppleSMC" );
			io_iterator_t itIterator;
			krResult = ::IOServiceGetMatchingServices( mpMasterPort, dictMatch, &itIterator );

			if ( krResult == KERN_SUCCESS ) {
				io_object_t ioDevice = ::IOIteratorNext( itIterator );
				if ( ioDevice ) {
					::IOServiceOpen( ioDevice, mach_task_self(), 0, &m_icDataPort );
					::IOObjectRelease( ioDevice );
				}
				::IOObjectRelease( itIterator );
			}
		}
#endif
#endif
	}

	CCpuTemperatureReader::~CCpuTemperatureReader() {
		StopThread();
#if defined( _WIN32 )
		if ( m_pServices ) {
			m_pServices->Release();
		}
		if ( m_pLocator ) {
			m_pLocator->Release();
		}
		if ( m_bComInitialized ) {
			::CoUninitialize();
		}
#elif defined( __linux__ )
		if ( m_ifStream.is_open() ) {
			m_ifStream.close();
		}
#elif defined( __APPLE__ )
#if TARGET_OS_OSX
		if ( m_icDataPort ) {
			::IOServiceClose( m_icDataPort );
		}
#endif
#endif
	}


	// == Functions.
	/**
	 * \brief Starts the polling thread. Safe to call multiple times.
	 */
	void CCpuTemperatureReader::StartThread() {
		std::unique_lock<std::mutex> uLock( m_mMutex );
		if ( m_thThread.joinable() ) {
			return;
		}
	
		m_bStopThread = false;
		m_bPoll = false;
		m_fCachedTemperature = GetTemperature();
		m_sNoiseBufferIdx = m_sTickCnt = 0;
		m_thThread = std::thread( &CCpuTemperatureReader::ThreadRoutine, this );
	}

	/**
	 * \brief Stops the polling thread. Safe to call multiple times.
	 */
	void CCpuTemperatureReader::StopThread() {
		{
			std::unique_lock<std::mutex> uLock( m_mMutex );
			if ( !m_thThread.joinable() ) {
				return;
			}
		
			m_bStopThread = true;
			m_cvWait.notify_one();
		}
		m_thThread.join();
	}

	/**
	 * \brief Sets m_bPoll to true to trigger a temperature read on the background thread.
	 */
	void CCpuTemperatureReader::PollTemperature() {
		std::unique_lock<std::mutex> uLock( m_mMutex );
		m_bPoll = true;
		m_cvWait.notify_one();
	}

	/**
	 * \brief Gets the most recently cached temperature from the background thread.
	 *
	 * \return Returns the cached temperature in Kelvin.
	 */
	float CCpuTemperatureReader::GetCachedTemperature() {
		std::unique_lock<std::mutex> uLock( m_mMutex );
		return m_fCachedTemperature;
	}

	/**
	 * \brief The background thread routine that sits, waits, and polls.
	 */
	void CCpuTemperatureReader::ThreadRoutine() {
		while ( true ) {
			std::unique_lock<std::mutex> uLock( m_mMutex );
		
			m_cvWait.wait( uLock, [this]() {
				return m_bStopThread || m_bPoll;
			} );

			if ( m_bStopThread ) { break; }

			if ( m_bPoll ) {
				m_bPoll = false;
			
				// Unlock before doing hardware I/O to avoid blocking GetCachedTemperature() calls on the main thread.
				uLock.unlock(); 

				float fTemp = GetTemperature();

				// Re-lock to safely update the cached result.
				uLock.lock();
				m_fCachedTemperature = fTemp;
			}
		}
	}

	/**
	 * \brief Reads the current CPU temperature.
	 *
	 * \return Returns the temperature in Kelvin, or -999.0f on failure.
	 */
	float CCpuTemperatureReader::GetTemperature() {
#if defined( _WIN32 )
		if ( !m_pServices ) {
			return -999.0f;
		}

		IEnumWbemClassObject * pEnumerator = NULL;
		HRESULT hRes = m_pServices->ExecQuery( bstr_t( "WQL" ), bstr_t( "SELECT * FROM MSAcpi_ThermalZoneTemperature" ), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator );

		float fTempKelvin = -999.0f;
		if ( SUCCEEDED( hRes ) ) {
			IWbemClassObject * pclsObj = NULL;
			ULONG uReturn = 0;

			hRes = pEnumerator->Next( WBEM_INFINITE, 1, &pclsObj, &uReturn );
			if ( 0 != uReturn ) {
				VARIANT vtProp{};
				::VariantInit( &vtProp );
				hRes = pclsObj->Get( L"CurrentTemperature", 0, &vtProp, 0, 0 );
				if ( SUCCEEDED( hRes ) ) {
					long lTempKelvinTenths = vtProp.lVal;
					fTempKelvin = (static_cast<float>(lTempKelvinTenths) / 10.0f);
					::VariantClear( &vtProp );
				}
			
				pclsObj->Release();
			}
			pEnumerator->Release();
		}

		return fTempKelvin;
#elif defined( __linux__ )
		if ( m_ifStream.is_open() ) {
			m_ifStream.clear();
			m_ifStream.seekg( 0 );
		
			int iTempMicro;
			m_ifStream >> iTempMicro;
			return (static_cast<float>(iTempMicro) / 1000.0f) + 273.15f;
		}
		else {
			return -999.0f;
		}
#elif defined( __APPLE__ )
#if TARGET_OS_OSX
		if ( !m_icDataPort ) {
			return -999.0f;
		}
	
		SMCParamStruct spsInputStruct;
		SMCParamStruct spsOutputStruct;
		std::memset( &spsInputStruct, 0, sizeof( SMCParamStruct ) );
		std::memset( &spsOutputStruct, 0, sizeof( SMCParamStruct ) );

		
		spsInputStruct.key = ('T' << 24) | ('C' << 16) | ('0' << 8) | 'P';
		spsInputStruct.data8 = 5; // SMC_CMD_READ_BYTES
		spsInputStruct.keyInfo.dataSize = 2;

		size_t sInputSize = sizeof( SMCParamStruct );
		size_t sOutputSize = sizeof( SMCParamStruct );

		// KERNEL_INDEX_SMC = 2.
		kern_return_t krResult = ::IOConnectCallStructMethod( m_icDataPort, 2, &spsInputStruct, sInputSize, &spsOutputStruct, &sOutputSize );
	
		if ( krResult == KERN_SUCCESS ) {
			return static_cast<float>(spsOutputStruct.bytes[0]) + (static_cast<float>(spsOutputStruct.bytes[1]) / 256.0f) + 273.15f;
		}
	
		return -999.0f;
#else
		return -999.0f;
#endif
#else
		return -999.0f;
#endif
	}

	/**
	 * \brief Performs a per-frame update.
	 */
	void CCpuTemperatureReader::Tick() {
	}

}	// namespace lsn