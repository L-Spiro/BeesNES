/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Streams captured frames and audio samples to FFmpeg to make a movie file.
 */

#include "LSNVideoExporter.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <format>
#include <numeric>
#include <set>
#include <sstream>


namespace lsn {

	namespace {

		/** How long to wait for FFmpeg to finish a file after its inputs end. */
		constexpr uint32_t ui32FinishTimeoutMs = 5 * 60 * 1000;

		/** How long to let the format check and the test encode run. */
		constexpr uint32_t ui32CheckTimeoutMs = 60 * 1000;

		/**
		 * Converts an ASCII/UTF-8 string to a wide string.
		 *
		 * \param _pcString The string to convert.
		 * \return Returns the wide string.
		 */
		std::wstring											Wide( const char * _pcString ) {
			return _pcString ? CFfmpegProcess::FromUtf8( _pcString ) : std::wstring();
		}

		/**
		 * Splits a string of space-separated FFmpeg arguments, replacing each "{Q}" with a value, and appends them.
		 *
		 * \param _pcArgs The arguments.  May be nullptr.
		 * \param _pcValue The text that replaces "{Q}", or nullptr.
		 * \param _vArgs The vector to which to append the arguments.
		 */
		void													AppendArgs( const char * _pcArgs, const char * _pcValue, std::vector<std::wstring> &_vArgs ) {
			if ( !_pcArgs ) { return; }
			std::string sArgs = _pcArgs;
			std::istringstream issStream( sArgs );
			std::string sToken;
			while ( issStream >> sToken ) {
				if ( _pcValue ) {
					for ( size_t sPos = sToken.find( "{Q}" ); sPos != std::string::npos; sPos = sToken.find( "{Q}", sPos ) ) {
						sToken.replace( sPos, 3, _pcValue );
						sPos += std::strlen( _pcValue );
					}
				}
				_vArgs.push_back( Wide( sToken.c_str() ) );
			}
		}

		/**
		 * Gets the names listed by "ffmpeg -encoders", "-muxers", or "-filters":  the first word after the flags on each line.
		 *
		 * \param _sOutput The output of FFmpeg.
		 * \return Returns the names.
		 */
		std::set<std::string>									ParseList( const std::string &_sOutput ) {
			std::set<std::string> sRet;
			std::istringstream issLines( _sOutput );
			std::string sLine;
			while ( std::getline( issLines, sLine ) ) {
				std::istringstream issLine( sLine );
				std::string sFlags, sName;
				if ( !(issLine >> sFlags >> sName) ) { continue; }
				// Names can be comma-separated lists (such as "mov,mp4,m4a").
				size_t sStart = 0;
				while ( sStart <= sName.size() ) {
					size_t sEnd = sName.find( ',', sStart );
					if ( sEnd == std::string::npos ) { sEnd = sName.size(); }
					if ( sEnd > sStart ) { sRet.insert( sName.substr( sStart, sEnd - sStart ) ); }
					sStart = sEnd + 1;
				}
			}
			return sRet;
		}

		/**
		 * Formats a rate as "num/den".
		 *
		 * \param _rRate The rate.
		 * \return Returns the formatted rate.
		 */
		std::string												RateStr( CVideoExporter::LSN_RATE _rRate ) {
			return std::to_string( _rRate.ui32Num ) + "/" + std::to_string( _rRate.ui32Den );
		}

		/**
		 * Deletes a file, ignoring errors.
		 *
		 * \param _wsPath The file to delete.
		 */
		void													DeleteFile( const std::wstring &_wsPath ) {
			std::error_code ecErr;
			std::filesystem::remove( std::filesystem::path( _wsPath ), ecErr );
		}

	}	// namespace


	// == CSession.
	CVideoExporter::CSession::CSession( CVideoExporter * _pveOwner ) :
		m_pveOwner( _pveOwner ),
		m_abFailed( false ) {
	}
	CVideoExporter::CSession::~CSession() {
		if ( m_bStarted ) {
			// End() was not called (only on an unexpected path).  Do not wait on FFmpeg; just stop the threads.
			{
				std::lock_guard<std::mutex> lgLock( m_mMutex );
				m_bClosing = true;
			}
			m_fpProcess.Cancel();
			m_cvVideo.notify_all();
			m_cvAudio.notify_all();
			m_cvAudioSpace.notify_all();
			if ( m_tVideo.joinable() ) { m_tVideo.join(); }
			if ( m_tAudio.joinable() ) { m_tAudio.join(); }
		}
	}

	// == Functions.
	/**
	 * Launches FFmpeg and the writer threads.
	 *
	 * \param _spProps The stream properties.
	 * \param _wsPath The movie file.
	 * \param _wsError Receives an error message on failure.
	 * \return Returns true if FFmpeg and the threads were started.
	 */
	bool CVideoExporter::CSession::Begin( const LSN_STREAM_PROPS &_spProps, const std::wstring &_wsPath, std::wstring &_wsError ) {
		try {
			spProps = _spProps;
			wsPath = _wsPath;
			wsLogPath = _wsPath + L".log";
			rInRate = InputRate( (*m_pveOwner->m_pvfdFormat), _spProps.rEmuRate );

			std::wstring wsVideoPipe, wsAudioPipe;
			if ( !m_fpProcess.CreatePipes( wsVideoPipe, wsAudioPipe ) ) {
				_wsError = LSN_LSTR( LSN_VE_PIPE_FAILED );
				return false;
			}

			const char * pcInFormat = CVideoFormat::FfmpegInputPixelFormat( _spProps.cfFormat );
			if ( !pcInFormat ) {
				_wsError = LSN_LSTR( LSN_VE_UNSUPPORTED_CAPTURE );
				return false;
			}

			std::vector<std::wstring> vArgs = {
				L"-hide_banner", L"-nostdin", L"-loglevel", L"error", L"-y",
				// Input 0:  raw video.  The small probe size keeps FFmpeg from waiting for lots of data before it opens the audio input.
				L"-f", L"rawvideo", L"-pix_fmt", Wide( pcInFormat ),
				L"-s", std::to_wstring( _spProps.ui32Width ) + L"x" + std::to_wstring( _spProps.ui32Height ),
				L"-framerate", Wide( RateStr( rInRate ).c_str() ),
				L"-probesize", L"32", L"-analyzeduration", L"0", L"-thread_queue_size", L"4",
				L"-i", wsVideoPipe,
				// Input 1:  32-bit float mono audio.  FFmpeg converts it to whatever the audio codec needs.
				L"-f", L"f32le", L"-ar", std::to_wstring( _spProps.ui32AudioHz ), L"-ac", L"1",
				L"-probesize", L"32", L"-analyzeduration", L"0", L"-thread_queue_size", L"1024",
				L"-i", wsAudioPipe,
			};
			if ( !BuildOutputArgs( (*m_pveOwner->m_pvfdFormat), m_pveOwner->m_foOptions, m_pveOwner->m_bInterpolate,
				_spProps.ui32Width, _spProps.ui32Height, rInRate, _wsPath, vArgs, _wsError ) ) { return false; }

			if ( !m_fpProcess.Launch( m_pveOwner->m_wsFfmpeg, vArgs, wsLogPath ) ) {
				_wsError = std::format( LSN_LSTR( LSN_VE_LAUNCH_FAILED ), m_pveOwner->m_wsFfmpeg );
				m_fpProcess.Reset();
				DeleteFile( wsLogPath );
				return false;
			}

			m_bStarted = true;
			m_tVideo = std::thread( VideoThread, this );
			m_tAudio = std::thread( AudioThread, this );
			return true;
		}
		catch ( ... ) {
			// Out of memory or the threads could not be created.
			_wsError = LSN_LSTR( LSN_OUT_OF_MEMORY );
			if ( m_bStarted ) {
				SetFailed( _wsError );
				m_fpProcess.Cancel();
			}
			return false;
		}
	}

