/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Reads the CPU temperature.
 */

#pragma once

#include "../Utilities/LSNUtilities.h"

#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <random>
#include <thread>

#if defined( _WIN32 )
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment( lib, "wbemuuid.lib" )
#elif defined( __linux__ )
#include <fstream>
#elif defined( __APPLE__ )
#include <TargetConditionals.h>
#if TARGET_OS_OSX
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

/**
 * \brief macOS SMC version struct.
 */
typedef struct {
	uint8_t									major;
	uint8_t									minor;
	uint8_t									build;
	uint8_t									reserved[1];
	uint16_t								release;
} SMCKeyData_vers_t;

/**
 * \brief macOS SMC power limit struct.
 */
typedef struct {
	uint16_t								version;
	uint16_t								length;
	uint32_t								cpuPLimit;
	uint32_t								gpuPLimit;
	uint32_t								memPLimit;
} SMCKeyData_pLimitData_t;

/**
 * \brief macOS SMC key info struct.
 */
typedef struct {
	uint32_t								dataSize;
	uint32_t								dataType;
	uint8_t									dataAttributes;
} SMCKeyData_keyInfo_t;

/**
 * \brief macOS SMC parameter struct for I/O kit communication.
 */
typedef struct {
	uint32_t								key;
	SMCKeyData_vers_t						vers;
	SMCKeyData_pLimitData_t					pLimitData;
	SMCKeyData_keyInfo_t					keyInfo;
	uint8_t									result;
	uint8_t									status;
	uint8_t									data8;
	uint32_t								data32;
	uint8_t									bytes[32];
} SMCParamStruct;
#endif	// #if TARGET_OS_OSX
#endif	// #elif defined( __APPLE__ )

namespace lsn {
	
	/**
	 * Class CCpuTemperatureReader
	 * \brief Reads the CPU temperature.
	 *
	 * Description: Reads the CPU temperature.
	 */
	class CCpuTemperatureReader {
	public :
		CCpuTemperatureReader();
		~CCpuTemperatureReader();

		
		// == Functions.
		/**
		 * \brief Starts the polling thread. Safe to call multiple times.
		 */
		void									StartThread();

		/**
		 * \brief Stops the polling thread. Safe to call multiple times.
		 */
		void									StopThread();

		/**
		 * \brief Sets m_bPoll to true to trigger a temperature read on the background thread.
		 */
		void									PollTemperature();

		/**
		 * \brief Gets the most recently cached temperature from the background thread.
		 *
		 * \return Returns the cached temperature in Kelvin.
		 */
		float									GetCachedTemperature();

		/**
		 * \brief Reads the current CPU temperature.
		 *
		 * \return Returns the temperature in Kelvin, or -999.0f on failure.
		 */
		float									GetTemperature();

		/**
		 * \brief Gets the filtered current CPU temperature (returns 280.0 <= GetTemperature() <= 400.0).
		 * 
		 * \return Returns 280.0 <= GetTemperature() <= 400.0.
		 */
		inline float							FilteredTemperature() {
			return std::clamp( GetTemperature(), 280.0f, 400.0f );
		}

		/**
		 * \brief Gets the filtered current CPU temperature (returns 280.0 <= GetCachedTemperature() <= 400.0).
		 * 
		 * \return Returns 280.0 <= GetCachedTemperature() <= 400.0.
		 */
		inline float							FilteredCachedTemperature() {
			return std::clamp( GetCachedTemperature(), 280.0f, 400.0f );
		}


	private :
		// == Functions.
		/**
		 * \brief The background thread routine that sits, waits, and polls.
		 */
		void									ThreadRoutine();


		// == Members.
#if defined( _WIN32 )	
		/** \brief The WMI locator object. */
		IWbemLocator *							m_pLocator = NULL;
	
		/** \brief The WMI services connection. */
		IWbemServices *							m_pServices = NULL;

		/** \brief Flag indicating if COM was successfully initialized by this class. */
		bool									m_bComInitialized = false;
#elif defined( __linux__ )
		/** \brief The open file stream for the sysfs thermal zone. */
		std::ifstream							m_ifStream;
#elif defined( __APPLE__ )
#if TARGET_OS_OSX
		/** \brief The open data port connection to the AppleSMC service. */
		io_connect_t							m_icDataPort;
#endif
#endif

		/** \brief The background thread for polling the temperature. */
		std::thread								m_thThread;

		/** \brief Mutex to protect thread synchronization and shared data. */
		std::mutex								m_mMutex;

		/** \brief Condition variable to wake the polling thread. */
		std::condition_variable					m_cvWait;

		/** \brief The random-number device. */
		std::random_device						m_rdDev;

		/** \brief The random-number generator. */
		std::mt19937							m_mGen;

		/** \brief The cached temperature value. */
		float									m_fCachedTemperature = -999.0f;

		/** \brief Flag to instruct the thread to stop and exit. */
		bool									m_bStopThread = false;

		/** \brief Flag to instruct the thread to poll the temperature. */
		bool									m_bPoll = false;
	};

}	// namespace lsn
