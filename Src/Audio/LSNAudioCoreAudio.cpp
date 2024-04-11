/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The implementation of the OpenAL audio system.
 */



#include "LSNAudioCoreAudio.h"

#ifdef LSN_APPLE

#include "LSAApple.h"
#include "../Localization/LSNLocalization.h"

#include <AudioToolbox/AudioComponent.h>

namespace lsn {

	// == Functions.
	/**
	 * Initializes audio.
	 *
	 * \return Returns true if initialization was successful.
	 **/
	bool CAudioCoreAudio::InitializeAudio() {
		
		std::vector<AudioObjectID> vDeviceIds;
		if ( !GetAudioDevices( vDeviceIds ) ) { return false; }
		if ( !CreateAudioById( vDeviceIds[0] ) ) { return false; }

		return true;
	}

	/**
	 * Shuts down the audio.
	 *
	 * \return Returns true if shutdown was successful.
	 **/
	bool CAudioCoreAudio::ShutdownAudio() {
		DestroyAudio();
		return true;
	}

	/**
	 * Called when emulation begins.  Resets the ring buffer of buckets.
	 **/
	void CAudioCoreAudio::BeginEmulation() {
	}

	/**
	 * Gets the name of an audio device given its ID.
	 *
	 * \param _aoiId The ID of the device whose name is to be obtained.
	 * \return Returns the name of the given device or an empty string.
	 **/
	std::u8string CAudioCoreAudio::AudioDeviceName( AudioObjectID _aoiId ) {
		lsa::LSA_STRINGREF srName;

		UInt32 ui32DataSize = sizeof( CFStringRef );
		AudioObjectPropertyAddress aopaNameAddr = {
			kAudioDevicePropertyDeviceNameCFString,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMain
		};

		OSStatus sStatus = ::AudioObjectGetPropertyData( _aoiId, &aopaNameAddr, 0, nullptr, &ui32DataSize, &srName.srRef );

		if ( kAudioHardwareNoError == sStatus ) {
			try {
				return srName.ToString_u8();
			}
			catch ( ... ) {}
		}
		return {};
	}

	/**
	 * Gets the number of output streams of an audio device given its ID.
	 *
	 * \param _aoiId The ID of the device whose number of output streams is to be obtained.
	 * \return Returns the number of output streams of the given device.
	 **/
	UInt32 CAudioCoreAudio::AudioDeviceOutputStreams( AudioObjectID _aoiId ) {
		UInt32 ui32Ret = 0;
		
		// Use kAudioDevicePropertyStreams to get the number of output streams
		AudioObjectPropertyAddress aopaStreamsAddress = {
			kAudioDevicePropertyStreams,
			kAudioDevicePropertyScopeOutput,
			kAudioObjectPropertyElementMain
		};
		
		OSStatus sStatus = ::AudioObjectGetPropertyDataSize( _aoiId, &aopaStreamsAddress, 0, nullptr, &ui32Ret );

		if ( kAudioHardwareNoError == sStatus ) {
			return ui32Ret;
		}
		return 0;
	}

	/**
	 * Gets the default output device ID.
	 *
	 * \return Returns the default output device ID.
	 **/
	AudioObjectID CAudioCoreAudio::DefaultAudioDevice() {
		AudioObjectID aoiDefaultDeviceID;
		UInt32 ui32DataSize = sizeof( aoiDefaultDeviceID );
		AudioObjectPropertyAddress aopaPropertyAddress = {
			kAudioHardwarePropertyDefaultOutputDevice,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMain
		};
		OSStatus sStatus = ::AudioObjectGetPropertyData( kAudioObjectSystemObject, &aopaPropertyAddress, 0, NULL, &ui32DataSize, &aoiDefaultDeviceID );
		if ( kAudioHardwareNoError == sStatus ) {
			return aoiDefaultDeviceID;
		}
		return 0;
	}

	/**
	 * Gathers the sound devices into a vector with the default sound device in index 0.
	 *
	 * \param _vRet Holds the returned vector of audio devices by ID.
	 * \return Returns true if the sound devices were successfully gathered into a vector.
	 **/
	bool CAudioCoreAudio::GetAudioDevices( std::vector<AudioObjectID> &_vRet ) {
		AudioObjectID aoiDefId = DefaultAudioDevice();
		if ( aoiDefId == 0 ) { return false; }
		
		UInt32 ui32DataSize;
		AudioObjectPropertyAddress aopaPropAddr = {
			kAudioHardwarePropertyDevices,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMain
		};

		// Get the size of the property data.
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

		try {
			_vRet.push_back( aoiDefId );
		}
		catch ( ... ) { return false; }
		for ( UInt32 I = 0; I < ui32DeviceCnt; ++I ) {
			if ( vDeviceIds[I] != aoiDefId && AudioDeviceOutputStreams( vDeviceIds[I] ) >= 1 ) {
				try {
					_vRet.push_back( vDeviceIds[I] );
				}
				catch ( ... ) { return false; }
			}
		}
		return true;
	}