	/**
	 * Sends the remaining data, closes the pipes, and waits for FFmpeg to finish the file.
	 *
	 * \param _wsError Receives an error message on failure.
	 * \return Returns true if FFmpeg finished the file without error.
	 */
	bool CVideoExporter::CSession::End( std::wstring &_wsError ) {
		if ( !m_bStarted ) { return true; }
		{
			std::lock_guard<std::mutex> lgLock( m_mMutex );
			m_bClosing = true;
		}
		m_cvVideo.notify_all();
		m_cvAudio.notify_all();
		m_cvAudioSpace.notify_all();
		// The threads send what is queued, then close their pipes, which tells FFmpeg that the inputs have ended.
		if ( m_tVideo.joinable() ) { m_tVideo.join(); }
		if ( m_tAudio.joinable() ) { m_tAudio.join(); }
		m_bStarted = false;

		int32_t i32Code = -1;
		bool bExited = m_fpProcess.WaitForExit( ui32FinishTimeoutMs, i32Code );
		if ( !bExited ) {
			m_fpProcess.Kill();
			m_fpProcess.WaitForExit( 5000, i32Code );
			m_fpProcess.Reset();
			_wsError = std::format( LSN_LSTR( LSN_VE_FFMPEG_TIMEOUT ), wsPath );
			return false;
		}
		m_fpProcess.Reset();

		if ( m_abFailed ) {
			_wsError = std::format( LSN_LSTR( LSN_VE_FFMPEG_STOPPED ), wsPath, i32Code, wsLogPath, LogTail( wsLogPath, 12 ) );
			return false;
		}
		if ( i32Code != 0 ) {
			_wsError = std::format( LSN_LSTR( LSN_VE_FFMPEG_ERROR ), wsPath, i32Code, wsLogPath, LogTail( wsLogPath, 12 ) );
			return false;
		}
		// Success.  The log only matters when something goes wrong.
		DeleteFile( wsLogPath );
		return true;
	}

	/**
	 * Queues a pool frame for the video thread.
	 *
	 * \param _sIdx The index of the pool frame.
	 */
	void CVideoExporter::CSession::QueueFrame( size_t _sIdx ) {
		bool bQueued = false;
		{
			std::lock_guard<std::mutex> lgLock( m_mMutex );
			if ( !m_bClosing ) {
				try {
					m_dFrames.push_back( _sIdx );
					bQueued = true;
				}
				catch ( ... ) {}
			}
		}
		if ( !bQueued ) {
			SetFailed( LSN_LSTR( LSN_OUT_OF_MEMORY ) );
			m_pveOwner->ReleaseFrame( _sIdx );
			return;
		}
		++ui64Frames;
		m_cvVideo.notify_one();
	}

	/**
	 * Queues audio samples for the audio thread, waiting first if too many are already queued.
	 *
	 * \param _pfSamples The samples.
	 * \param _sTotal The number of samples.
	 * \return Returns false if the session has failed or waits were aborted.
	 */
	bool CVideoExporter::CSession::QueueAudio( const float * _pfSamples, size_t _sTotal ) {
		if ( !_sTotal ) { return true; }
		std::unique_lock<std::mutex> ulLock( m_mMutex );
		while ( m_dAudio.size() >= LSN_MAX_AUDIO_BLOCKS && !m_abFailed && !m_bClosing && !m_pveOwner->m_abAbort ) {
			m_cvAudioSpace.wait_for( ulLock, std::chrono::milliseconds( 50 ) );
		}
		if ( m_abFailed || m_bClosing ) { return false; }
		try {
			m_dAudio.emplace_back( _pfSamples, _pfSamples + _sTotal );
		}
		catch ( ... ) {
			ulLock.unlock();
			SetFailed( LSN_LSTR( LSN_OUT_OF_MEMORY ) );
			return false;
		}
		ui64Samples += _sTotal;
		ulLock.unlock();
		m_cvAudio.notify_one();
		return true;
	}

	/**
	 * Gets the error message after a failure.
	 *
	 * \return Returns the error message.
	 */
	std::wstring CVideoExporter::CSession::Error() const {
		std::lock_guard<std::mutex> lgLock( m_mMutex );
		return m_wsError;
	}

	/**
	 * Wakes any wait in QueueAudio() (used when waits are aborted).
	 */
	void CVideoExporter::CSession::Wake() {
		m_cvAudioSpace.notify_all();
	}

	/**
	 * Records a failure.  The first message is kept.
	 *
	 * \param _wsError The error message.
	 */
	void CVideoExporter::CSession::SetFailed( const std::wstring &_wsError ) {
		{
			std::lock_guard<std::mutex> lgLock( m_mMutex );
			if ( m_wsError.empty() ) {
				try {
					m_wsError = _wsError;
				}
				catch ( ... ) {}
			}
		}
		m_abFailed = true;
		m_cvVideo.notify_all();
		m_cvAudio.notify_all();
		m_cvAudioSpace.notify_all();
	}

	/**
	 * The video thread.
	 *
	 * \param _psThis This object.
	 */
	void CVideoExporter::CSession::VideoThread( CSession * _psThis ) {
		CSession & sThis = (*_psThis);
		if ( !sThis.m_fpProcess.ConnectPipe( CFfmpegProcess::LSN_P_VIDEO ) ) {
			sThis.SetFailed( L"" );
		}
		LSN_CAPTURE_BUFFER cbScratch;
		while ( true ) {
			size_t sIdx;
			{
				std::unique_lock<std::mutex> ulLock( sThis.m_mMutex );
				sThis.m_cvVideo.wait( ulLock, [&]() { return !sThis.m_dFrames.empty() || sThis.m_bClosing; } );
				if ( sThis.m_dFrames.empty() ) { break; }
				sIdx = sThis.m_dFrames.front();
				sThis.m_dFrames.pop_front();
			}
			if ( !sThis.m_abFailed ) {
				const LSN_CAPTURE_BUFFER * pcbFrame = CVideoFormat::Convert( sThis.m_pveOwner->m_vfFormat, sThis.m_pveOwner->m_cbPool[sIdx], cbScratch );
				if ( !pcbFrame || (*pcbFrame).ui32Width != sThis.spProps.ui32Width || (*pcbFrame).ui32Height != sThis.spProps.ui32Height ||
					(*pcbFrame).cfFormat != sThis.spProps.cfFormat ) {
					sThis.SetFailed( LSN_LSTR( LSN_VE_UNSUPPORTED_CAPTURE ) );
				}
				else {
					size_t sRowSize = size_t( (*pcbFrame).ui32Width ) * LSN_CAPTURE_BUFFER::BytesPerPixel( (*pcbFrame).cfFormat );
					bool bOk = true;
					if ( (*pcbFrame).ui32Pitch == sRowSize ) {
						bOk = sThis.m_fpProcess.WritePipe( CFfmpegProcess::LSN_P_VIDEO, (*pcbFrame).vBuffer.data(), sRowSize * (*pcbFrame).ui32Height );
					}
					else {
						for ( uint32_t Y = 0; Y < (*pcbFrame).ui32Height && bOk; ++Y ) {
							bOk = sThis.m_fpProcess.WritePipe( CFfmpegProcess::LSN_P_VIDEO, &(*pcbFrame).vBuffer[size_t( Y )*(*pcbFrame).ui32Pitch], sRowSize );
						}
					}
					if ( !bOk ) { sThis.SetFailed( L"" ); }
				}
			}
			// Even after a failure, frames are returned to the pool so that the emulator never waits on them.
			sThis.m_pveOwner->ReleaseFrame( sIdx );
		}
		sThis.m_fpProcess.ClosePipe( CFfmpegProcess::LSN_P_VIDEO );
	}

