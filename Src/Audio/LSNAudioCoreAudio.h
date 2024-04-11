/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The implementation of the Core Audio audio system.
 */


#pragma once

#include "../LSNLSpiroNes.h"

#ifdef LSN_APPLE

#include "LSNAudioBase.h"

#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>

namespace lsn {

    /**
     * Class CAudioCoreAudio
     * \brief The implementation of the Core Audio audio system.
     *
     * Description: The implementation of the Core Audio audio system.
     */
    class CAudioCoreAudio : public CAudioBase {
	public :
        // == Functions.
        /**
         * Initializes audio.
         *
         * \return Returns true if initialization was successful.
         **/
        virtual bool                                        InitializeAudio();
        
        /**
         * Shuts down the audio.
         *
         * \return Returns true if shutdown was successful.
         **/
        virtual bool                                        ShutdownAudio();
        
        /**
         * Called when emulation begins.  Resets the ring buffer of buckets.
         **/
        virtual void                                        BeginEmulation();
		
		/**
		 * Gets the name of an audio device given its ID.
		 *
		 * \param _aoiId The ID of the device whose name is to be obtained.
		 * \return Returns the name of the given device or an empty string.
		 **/
		static std::u8string								AudioDeviceName( AudioObjectID _aoiId );
		
		/**
		 * Gets the number of output streams of an audio device given its ID.
		 *
		 * \param _aoiId The ID of the device whose number of output streams is to be obtained.
		 * \return Returns the number of output streams of the given device.
		 **/
		static UInt32										AudioDeviceOutputStreams( AudioObjectID _aoiId );
		
		/**
		 * Gets the default output device ID.
		 *
		 * \return Returns the default output device ID.
		 **/
		static AudioObjectID								DefaultAudioDevice();
		
		/**
		 * Gathers the sound devices into a vector with the default sound device in index 0.
		 *
		 * \param _vRet Holds the returned vector of audio devices by ID.
		 * \return Returns true if the sound devices were successfully gathered into a vector.
		 **/
		static bool											GetAudioDevices( std::vector<AudioObjectID> &_vRet );
		
		/**
		 * Gets a Core Audio error string given the error code.
		 *
		 * \param _sError The error code to turn into a string.
		 * \return Returns a pointer to a constant string containing the string form of the error code and its descripton.
		 */
		static const char8_t *								ErrorCodeToString( OSStatus _sError );
		
	protected :
		// == Members.
		AudioComponentInstance								m_aciInstance = nullptr;					/**< The audio component instance. */
		
		
		// == Functions.
		/**
		 * Creates an audio component given a device ID.
		 *
		 * \param _aoiId the audio device from which to create the AudioComponentInstance instance.
		 * \return Returns true if the audio instance was created.
		 **/
		bool												CreateAudioById( AudioObjectID _aoiId );
		
		/**
		 * Destroys the audio component.
		 **/
		void												DestroyAudio();
    };

}   // namespace lsn

#endif  // #ifdef LSN_APPLE
