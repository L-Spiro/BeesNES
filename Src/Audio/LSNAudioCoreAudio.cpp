/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The implementation of the OpenAL audio system.
 */



#include "LSNAudioCoreAudio.h"

#ifdef LSN_APPLE

namespace lsn {

	// == Functions.
	/**
	 * Initializes audio.
	 *
	 * \return Returns true if initialization was successful.
	 **/
	bool CAudioCoreAudio::InitializeAudio() {
		UInt32 ui32DataSize;
		AudioObjectPropertyAddress aopaPropAddr = {
			kAudioHardwarePropertyDevices,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMain
		};

		// Get the size of the property data
		OSStatus sStatus = ::AudioObjectGetPropertyDataSize( kAudioObjectSystemObject, &aopaPropAddr, 0, nullptr, &ui32DataSize );

		if ( kAudioHardwareNoError != sStatus ) { return false; }

		UInt32 ui32DeviceCnt = ui32DataSize / sizeof( AudioObjectID );
		std::vector<AudioObjectID> vDeviceIds;
		try {
			vDeviceIds.resize( ui32DeviceCnt );
		}
		catch ( ... ) { return false; }

		// Get the property data.
		sStatus = ::AudioObjectGetPropertyData( kAudioObjectSystemObject, &aopaPropAddr, 0, nullptr, &ui32DataSize, vDeviceIds.data() );

		if ( kAudioHardwareNoError != sStatus ) { return false; }

		for ( UInt32 I = 0; I < ui32DeviceCnt; ++I ) {
			AudioObjectPropertyAddress aopaThisDevice = {
				kAudioDevicePropertyStreamConfiguration,
				kAudioDevicePropertyScopeOutput,
				kAudioObjectPropertyElementMain
			};
			sStatus = ::AudioObjectGetPropertyDataSize( vDeviceIds[I], &aopaThisDevice, 0, nullptr, &ui32DataSize );
			if ( kAudioHardwareNoError != sStatus && ui32DataSize > 0 ) {
				// This device has output streams, so it's an output device.
			}
		}


		return true;
	}

	/**
	 * Shuts down the audio.
	 *
	 * \return Returns true if shutdown was successful.
	 **/
	bool CAudioCoreAudio::ShutdownAudio() {
		return true;
	}

	/**
	 * Called when emulation begins.  Resets the ring buffer of buckets.
	 **/
	void CAudioCoreAudio::BeginEmulation() {
	}

}	// namespace lsn

#endif	// #ifdef LSN_APPLE