	/**
	 * The audio thread.
	 *
	 * \param _psThis This object.
	 */
	void CVideoExporter::CSession::AudioThread( CSession * _psThis ) {
		CSession & sThis = (*_psThis);
		if ( !sThis.m_fpProcess.ConnectPipe( CFfmpegProcess::LSN_P_AUDIO ) ) {
			sThis.SetFailed( L"" );
		}
		std::vector<float> vBlock;
		while ( true ) {
			{
				std::unique_lock<std::mutex> ulLock( sThis.m_mMutex );
				sThis.m_cvAudio.wait( ulLock, [&]() { return !sThis.m_dAudio.empty() || sThis.m_bClosing; } );
				if ( sThis.m_dAudio.empty() ) { break; }
				vBlock.swap( sThis.m_dAudio.front() );
				sThis.m_dAudio.pop_front();
			}
			sThis.m_cvAudioSpace.notify_all();
			if ( !sThis.m_abFailed ) {
				if ( !sThis.m_fpProcess.WritePipe( CFfmpegProcess::LSN_P_AUDIO, vBlock.data(), vBlock.size() * sizeof( float ) ) ) {
					sThis.SetFailed( L"" );
				}
			}
		}
		sThis.m_fpProcess.ClosePipe( CFfmpegProcess::LSN_P_AUDIO );
	}


	// == CVideoExporter.
	CVideoExporter::CVideoExporter() :
		m_abFailed( false ),
		m_abAbort( false ) {
	}
	CVideoExporter::~CVideoExporter() {
		std::wstring wsError;
		Stop( wsError );
	}

	// == Functions.
	/**
	 * Gets the frame rate of a region.
	 *
	 * \param _pmRegion The region.
	 * \return Returns the exact frame rate of the region (NTSC for unknown regions).
	 */
	CVideoExporter::LSN_RATE CVideoExporter::RegionFrameRate( LSN_PPU_METRICS _pmRegion ) {
		switch ( _pmRegion ) {
			case LSN_PM_PAL : {}				LSN_FALLTHROUGH
			case LSN_PM_DENDY : { return LSN_RATE{ 322445, 6448 }; }
			case LSN_PM_PALM : { return LSN_RATE{ 383484375, 6387953 }; }
			case LSN_PM_PALN : { return LSN_RATE{ 14328225, 283712 }; }
			default : { return LSN_RATE{ 39375000, 655171 }; }
		}
	}

	/**
	 * Prepares to export a movie:  finds FFmpeg, checks that it supports the selected format, and runs a short test encode with the
	 *	selected settings.  FFmpeg is launched for real when the first frame arrives.  Call only while the emulation thread is stopped.
	 *
	 * \param _veoOptions The video-export options.
	 * \param _rEmuRate The emulation frame rate.
	 * \param _ui32AudioHz The audio rate.
	 * \param _pfNewPath The function that generates the path of each new movie file.
	 * \param _pvParm The parameter to pass to _pfNewPath.
	 * \param _wsError Receives an error message on failure.
	 * \return Returns true if exporting has started.
	 */
	bool CVideoExporter::Start( const LSN_VIDEO_EXPORT_OPTIONS &_veoOptions, LSN_RATE _rEmuRate, uint32_t _ui32AudioHz,
		PfNewPath _pfNewPath, void * _pvParm, std::wstring &_wsError ) {
		if ( m_bActive ) {
			std::wstring wsTmp;
			Stop( wsTmp );
		}
		_wsError.clear();
		m_abFailed = false;
		m_abAbort = false;
		m_wsError.clear();
		m_vMovies.clear();

		m_vfFormat = _veoOptions.vfFormat;
		m_pvfdFormat = CVideoFormat::Data( m_vfFormat );
		if ( !m_pvfdFormat || !_pfNewPath ) {
			_wsError = LSN_LSTR( LSN_VE_INVALID_FORMAT );
			return false;
		}
		m_foOptions = _veoOptions.foFormat[m_vfFormat];
		m_foOptions.Validate( m_vfFormat );
		m_bInterpolate = _veoOptions.bInterpolateFrames;
		m_pfNewPath = _pfNewPath;
		m_pvNewPathParm = _pvParm;
		m_rLastRate = _rEmuRate;
		m_ui32LastAudioHz = _ui32AudioHz;

		if ( !CheckFfmpeg( _rEmuRate, _ui32AudioHz, _wsError ) ) { return false; }

		try {
			m_vFree.clear();
			m_dIncoming.clear();
			for ( size_t I = LSN_POOL_SIZE; I--; ) {
				m_vFree.push_back( I );
			}
			m_vStage.clear();
			m_vStage.reserve( 1 << 16 );
		}
		catch ( ... ) {
			_wsError = LSN_LSTR( LSN_OUT_OF_MEMORY );
			return false;
		}
		m_ui64SamplesThisFrame = 0;
		m_bActive = true;
		return true;
	}

	/**
	 * Stops exporting:  sends any frames and samples that are still waiting, then lets FFmpeg finish the file.  Call only while the
	 *	emulation thread is stopped.  Does nothing if not exporting.
	 *
	 * \param _wsError Receives an error message on failure (including any earlier failure that ended the export).
	 * \return Returns true if every movie was finished without error.
	 */
	bool CVideoExporter::Stop( std::wstring &_wsError ) {
		_wsError.clear();
		if ( !m_bActive ) { return true; }
		m_abAbort = false;

		// Frames submitted after the last frame boundary still belong in the movie.
		if ( !m_abFailed ) {
			std::deque<size_t> dIncoming;
			{
				std::lock_guard<std::mutex> lgLock( m_mPool );
				dIncoming.swap( m_dIncoming );
			}
			for ( size_t I = 0; I < dIncoming.size(); ++I ) {
				if ( m_abFailed ) {
					ReleaseFrame( dIncoming[I] );
					continue;
				}
				ProcessFrame( dIncoming[I], m_rLastRate, m_ui32LastAudioHz );
			}
			if ( !m_abFailed ) { CommitAudio(); }
		}
		EndSession();

		{
			std::lock_guard<std::mutex> lgLock( m_mPool );
			m_dIncoming.clear();
			m_vFree.clear();
		}
		for ( size_t I = 0; I < LSN_POOL_SIZE; ++I ) {
			m_cbPool[I].Reset();
		}
		m_cbConverted.Reset();
		m_vStage = std::vector<float>();
		m_bActive = false;
		_wsError = m_wsError;
		bool bRet = !m_abFailed;
		m_abFailed = false;
		return bRet;
	}

