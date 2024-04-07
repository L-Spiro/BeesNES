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
		AudioObjectID defaultDeviceID;
		UInt32 dataSize = sizeof(defaultDeviceID);
		AudioObjectPropertyAddress propertyAddress = {
			kAudioHardwarePropertyDefaultOutputDevice,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMain
		};
		OSStatus result = ::AudioObjectGetPropertyData( kAudioObjectSystemObject, &propertyAddress, 0, NULL, &dataSize, &defaultDeviceID );
		
		if ( kAudioHardwareNoError != result ) {
			return false;
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
