/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Uses OpenAL or Core Audio to handle all for our audio needs.  As an emulator, we just need a single device, context, and source.
 *	This class owns and manages the objects and provides the interface we need to generate audio.
 */

#include "LSNAudio.h"
#include "../Utilities/LSNUtilities.h"

namespace lsn {

	// == Members.
	/** The audio interface object. */
	CAudio::CAudioDevice CAudio::m_adAudioDevice;

	/** The audio thread. */
	//std::unique_ptr<std::thread> CAudio::m_ptAudioThread;

	/** Boolean to stop the audio thread. */
	//std::atomic<bool> CAudio::m_bRunThread = true;

	/** The signal that the thread has finished. */
	//CEvent CAudio::m_eThreadClosed;

	/** The sample box for band-passed output. */
	CSampleBox CAudio::m_sbSampleBox;

	/** The index of the audio device being used. */
	uint32_t CAudio::m_ui32AudioDeviceIdx = uint32_t( ~0 );

	/** The audio devices. */
	std::vector<std::u16string> CAudio::m_vAudioDevices;

	/** The supported audio formats. */
	std::vector<uint32_t> CAudio::m_vSupportedFormats;


	// == Functions.
	/**
	 * Initializes audio.
	 * 
	 * \param _ui32Device The audio device to use.
	 * \return Returns true if initialization was successful.
	 **/
	bool CAudio::InitializeAudio( uint32_t _ui32Device ) {
		m_ui32AudioDeviceIdx = uint32_t( ~0 );
		m_sbSampleBox.SetFeatureSet( CUtilities::IsAvx512FSupported(), CUtilities::IsAvxSupported(), CUtilities::IsSse4Supported(), CUtilities::IsFmaSupported() );

		if ( !m_adAudioDevice.InitializeAudio( _ui32Device ) ) { return false; }

		if ( !StartThread() ) { return false; }
		m_ui32AudioDeviceIdx = _ui32Device;
		m_vAudioDevices = m_adAudioDevice.GetAudioDevices();
		m_vSupportedFormats = m_adAudioDevice.GetAudioFormatsAndHz();
		return true;
	}

	/**
	 * Shuts down the audio.
	 * 
	 * \return Returns true if shutdown was successful.
	 **/
	bool CAudio::ShutdownAudio() {
		StopThread();
		
		return m_adAudioDevice.ShutdownAudio();
	}

	/**
	 * Sets all output settings.
	 * 
	 * \param _aoSettings The new settings to apply.
	 **/
	void CAudio::SetOutputSettings( const LSN_AUDIO_OPTIONS &_aoSettings ) {
		SetOutputFormat( _aoSettings.afFormat );
		SetOutputFrequency( _aoSettings.ui32OutputHz );
		m_adAudioDevice.SetDither( _aoSettings.bDither );
	}

	/**
	 * Called when emulation begins.  Resets the ring buffer of buckets.
	 **/
	void CAudio::BeginEmulation() {
		StopThread();

		m_adAudioDevice.BeginEmulation();

		StartThread();
	}

	/**
	 * Adds a sample to all buckets that need it.
	 *
	 * \param _fSample The audio sample to be added.
	 **/
	void CAudio::AddSample( float _fSample ) {
		m_sbSampleBox.AddSample( _fSample );
		std::vector<float> & vOut = m_sbSampleBox.Output();
		for ( size_t I = 0; I < vOut.size(); ++I ) {
			m_adAudioDevice.AddSample( vOut[I] );
		}
		vOut.clear();
	}

	/**
	 * Starts the audio thread.
	 * 
	 * \return Returns true if the audio thread is started.
	 **/
	bool CAudio::StartThread() {
		return true;	// No audio thread yet.
		/*StopThread();
		m_bRunThread = true;
		m_ptAudioThread = std::make_unique<std::thread>( AudioThread, nullptr );
		return m_ptAudioThread.get() != nullptr;*/
	}

	/**
	 * Stops the audio thread.
	 **/
	void CAudio::StopThread() {
		/*if ( nullptr == m_ptAudioThread.get() ) { return; }
		m_bRunThread = false;
		m_eThreadClosed.WaitForSignal();
		m_ptAudioThread->join();
		m_ptAudioThread.reset();*/
	}

	/**
	 * The audio thread.
	 *
	 * \param _pvParm Unused.
	 */
	void CAudio::AudioThread( void * /*_pvParm*/ ) {
		/*while ( m_bRunThread ) {
		}
		m_eThreadClosed.Signal();*/
	}

}	// namespace lsn