	/**
	 * Makes every wait return immediately (true) or restores normal waiting (false).  Set while stopping the emulation thread so that it
	 *	cannot remain blocked waiting for FFmpeg.
	 *
	 * \param _bAbort Whether to abort waits.
	 */
	void CVideoExporter::SetAbortWaits( bool _bAbort ) {
		m_abAbort = _bAbort;
		if ( _bAbort ) {
			m_cvPool.notify_all();
			if ( m_psSession ) { m_psSession->Wake(); }
		}
	}

	/**
	 * Adds a rendered frame.  Called on the rendering thread after each rendered frame has been captured.  The frame is copied.
	 *
	 * \param _cbFrame The captured frame.
	 */
	void CVideoExporter::SubmitFrame( const LSN_CAPTURE_BUFFER &_cbFrame ) {
		if ( !m_bActive || m_abFailed || !_cbFrame.Valid() ) { return; }
		size_t sIdx;
		{
			std::unique_lock<std::mutex> ulLock( m_mPool );
			// FrameBoundary() makes sure that a frame is free before each frame is rendered, so this does not normally wait.
			while ( m_vFree.empty() && !m_abFailed && !m_abAbort ) {
				m_cvPool.wait_for( ulLock, std::chrono::milliseconds( 50 ) );
			}
			if ( m_vFree.empty() ) { return; }
			sIdx = m_vFree.back();
			m_vFree.pop_back();
		}
		try {
			m_cbPool[sIdx] = _cbFrame;
		}
		catch ( ... ) {
			ReleaseFrame( sIdx );
			Fail( LSN_LSTR( LSN_OUT_OF_MEMORY ) );
			return;
		}
		std::lock_guard<std::mutex> lgLock( m_mPool );
		try {
			m_dIncoming.push_back( sIdx );
		}
		catch ( ... ) {
			m_vFree.push_back( sIdx );		// Cannot fail:  the vector's capacity already holds every pool frame.
			Fail( LSN_LSTR( LSN_OUT_OF_MEMORY ) );
		}
	}

	/**
	 * Receives each audio sample.  Called on the emulation thread for each sample the APU outputs (set as the APU's sample-out function).
	 *
	 * \param _pvThis A pointer to this object.
	 * \param _fSample The sample.
	 */
	void CVideoExporter::AddSample( void * _pvThis, float _fSample ) {
		CVideoExporter * pveThis = static_cast<CVideoExporter *>(_pvThis);
		try {
			(*pveThis).m_vStage.push_back( _fSample );
			++(*pveThis).m_ui64SamplesThisFrame;
		}
		catch ( ... ) {
			(*pveThis).Fail( LSN_LSTR( LSN_OUT_OF_MEMORY ) );
		}
	}

	/**
	 * Called on the emulation thread at the end of each emulated frame, before the frame is handed to the renderer.  Sends the frames
	 *	that have been submitted since the last call to FFmpeg (restarting it if anything about the stream changed), sends the matching
	 *	audio, and waits until there is room for the next frame.
	 *
	 * \param _rEmuRate The emulation frame rate.
	 * \param _ui32AudioHz The current audio rate.
	 * \return Returns false if the export has failed.
	 */
	bool CVideoExporter::FrameBoundary( LSN_RATE _rEmuRate, uint32_t _ui32AudioHz ) {
		if ( !m_bActive ) { return true; }
		if ( m_abFailed ) { return false; }
		if ( m_psSession && m_psSession->Failed() ) {
			// FFmpeg stopped.  Finish the session now so that the error message (with FFmpeg's log) is available.
			EndSession();
			return false;
		}

		std::deque<size_t> dIncoming;
		{
			std::lock_guard<std::mutex> lgLock( m_mPool );
			dIncoming.swap( m_dIncoming );
		}
		m_rLastRate = _rEmuRate;
		m_ui32LastAudioHz = _ui32AudioHz;
		for ( size_t I = 0; I < dIncoming.size(); ++I ) {
			if ( m_abFailed ) {
				ReleaseFrame( dIncoming[I] );
				continue;
			}
			ProcessFrame( dIncoming[I], _rEmuRate, _ui32AudioHz );			// Releases the frame itself on failure.
		}
		if ( m_abFailed || !CommitAudio() ) { return false; }

		// Make sure the next rendered frame will have a place to go.  If FFmpeg is behind, this is where the emulator slows down.
		std::unique_lock<std::mutex> ulLock( m_mPool );
		while ( m_vFree.empty() && !m_abFailed && !m_abAbort && !(m_psSession && m_psSession->Failed()) ) {
			m_cvPool.wait_for( ulLock, std::chrono::milliseconds( 50 ) );
		}
		return !m_abFailed;
	}

	/**
	 * Gets the path of the movie currently being written.
	 *
	 * \return Returns the path of the movie currently being written, or an empty string.
	 */
	std::wstring CVideoExporter::CurrentPath() const {
		return m_psSession ? m_psSession->wsPath : std::wstring();
	}

	/**
	 * Gets the input frame rate given to FFmpeg for a format:  the emulation rate, or the closest rate the format can store if it cannot
	 *	store the exact rate.
	 *
	 * \param _vfdFormat The video format.
	 * \param _rEmuRate The emulation frame rate.
	 * \return Returns the input frame rate.
	 */
	CVideoExporter::LSN_RATE CVideoExporter::InputRate( const LSN_VIDEO_FORMAT_DATA &_vfdFormat, LSN_RATE _rEmuRate ) {
		if ( !_vfdFormat.bAllowsFractions && _rEmuRate.ui32Den ) {
			return LSN_RATE{ std::max<uint32_t>( 1, uint32_t( std::round( _rEmuRate.Value() ) ) ), 1 };
		}
		// The codec's time base is the inverse of the frame rate, so its denominator is the frame rate's numerator.
		if ( _vfdFormat.ui32MaxTimeBaseDen && _rEmuRate.ui32Num > _vfdFormat.ui32MaxTimeBaseDen ) {
			return LimitNumerator( _rEmuRate, _vfdFormat.ui32MaxTimeBaseDen );
		}
		return _rEmuRate;
	}