	/**
	 * Gets a Core Audio error string given the error code.
	 *
	 * \param _sError The error code to turn into a string.
	 * \return Returns a pointer to a constant string containing the string form of the error code and its descripton.
	 */
	const char8_t * CAudioCoreAudio::ErrorCodeToString( OSStatus _sError ) {
		switch ( _sError ) {
			case kAudioHardwareNoError : { return u8"kAudioHardwareNoError: " LSN_U8STR( LSN_AUDIO_HARDWARE_NO_ERROR ); }
			case kAudioHardwareNotRunningError : { return u8"kAudioHardwareNotRunningError: " LSN_U8STR( LSN_AUDIO_HARDWARE_NOT_RUNNING_ERROR ); }
			case kAudioHardwareUnspecifiedError : { return u8"kAudioHardwareUnspecifiedError: " LSN_U8STR( LSN_AUDIO_HARDWARE_UNSPECIFIED_ERROR ); }
			case kAudioHardwareUnknownPropertyError : { return u8"kAudioHardwareUnknownPropertyError: " LSN_U8STR( LSN_AUDIO_HARDWARE_UNKNOWN_PROPERTY_ERROR ); }
			case kAudioHardwareBadPropertySizeError : { return u8"kAudioHardwareBadPropertySizeError: " LSN_U8STR( LSN_AUDIO_HARDWARE_BAD_PROPERTY_SIZE_ERROR ); }
			case kAudioHardwareIllegalOperationError : { return u8"kAudioHardwareIllegalOperationError: " LSN_U8STR( LSN_AUDIO_HARDWARE_ILLEGAL_OPERATION_ERROR ); }
			case kAudioHardwareBadObjectError : { return u8"kAudioHardwareBadObjectError: " LSN_U8STR( LSN_AUDIO_HARDWARE_BAD_OBJECT_ERROR ); }
			case kAudioHardwareBadDeviceError : { return u8"kAudioHardwareBadDeviceError: " LSN_U8STR( LSN_AUDIO_HARDWARE_BAD_DEVICE_ERROR ); }
			case kAudioHardwareBadStreamError : { return u8"kAudioHardwareBadStreamError: " LSN_U8STR( LSN_AUDIO_HARDWARE_BAD_STREAM_ERROR ); }
			case kAudioHardwareUnsupportedOperationError : { return u8"kAudioHardwareUnsupportedOperationError: " LSN_U8STR( LSN_AUDIO_HARDWARE_UNSUPPORTED_OPERATION_ERROR ); }
			case kAudioHardwareNotReadyError : { return u8"kAudioHardwareNotReadyError: " LSN_U8STR( LSN_AUDIO_HARDWARE_NOT_READY_ERROR ); }
			case kAudioDeviceUnsupportedFormatError : { return u8"kAudioDeviceUnsupportedFormatError: " LSN_U8STR( LSN_AUDIO_DEVICE_UNSUPPORTED_FORMAT_ERROR ); }
			case kAudioDevicePermissionsError : { return u8"kAudioDevicePermissionsError: " LSN_U8STR( LSN_AUDIO_DEVICE_PERMISSIONS_ERROR ); }
			default : { return LSN_U8STR( LSN_CORE_AUDIO_ERROR_UNKNOWN ); }
		}
	}

	/**
	 * Creates an audio component given a device ID.
	 *
	 * \param _aoiId the audio device from which to create the AudioComponentInstance instance.
	 * \return Returns true if the audio instance was created.
	 **/
	bool CAudioCoreAudio::CreateAudioById( AudioObjectID _aoiId ) {
		DestroyAudio();
		// Set up the AudioComponentDescription to specify the type of AudioComponent.
		AudioComponentDescription acdDesc = {};
		acdDesc.componentType = kAudioUnitType_Output;
		acdDesc.componentSubType = kAudioUnitSubType_HALOutput; // For hardware audio interfaces.
		acdDesc.componentManufacturer = kAudioUnitManufacturer_Apple;

		// Find the AudioComponent that matches the description.
		AudioComponent acOutputComp = ::AudioComponentFindNext( nullptr, &acdDesc );

		// Declare an AudioComponentInstance variable.
		AudioComponentInstance aciUnit;

		// Instantiate the AudioComponentInstance with the found AudioComponent.
		OSStatus sStatus = ::AudioComponentInstanceNew( acOutputComp, &aciUnit );

		if ( kAudioHardwareNoError != sStatus ) {
			return false;
		}

		// Now, set the current device to the aciUnit instance.
		sStatus = ::AudioUnitSetProperty(
			aciUnit,
			kAudioOutputUnitProperty_CurrentDevice,
			kAudioUnitScope_Global,
			0,
			&_aoiId,
			sizeof( _aoiId )
		);

		if ( kAudioHardwareNoError != sStatus ) {
			return false;
		}

		// Finally, initialize the audio unit to prepare it for use.
		sStatus = ::AudioUnitInitialize( aciUnit );

		if ( kAudioHardwareNoError != sStatus ) {
			::AudioComponentInstanceDispose( aciUnit );
			return false;
		}

		// AudioUnit is now ready to be used with the selected device.
		m_aciInstance = aciUnit;
		return true;
	}

	/**
	 * Destroys the audio component.
	 **/
	void CAudioCoreAudio::DestroyAudio() {
		if ( m_aciInstance == nullptr ) { return; }
		
		
		// Uninitialize the audio unit.
		OSStatus sStatus = ::AudioUnitUninitialize( m_aciInstance );

		if ( kAudioHardwareNoError != sStatus ) {
			// Handle the error.
		}

		// Dispose of the audio unit.
		sStatus = ::AudioComponentInstanceDispose( m_aciInstance );

		if ( kAudioHardwareNoError != sStatus ) {
			// Handle the error.
		}

		// The audioUnit is now properly shut down and resources are freed.
		m_aciInstance = nullptr;
	}

}	// namespace lsn

#endif	// #ifdef LSN_APPLE