	/**
	 * Converts a frame rate in frames per second to a fraction.  Common NTSC-style rates (such as 59.94) become exact fractions (60000/1001).
	 *
	 * \param _dRate The rate.
	 * \param _ui32MaxNum The largest allowed numerator, or 0 for no limit.
	 * \return Returns the fraction.
	 */
	CVideoExporter::LSN_RATE CVideoExporter::ToRate( double _dRate, uint32_t _ui32MaxNum ) {
		LSN_RATE rRet;
		if ( !(_dRate > 0.0) || _dRate > 100000.0 ) { return rRet; }
		double dWhole = std::round( _dRate );
		double dNtsc = std::round( _dRate * 1.001 );
		if ( std::fabs( _dRate - dWhole ) < 1.0e-6 ) {
			rRet = LSN_RATE{ uint32_t( dWhole ), 1 };
		}
		else if ( std::fabs( _dRate - dNtsc * 1000.0 / 1001.0 ) < 0.001 ) {
			rRet = LSN_RATE{ uint32_t( dNtsc * 1000.0 ), 1001 };
		}
		else {
			uint32_t ui32Den = 10000;
			uint32_t ui32Num = uint32_t( std::round( _dRate * ui32Den ) );
			uint32_t ui32Gcd = std::gcd( ui32Num, ui32Den );
			rRet = LSN_RATE{ ui32Num / ui32Gcd, ui32Den / ui32Gcd };
		}
		if ( _ui32MaxNum && rRet.ui32Num > _ui32MaxNum ) { rRet = LimitNumerator( rRet, _ui32MaxNum ); }
		return rRet;
	}

	/**
	 * Finds the fraction closest to _rRate whose numerator does not exceed _ui32MaxNum.
	 *
	 * \param _rRate The rate to approximate.
	 * \param _ui32MaxNum The largest allowed numerator.
	 * \return Returns the closest fraction.
	 */
	CVideoExporter::LSN_RATE CVideoExporter::LimitNumerator( LSN_RATE _rRate, uint32_t _ui32MaxNum ) {
		if ( !_rRate.ui32Num || !_rRate.ui32Den || _rRate.ui32Num <= _ui32MaxNum || !_ui32MaxNum ) { return _rRate; }
		// Approximate the inverse (den/num) with a denominator no larger than _ui32MaxNum, using continued fractions.
		uint64_t ui64N = _rRate.ui32Den, ui64D = _rRate.ui32Num;
		uint64_t ui64P0 = 0, ui64Q0 = 1, ui64P1 = 1, ui64Q1 = 0;
		while ( ui64D ) {
			uint64_t ui64A = ui64N / ui64D;
			uint64_t ui64Q2 = ui64Q0 + ui64A * ui64Q1;
			if ( ui64Q2 > _ui32MaxNum ) { break; }
			uint64_t ui64P2 = ui64P0 + ui64A * ui64P1;
			ui64P0 = ui64P1;
			ui64Q0 = ui64Q1;
			ui64P1 = ui64P2;
			ui64Q1 = ui64Q2;
			uint64_t ui64Tmp = ui64N - ui64A * ui64D;
			ui64N = ui64D;
			ui64D = ui64Tmp;
		}
		// The 2 best candidates.
		uint64_t ui64K = ui64Q1 ? (_ui32MaxNum - ui64Q0) / ui64Q1 : 0;
		uint64_t ui64BoundP = ui64P0 + ui64K * ui64P1, ui64BoundQ = ui64Q0 + ui64K * ui64Q1;
		double dTarget = double( _rRate.ui32Den ) / _rRate.ui32Num;
		double dErr1 = ui64BoundQ ? std::fabs( double( ui64BoundP ) / ui64BoundQ - dTarget ) : 1.0e300;
		double dErr2 = ui64Q1 ? std::fabs( double( ui64P1 ) / ui64Q1 - dTarget ) : 1.0e300;
		uint64_t ui64P = dErr2 <= dErr1 ? ui64P1 : ui64BoundP;
		uint64_t ui64Q = dErr2 <= dErr1 ? ui64Q1 : ui64BoundQ;
		if ( !ui64P || !ui64Q || ui64P > UINT32_MAX || ui64Q > UINT32_MAX ) { return _rRate; }
		// ui64P/ui64Q approximates den/num, so the rate is ui64Q/ui64P.
		return LSN_RATE{ uint32_t( ui64Q ), uint32_t( ui64P ) };
	}

	/**
	 * Builds the FFmpeg arguments that describe the output (filters, encoders, and the file).
	 *
	 * \param _vfdFormat The video format.
	 * \param _foOptions The settings for the format.
	 * \param _bInterpolate If true, frames are blended when the output frame rate differs from the input frame rate.
	 * \param _ui32Width The width of the input frames.
	 * \param _ui32Height The height of the input frames.
	 * \param _rInRate The input frame rate.
	 * \param _wsOutPath The output file.
	 * \param _vArgs Receives the arguments (appended).
	 * \param _wsError Receives an error message on failure.
	 * \return Returns true if the arguments were built.
	 */
	bool CVideoExporter::BuildOutputArgs( const LSN_VIDEO_FORMAT_DATA &_vfdFormat, const LSN_VIDEO_EXPORT_OPTIONS::LSN_FORMAT_OPTIONS &_foOptions,
		bool _bInterpolate, uint32_t _ui32Width, uint32_t _ui32Height, LSN_RATE _rInRate, const std::wstring &_wsOutPath,
		std::vector<std::wstring> &_vArgs, std::wstring &_wsError ) {
		const LSN_VIDEO_PIXEL_FORMAT_DATA * pvpfdPixel = CVideoFormat::PixelFormatData( _foOptions.vpfPixelFormat );
		if ( !pvpfdPixel ) { pvpfdPixel = CVideoFormat::PixelFormatData( _vfdFormat.vpfBest ); }
		const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdAudio = CVideoFormat::AudioCodecData( _foOptions.vacAudioCodec );
		if ( !pvacdAudio ) { pvacdAudio = CVideoFormat::AudioCodecData( _vfdFormat.vacBest ); }
		if ( !pvpfdPixel || !pvacdAudio ) {
			_wsError = LSN_LSTR( LSN_VE_INVALID_FORMAT );
			return false;
		}

		// Chroma subsampling needs even sizes:  crop the odd row/column.
		uint32_t ui32W = _ui32Width & ~((1U << (*pvpfdPixel).ui8ChromaShiftX) - 1U);
		uint32_t ui32H = _ui32Height & ~((1U << (*pvpfdPixel).ui8ChromaShiftY) - 1U);
		if ( ui32W < 16 || ui32H < 16 ) {
			_wsError = std::format( LSN_LSTR( LSN_VE_FRAME_TOO_SMALL ), _ui32Width, _ui32Height );
			return false;
		}
		if ( ui32W > _vfdFormat.ui32MaxWidth || ui32H > _vfdFormat.ui32MaxHeight ) {
			_wsError = std::format( LSN_LSTR( LSN_VE_FRAME_TOO_LARGE ), _ui32Width, _ui32Height, CVideoFormat::ToStrWStr( _vfdFormat.vfType ),
				_vfdFormat.ui32MaxWidth, _vfdFormat.ui32MaxHeight );
			return false;
		}

		LSN_RATE rOutRate = _foOptions.dFrameRate > 0.0 ? ToRate( _foOptions.dFrameRate, _vfdFormat.ui32MaxTimeBaseDen ) : _rInRate;
		if ( !rOutRate.ui32Num ) { rOutRate = _rInRate; }
		bool bRateChange = rOutRate != _rInRate;
		std::string sOutRate = RateStr( rOutRate );
		// The "framerate" filter derives its output time base from its input time base and the output rate, which overflows with exact NES
		//	rates (such as 655171/39375000) and stalls the filter.  Re-timing the frames to a time base made from the output rate avoids that.
		uint64_t ui64Ticks = uint64_t( rOutRate.ui32Num ) * std::clamp<uint64_t>( 0x7FFFFFFFULL / std::max<uint32_t>( rOutRate.ui32Num, 1 ), 1, 1000 );
		std::string sSetTb = "settb=1/" + std::to_string( ui64Ticks );
		std::string sOutFormat = (*pvpfdPixel).pcFfmpegName;

		// The filters.  RGB-to-YUV conversions use BT.709 (and the output is tagged as such) so that every player shows the same colors.
		std::vector<std::string> vFilters;
		if ( ui32W != _ui32Width || ui32H != _ui32Height ) {
			vFilters.push_back( "crop=" + std::to_string( ui32W ) + ":" + std::to_string( ui32H ) + ":0:0" );
		}
		if ( bRateChange && _bInterpolate ) {
			// FFmpeg's "framerate" filter blends frames but works only in planar YUV.
			if ( (*pvpfdPixel).bRgb ) {
				vFilters.push_back( "scale=out_color_matrix=bt709:out_range=pc" );
				vFilters.push_back( "format=yuv444p12le" );
				vFilters.push_back( sSetTb );
				vFilters.push_back( "framerate=fps=" + sOutRate );
				vFilters.push_back( "scale=in_color_matrix=bt709:in_range=pc" );
				vFilters.push_back( "format=" + sOutFormat );
			}
			else {
				std::string sBlendFormat = sOutFormat;
				if ( _foOptions.vpfPixelFormat == LSN_VPF_NV12 ) { sBlendFormat = "yuv420p"; }
				else if ( _foOptions.vpfPixelFormat == LSN_VPF_P010 ) { sBlendFormat = "yuv420p10le"; }
				else if ( _foOptions.vpfPixelFormat == LSN_VPF_YUV444P16 ) { sBlendFormat = "yuv444p12le"; }
				vFilters.push_back( "scale=out_color_matrix=bt709:out_range=tv" );
				vFilters.push_back( "format=" + sBlendFormat );
				vFilters.push_back( sSetTb );
				vFilters.push_back( "framerate=fps=" + sOutRate );
				if ( sBlendFormat != sOutFormat ) { vFilters.push_back( "format=" + sOutFormat ); }
			}
		}
		else {
			if ( !(*pvpfdPixel).bRgb ) { vFilters.push_back( "scale=out_color_matrix=bt709:out_range=tv" ); }
			vFilters.push_back( "format=" + sOutFormat );
			// Without blending, frames are dropped or repeated to reach the output rate.
			if ( bRateChange ) { vFilters.push_back( "fps=fps=" + sOutRate ); }
		}
		std::string sFilter;
		for ( size_t I = 0; I < vFilters.size(); ++I ) {
			if ( I ) { sFilter += ','; }
			sFilter += vFilters[I];
		}

		_vArgs.push_back( L"-map" );
		_vArgs.push_back( L"0:v:0" );
		_vArgs.push_back( L"-map" );
		_vArgs.push_back( L"1:a:0" );
		_vArgs.push_back( L"-vf" );
		_vArgs.push_back( Wide( sFilter.c_str() ) );

		// The video encoder.
		_vArgs.push_back( L"-c:v" );
		_vArgs.push_back( Wide( _vfdFormat.pcEncoder ) );
		_vArgs.push_back( L"-pix_fmt" );
		_vArgs.push_back( Wide( sOutFormat.c_str() ) );
		if ( _vfdFormat.pvpPresets && _foOptions.ui32Preset < _vfdFormat.ui32TotalPresets ) {
			AppendArgs( _vfdFormat.pvpPresets[_foOptions.ui32Preset].pcArgs, nullptr, _vArgs );
		}
		if ( !_vfdFormat.bAlwaysLossless ) {
			if ( _foOptions.bLossless && _vfdFormat.pcLosslessArgs ) {
				AppendArgs( _vfdFormat.pcLosslessArgs, nullptr, _vArgs );
			}
			else {
				bool bAlt = _foOptions.bUseAltQuality && _vfdFormat.vqdAltQuality.vqmMode != LSN_VQM_NONE;
				const LSN_VIDEO_QUALITY_DATA & vqdQuality = bAlt ? _vfdFormat.vqdAltQuality : _vfdFormat.vqdQuality;
				int32_t i32Value = std::clamp( bAlt ? _foOptions.i32AltQuality : _foOptions.i32Quality, vqdQuality.i32Min, vqdQuality.i32Max );
				AppendArgs( vqdQuality.pcArgs, std::to_string( i32Value ).c_str(), _vArgs );
			}
		}
		AppendArgs( _vfdFormat.pcExtraArgs, nullptr, _vArgs );
		if ( !(*pvpfdPixel).bRgb ) {
			AppendArgs( "-colorspace bt709 -color_primaries bt709 -color_trc bt709 -color_range tv", nullptr, _vArgs );
		}

		// The audio encoder.
		_vArgs.push_back( L"-c:a" );
		_vArgs.push_back( Wide( (*pvacdAudio).pcEncoder ) );
		if ( !(*pvacdAudio).bLossless ) {
			uint32_t ui32Bitrate = std::clamp( _foOptions.ui32AudioBitrate, (*pvacdAudio).ui32MinBitrate, (*pvacdAudio).ui32MaxBitrate );
			_vArgs.push_back( L"-b:a" );
			_vArgs.push_back( std::to_wstring( ui32Bitrate ) + L"k" );
		}

		_vArgs.push_back( L"-f" );
		_vArgs.push_back( Wide( _vfdFormat.pcMuxer ) );
		_vArgs.push_back( _wsOutPath );
		return true;
	}

	/**
	 * Records a failure.  The first message is kept.
	 *
	 * \param _wsError The error message.
	 */
	void CVideoExporter::Fail( const std::wstring &_wsError ) {
		if ( m_wsError.empty() ) {
			try {
				m_wsError = _wsError;
			}
			catch ( ... ) {}
		}
		m_abFailed = true;
		m_cvPool.notify_all();
	}

	/**
	 * Returns a pool frame to the free list.
	 *
	 * \param _sIdx The index of the pool frame.
	 */
	void CVideoExporter::ReleaseFrame( size_t _sIdx ) {
		{
			std::lock_guard<std::mutex> lgLock( m_mPool );
			m_vFree.push_back( _sIdx );			// Cannot throw:  Start() reserved room for every pool frame.
		}
		m_cvPool.notify_all();
	}

	/**
	 * Sends a submitted frame to the current session, starting a new session first if there is none or if the stream properties changed.
	 *
	 * \param _sIdx The index of the pool frame.
	 * \param _rEmuRate The emulation frame rate.
	 * \param _ui32AudioHz The audio rate.
	 * \return Returns false on failure.
	 */
	bool CVideoExporter::ProcessFrame( size_t _sIdx, LSN_RATE _rEmuRate, uint32_t _ui32AudioHz ) {
		// The properties of the frame as FFmpeg will see it (after conversion).
		const LSN_CAPTURE_BUFFER * pcbConverted = CVideoFormat::Convert( m_vfFormat, m_cbPool[_sIdx], m_cbConverted );
		if ( !pcbConverted || !CVideoFormat::FfmpegInputPixelFormat( (*pcbConverted).cfFormat ) ) {
			ReleaseFrame( _sIdx );
			Fail( LSN_LSTR( LSN_VE_UNSUPPORTED_CAPTURE ) );
			return false;
		}
		LSN_STREAM_PROPS spProps;
		spProps.ui32Width = (*pcbConverted).ui32Width;
		spProps.ui32Height = (*pcbConverted).ui32Height;
		spProps.cfFormat = (*pcbConverted).cfFormat;
		spProps.rEmuRate = _rEmuRate;
		spProps.ui32AudioHz = _ui32AudioHz;

		if ( m_psSession && m_psSession->Failed() ) {
			ReleaseFrame( _sIdx );
			EndSession();
			return false;
		}
		if ( !m_psSession || !(m_psSession->spProps == spProps) ) {
			// Something changed (or this is the first frame):  finish the current movie and start a new one.
			if ( m_psSession ) {
				bool bSameAudio = m_psSession->spProps.ui32AudioHz == spProps.ui32AudioHz;
				if ( !CommitAudio() || m_abFailed ) {
					ReleaseFrame( _sIdx );
					return false;
				}
				if ( !EndSession() ) {
					ReleaseFrame( _sIdx );
					return false;
				}
				// Samples beyond the end of the old movie continue in the new one, unless they are at a different rate.
				if ( !bSameAudio ) { m_vStage.clear(); }
			}
			std::wstring wsPath = m_pfNewPath( m_pvNewPathParm );
			if ( wsPath.empty() ) {
				ReleaseFrame( _sIdx );
				Fail( LSN_LSTR( LSN_VE_MOVIES_FOLDER_FAILED ) );
				return false;
			}
			std::wstring wsError;
			try {
				m_psSession = std::make_unique<CSession>( this );
			}
			catch ( ... ) {
				ReleaseFrame( _sIdx );
				Fail( LSN_LSTR( LSN_OUT_OF_MEMORY ) );
				return false;
			}
			if ( !m_psSession->Begin( spProps, wsPath, wsError ) ) {
				m_psSession.reset();
				ReleaseFrame( _sIdx );
				Fail( wsError );
				return false;
			}
			try {
				m_vMovies.push_back( wsPath );
			}
			catch ( ... ) {}
		}
		m_psSession->QueueFrame( _sIdx );
		return true;
	}

	/**
	 * Sends staged audio to the current session:  as many samples as match the frames sent so far.  If no samples were produced during
	 *	the frame (audio is disabled), silence is sent instead so that the audio keeps pace with the video.
	 *
	 * \return Returns false on failure.
	 */
	bool CVideoExporter::CommitAudio() {
		bool bNoSamples = m_ui64SamplesThisFrame == 0;
		m_ui64SamplesThisFrame = 0;
		if ( !m_psSession ) { return !m_abFailed; }
		CSession & sSession = (*m_psSession);
		uint64_t ui64Target = SamplesForFrames( sSession.ui64Frames, sSession.rInRate, sSession.spProps.ui32AudioHz );
		if ( ui64Target <= sSession.ui64Samples ) { return !m_abFailed; }
		size_t sTotal = size_t( std::min<uint64_t>( m_vStage.size(), ui64Target - sSession.ui64Samples ) );
		if ( sTotal ) {
			if ( !sSession.QueueAudio( m_vStage.data(), sTotal ) ) { return QueueFailed(); }
			m_vStage.erase( m_vStage.begin(), m_vStage.begin() + sTotal );
		}
		if ( bNoSamples && m_vStage.empty() && sSession.ui64Samples < ui64Target ) {
			// The APU produced nothing this frame (audio disabled).  Keep the audio track the same length as the video.
			try {
				std::vector<float> vSilence( size_t( ui64Target - sSession.ui64Samples ), 0.0f );
				if ( !sSession.QueueAudio( vSilence.data(), vSilence.size() ) ) { return QueueFailed(); }
			}
			catch ( ... ) {
				Fail( LSN_LSTR( LSN_OUT_OF_MEMORY ) );
				return false;
			}
		}
		return !m_abFailed;
	}

	/**
	 * Handles a failed CSession::QueueAudio():  if the session failed it is ended (recording the error); if waits were aborted the samples
	 *	simply stay staged.
	 *
	 * \return Returns false if the export has failed.
	 */
	bool CVideoExporter::QueueFailed() {
		if ( m_psSession && m_psSession->Failed() ) { EndSession(); }
		return !m_abFailed;
	}

	/**
	 * Finishes the current session (if any).
	 *
	 * \return Returns false if the session failed.
	 */
	bool CVideoExporter::EndSession() {
		if ( !m_psSession ) { return true; }
		std::wstring wsError;
		bool bRet = m_psSession->End( wsError );
		m_psSession.reset();
		if ( !bRet ) { Fail( wsError ); }
		return bRet;
	}

	/**
	 * Finds FFmpeg and verifies that it supports the selected format, then runs a short test encode.
	 *
	 * \param _rEmuRate The emulation frame rate.
	 * \param _ui32AudioHz The audio rate.
	 * \param _wsError Receives an error message on failure.
	 * \return Returns true if FFmpeg can make the movie.
	 */
	bool CVideoExporter::CheckFfmpeg( LSN_RATE _rEmuRate, uint32_t _ui32AudioHz, std::wstring &_wsError ) {
		try {
			m_wsFfmpeg = CFfmpegProcess::FindFfmpeg();
			if ( m_wsFfmpeg.empty() ) {
				_wsError = LSN_LSTR( LSN_VE_FFMPEG_NOT_FOUND );
				return false;
			}

			std::string sOutput;
			int32_t i32Code;
			// Encoders.
			if ( !CFfmpegProcess::RunAndCapture( m_wsFfmpeg, { L"-hide_banner", L"-encoders" }, ui32CheckTimeoutMs, sOutput, i32Code ) || i32Code != 0 ) {
				_wsError = std::format( LSN_LSTR( LSN_VE_FFMPEG_FAILED_TO_RUN ), m_wsFfmpeg );
				return false;
			}
			std::set<std::string> sEncoders = ParseList( sOutput );
			const LSN_VIDEO_AUDIO_CODEC_DATA * pvacdAudio = CVideoFormat::AudioCodecData( m_foOptions.vacAudioCodec );
			for ( const char * pcEncoder : { (*m_pvfdFormat).pcEncoder, pvacdAudio ? (*pvacdAudio).pcEncoder : "" } ) {
				if ( !sEncoders.count( pcEncoder ) ) {
					_wsError = std::format( LSN_LSTR( LSN_VE_MISSING_ENCODER ), m_wsFfmpeg, Wide( pcEncoder ), CVideoFormat::ToStrWStr( m_vfFormat ) );
					return false;
				}
			}

			// Muxer.
			if ( !CFfmpegProcess::RunAndCapture( m_wsFfmpeg, { L"-hide_banner", L"-muxers" }, ui32CheckTimeoutMs, sOutput, i32Code ) || i32Code != 0 ) {
				_wsError = std::format( LSN_LSTR( LSN_VE_FFMPEG_FAILED_TO_RUN ), m_wsFfmpeg );
				return false;
			}
			if ( !ParseList( sOutput ).count( (*m_pvfdFormat).pcMuxer ) ) {
				_wsError = std::format( LSN_LSTR( LSN_VE_MISSING_MUXER ), m_wsFfmpeg, Wide( (*m_pvfdFormat).pcMuxer ) );
				return false;
			}

			// Filters.
			if ( !CFfmpegProcess::RunAndCapture( m_wsFfmpeg, { L"-hide_banner", L"-filters" }, ui32CheckTimeoutMs, sOutput, i32Code ) || i32Code != 0 ) {
				_wsError = std::format( LSN_LSTR( LSN_VE_FFMPEG_FAILED_TO_RUN ), m_wsFfmpeg );
				return false;
			}
			std::set<std::string> sFilters = ParseList( sOutput );
			for ( const char * pcFilter : { "scale", "format", "crop", "fps", "framerate", "settb" } ) {
				if ( !sFilters.count( pcFilter ) ) {
					_wsError = std::format( LSN_LSTR( LSN_VE_MISSING_FILTER ), m_wsFfmpeg, Wide( pcFilter ) );
					return false;
				}
			}

			// A short test encode with the real settings catches everything else (such as a hardware encoder without the hardware).
			std::wstring wsMovie = m_pfNewPath( m_pvNewPathParm );
			if ( wsMovie.empty() ) {
				_wsError = LSN_LSTR( LSN_VE_MOVIES_FOLDER_FAILED );
				return false;
			}
			std::wstring wsProbe = wsMovie + L".test";
			LSN_RATE rInRate = InputRate( (*m_pvfdFormat), _rEmuRate );
			std::vector<std::wstring> vArgs = {
				L"-hide_banner", L"-nostdin", L"-loglevel", L"error", L"-y",
				L"-f", L"lavfi", L"-i", L"color=c=black:s=320x240:r=" + Wide( RateStr( rInRate ).c_str() ) + L",format=bgr0",
				L"-f", L"lavfi", L"-i", L"anullsrc=r=" + std::to_wstring( _ui32AudioHz ) + L":cl=mono",
				L"-t", L"0.2",
			};
			if ( !BuildOutputArgs( (*m_pvfdFormat), m_foOptions, m_bInterpolate, 320, 240, rInRate, wsProbe, vArgs, _wsError ) ) { return false; }
			bool bRan = CFfmpegProcess::RunAndCapture( m_wsFfmpeg, vArgs, ui32CheckTimeoutMs, sOutput, i32Code );
			DeleteFile( wsProbe );
			if ( !bRan || i32Code != 0 ) {
				_wsError = std::format( LSN_LSTR( LSN_VE_TEST_FAILED ), CVideoFormat::ToStrWStr( m_vfFormat ), TextTail( sOutput, 12 ) );
				return false;
			}
			return true;
		}
		catch ( ... ) {
			_wsError = LSN_LSTR( LSN_OUT_OF_MEMORY );
			return false;
		}
	}

	/**
	 * Gets the number of audio samples that match a number of frames.
	 *
	 * \param _ui64Frames The number of frames.
	 * \param _rRate The frame rate.
	 * \param _ui32AudioHz The audio rate.
	 * \return Returns the number of audio samples that span the given number of frames.
	 */
	uint64_t CVideoExporter::SamplesForFrames( uint64_t _ui64Frames, LSN_RATE _rRate, uint32_t _ui32AudioHz ) {
		if ( !_rRate.ui32Num ) { return 0; }
		// floor( frames * Hz * den / num ) without overflowing 64 bits.  With frames = Q * num + R:
		//	Q * Hz * den + floor( R * Hz * den / num ), and with R * Hz = A * num + B:  floor( R * Hz * den / num ) = A * den + floor( B * den / num ).
		const uint64_t ui64Num = _rRate.ui32Num, ui64Den = _rRate.ui32Den;
		uint64_t ui64Q = _ui64Frames / ui64Num;
		uint64_t ui64R = _ui64Frames % ui64Num;
		uint64_t ui64X = ui64R * _ui32AudioHz;												// < 2^32 * 2^32.
		uint64_t ui64A = ui64X / ui64Num;
		uint64_t ui64B = ui64X % ui64Num;
		return ui64Q * _ui32AudioHz * ui64Den + ui64A * ui64Den + (ui64B * ui64Den) / ui64Num;	// ui64B * ui64Den < 2^32 * 2^32.
	}

	/**
	 * Gets the last lines of a log file.
	 *
	 * \param _wsPath The log file.
	 * \param _sMaxLines The maximum number of lines to return.
	 * \return Returns the last lines of the file.
	 */
	std::wstring CVideoExporter::LogTail( const std::wstring &_wsPath, size_t _sMaxLines ) {
		try {
#if defined( _WIN32 )
			std::FILE * pfFile = ::_wfopen( _wsPath.c_str(), L"rb" );
#else
			std::FILE * pfFile = std::fopen( CFfmpegProcess::ToUtf8( _wsPath ).c_str(), "rb" );
#endif	// #if defined( _WIN32 )
			if ( !pfFile ) { return std::wstring(); }
			std::string sText;
			char szBuffer[4096];
			size_t sRead;
			// Only the end matters; keep the last 64 KB.
			while ( (sRead = std::fread( szBuffer, 1, sizeof( szBuffer ), pfFile )) != 0 ) {
				sText.append( szBuffer, sRead );
				if ( sText.size() > 128 * 1024 ) { sText.erase( 0, sText.size() - 64 * 1024 ); }
			}
			std::fclose( pfFile );
			return TextTail( sText, _sMaxLines );
		}
		catch ( ... ) { return std::wstring(); }
	}

	/**
	 * Gets the last lines of some text.
	 *
	 * \param _sText The text.
	 * \param _sMaxLines The maximum number of lines to return.
	 * \return Returns the last lines of the text.
	 */
	std::wstring CVideoExporter::TextTail( const std::string &_sText, size_t _sMaxLines ) {
		std::vector<std::string> vLines;
		std::istringstream issLines( _sText );
		std::string sLine;
		while ( std::getline( issLines, sLine ) ) {
			while ( !sLine.empty() && (sLine.back() == '\r' || sLine.back() == ' ') ) { sLine.pop_back(); }
			if ( !sLine.empty() ) { vLines.push_back( sLine ); }
		}
		std::wstring wsRet;
		for ( size_t I = vLines.size() > _sMaxLines ? vLines.size() - _sMaxLines : 0; I < vLines.size(); ++I ) {
			if ( !wsRet.empty() ) { wsRet += L"\r\n"; }
			wsRet += CFfmpegProcess::FromUtf8( vLines[I] );
		}
		return wsRet;
	}

}	// namespace lsn
