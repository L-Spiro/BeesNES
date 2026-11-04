/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Streams captured frames and audio samples to FFmpeg to make a movie file.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Display/LSNCaptureBuffer.h"
#include "../Options/LSNVideoExportOptions.h"
#include "LSNFfmpegProcess.h"
#include "LSNVideoFormat.h"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>


namespace lsn {

	/**
	 * Class CVideoExporter
	 * \brief Streams captured frames and audio samples to FFmpeg to make a movie file.
	 *
	 * Description: Streams captured frames and audio samples to FFmpeg to make a movie file.  FFmpeg runs as a separate process and is fed
	 *	through 2 pipes, each written by its own thread.
	 *
	 * Threads:
	 *	Start() and Stop() are called while the emulation thread is stopped.
	 *	SubmitFrame() is called by the thread that renders (after each rendered frame has been captured).
	 *	AddSample() and FrameBoundary() are called by the emulation thread.
	 *
	 * No frame is ever dropped:  at most LSN_POOL_SIZE frames are held in memory, and when they are all in use FrameBoundary() waits (the
	 *	emulation slows down to the speed of FFmpeg).
	 *
	 * Whenever the frame size, the frame format, the audio rate, or the emulation frame rate changes, the current movie is finished and a
	 *	new one is started automatically.
	 */
	class CVideoExporter {
	public :
		CVideoExporter();
		~CVideoExporter();


		// == Enumerations.
		/** Sizes. */
		enum LSN_SIZES : size_t {
			LSN_POOL_SIZE										= 4,										/**< The number of frames that can be held in memory. */
			LSN_MAX_AUDIO_BLOCKS								= 2048,										/**< The number of audio blocks (1 per frame) that can wait for the audio thread before the emulation waits.  FFmpeg stops reading audio until the video encoder has filled its look-ahead, so this must cover several seconds. */
		};


		// == Types.
		/** A frame rate. */
		struct LSN_RATE {
			uint32_t											ui32Num = 0;								/**< The numerator. */
			uint32_t											ui32Den = 1;								/**< The denominator. */


			// == Operators.
			/**
			 * Compares 2 rates.
			 *
			 * \param _rOther The rate against which to compare.
			 * \return Returns true if the numerators and denominators match.
			 */
			bool												operator == ( const LSN_RATE &_rOther ) const { return ui32Num == _rOther.ui32Num && ui32Den == _rOther.ui32Den; }

			/**
			 * Compares 2 rates.
			 *
			 * \param _rOther The rate against which to compare.
			 * \return Returns true if the numerators or denominators differ.
			 */
			bool												operator != ( const LSN_RATE &_rOther ) const { return !((*this) == _rOther); }


			// == Functions.
			/**
			 * Gets the rate as a double.
			 *
			 * \return Returns the rate as a double.
			 */
			double												Value() const { return ui32Den ? double( ui32Num ) / ui32Den : 0.0; }
		};

		/**
		 * Generates the path for each new movie file.
		 *
		 * \param _pvParm The parameter passed to Start().
		 * \return Returns the full path for a new movie file, or an empty string on failure.
		 */
		typedef std::wstring (*									PfNewPath)( void * _pvParm );


		// == Functions.
		/**
		 * Gets the frame rate of a region.
		 *
		 * \param _pmRegion The region.
		 * \return Returns the exact frame rate of the region (NTSC for unknown regions).
		 */
		static LSN_RATE											RegionFrameRate( LSN_PPU_METRICS _pmRegion );

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
		bool													Start( const LSN_VIDEO_EXPORT_OPTIONS &_veoOptions, LSN_RATE _rEmuRate, uint32_t _ui32AudioHz,
			PfNewPath _pfNewPath, void * _pvParm, std::wstring &_wsError );

		/**
		 * Stops exporting:  sends any frames and samples that are still waiting, then lets FFmpeg finish the file.  Call only while the
		 *	emulation thread is stopped.  Does nothing if not exporting.
		 *
		 * \param _wsError Receives an error message on failure (including any earlier failure that ended the export).
		 * \return Returns true if every movie was finished without error.
		 */
		bool													Stop( std::wstring &_wsError );

		/**
		 * Determines whether a movie is being exported (Start() succeeded and Stop() has not been called).
		 *
		 * \return Returns true if exporting.
		 */
		inline bool												IsActive() const { return m_bActive; }

		/**
		 * Determines whether the export has failed.  The export stays active (so that Stop() can report the error) but no longer accepts
		 *	data and never waits.
		 *
		 * \return Returns true if an error ended the export.
		 */
		inline bool												Failed() const { return m_abFailed; }

		/**
		 * Makes every wait return immediately (true) or restores normal waiting (false).  Set while stopping the emulation thread so that it
		 *	cannot remain blocked waiting for FFmpeg.
		 *
		 * \param _bAbort Whether to abort waits.
		 */
		void													SetAbortWaits( bool _bAbort );

		/**
		 * Adds a rendered frame.  Called on the rendering thread after each rendered frame has been captured.  The frame is copied.
		 *
		 * \param _cbFrame The captured frame.
		 */
		void													SubmitFrame( const LSN_CAPTURE_BUFFER &_cbFrame );

		/**
		 * Receives each audio sample.  Called on the emulation thread for each sample the APU outputs (set as the APU's sample-out function).
		 *
		 * \param _pvThis A pointer to this object.
		 * \param _fSample The sample.
		 */
		static void												AddSample( void * _pvThis, float _fSample );

		/**
		 * Called on the emulation thread at the end of each emulated frame, before the frame is handed to the renderer.  Sends the frames
		 *	that have been submitted since the last call to FFmpeg (restarting it if anything about the stream changed), sends the matching
		 *	audio, and waits until there is room for the next frame.
		 *
		 * \param _rEmuRate The emulation frame rate.
		 * \param _ui32AudioHz The current audio rate.
		 * \return Returns false if the export has failed.
		 */
		bool													FrameBoundary( LSN_RATE _rEmuRate, uint32_t _ui32AudioHz );

		/**
		 * Gets the path of the movie currently being written.
		 *
		 * \return Returns the path of the movie currently being written, or an empty string.
		 */
		std::wstring											CurrentPath() const;

		/**
		 * Gets the path of every movie made since Start().
		 *
		 * \return Returns the paths of every movie made since Start().
		 */
		const std::vector<std::wstring> &						Movies() const { return m_vMovies; }

		/**
		 * Gets the input frame rate given to FFmpeg for a format:  the emulation rate, or the closest rate the format can store if it cannot
		 *	store the exact rate.
		 *
		 * \param _vfdFormat The video format.
		 * \param _rEmuRate The emulation frame rate.
		 * \return Returns the input frame rate.
		 */
		static LSN_RATE											InputRate( const LSN_VIDEO_FORMAT_DATA &_vfdFormat, LSN_RATE _rEmuRate );

		/**
		 * Converts a frame rate in frames per second to a fraction.  Common NTSC-style rates (such as 59.94) become exact fractions (60000/1001).
		 *
		 * \param _dRate The rate.
		 * \param _ui32MaxNum The largest allowed numerator, or 0 for no limit.
		 * \return Returns the fraction.
		 */
		static LSN_RATE											ToRate( double _dRate, uint32_t _ui32MaxNum );

		/**
		 * Finds the fraction closest to _rRate whose numerator does not exceed _ui32MaxNum.
		 *
		 * \param _rRate The rate to approximate.
		 * \param _ui32MaxNum The largest allowed numerator.
		 * \return Returns the closest fraction.
		 */
		static LSN_RATE											LimitNumerator( LSN_RATE _rRate, uint32_t _ui32MaxNum );

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
		static bool												BuildOutputArgs( const LSN_VIDEO_FORMAT_DATA &_vfdFormat, const LSN_VIDEO_EXPORT_OPTIONS::LSN_FORMAT_OPTIONS &_foOptions,
			bool _bInterpolate, uint32_t _ui32Width, uint32_t _ui32Height, LSN_RATE _rInRate, const std::wstring &_wsOutPath,
			std::vector<std::wstring> &_vArgs, std::wstring &_wsError );


	protected :
		// == Types.
		/** The properties of a stream; a change in any of them requires a new movie. */
		struct LSN_STREAM_PROPS {
			uint32_t											ui32Width = 0;								/**< The width of the frames sent to FFmpeg. */
			uint32_t											ui32Height = 0;								/**< The height of the frames sent to FFmpeg. */
			LSN_CAPTURE_FORMAT									cfFormat = LSN_CF_UNKNOWN;					/**< The format of the frames sent to FFmpeg. */
			LSN_RATE											rEmuRate;									/**< The emulation frame rate. */
			uint32_t											ui32AudioHz = 0;							/**< The audio rate. */


			// == Operators.
			/**
			 * Compares 2 sets of properties.
			 *
			 * \param _spOther The properties against which to compare.
			 * \return Returns true if all properties match.
			 */
			bool												operator == ( const LSN_STREAM_PROPS &_spOther ) const {
				return ui32Width == _spOther.ui32Width && ui32Height == _spOther.ui32Height && cfFormat == _spOther.cfFormat &&
					rEmuRate == _spOther.rEmuRate && ui32AudioHz == _spOther.ui32AudioHz;
			}
		};

		/**
		 * Class CSession
		 * \brief One FFmpeg process making one movie file.
		 *
		 * Description: One FFmpeg process making one movie file, with a thread writing each of its 2 pipes.
		 */
		class CSession {
		public :
			CSession( CVideoExporter * _pveOwner );
			~CSession();


			// == Functions.
			/**
			 * Launches FFmpeg and the writer threads.
			 *
			 * \param _spProps The stream properties.
			 * \param _wsPath The movie file.
			 * \param _wsError Receives an error message on failure.
			 * \return Returns true if FFmpeg and the threads were started.
			 */
			bool												Begin( const LSN_STREAM_PROPS &_spProps, const std::wstring &_wsPath, std::wstring &_wsError );

			/**
			 * Sends the remaining data, closes the pipes, and waits for FFmpeg to finish the file.
			 *
			 * \param _wsError Receives an error message on failure.
			 * \return Returns true if FFmpeg finished the file without error.
			 */
			bool												End( std::wstring &_wsError );

			/**
			 * Queues a pool frame for the video thread.
			 *
			 * \param _sIdx The index of the pool frame.
			 */
			void												QueueFrame( size_t _sIdx );

			/**
			 * Queues audio samples for the audio thread, waiting first if too many are already queued.
			 *
			 * \param _pfSamples The samples.
			 * \param _sTotal The number of samples.
			 * \return Returns false if the session has failed or waits were aborted.
			 */
			bool												QueueAudio( const float * _pfSamples, size_t _sTotal );

			/**
			 * Determines whether the session has failed.
			 *
			 * \return Returns true if a write failed or FFmpeg exited early.
			 */
			inline bool											Failed() const { return m_abFailed; }

			/**
			 * Gets the error message after a failure.
			 *
			 * \return Returns the error message.
			 */
			std::wstring										Error() const;

			/**
			 * Wakes any wait in QueueAudio() (used when waits are aborted).
			 */
			void												Wake();


			// == Members.
			LSN_STREAM_PROPS									spProps;									/**< The stream properties. */
			std::wstring										wsPath;										/**< The movie file. */
			std::wstring										wsLogPath;									/**< FFmpeg's log. */
			uint64_t											ui64Frames = 0;								/**< The number of frames queued. */
			uint64_t											ui64Samples = 0;							/**< The number of audio samples queued. */
			LSN_RATE											rInRate;									/**< The frame rate given to FFmpeg. */


		protected :
			// == Members.
			CVideoExporter *									m_pveOwner;									/**< The exporter that owns the frame pool. */
			CFfmpegProcess										m_fpProcess;								/**< FFmpeg. */
			std::thread											m_tVideo;									/**< Writes the video pipe. */
			std::thread											m_tAudio;									/**< Writes the audio pipe. */
			mutable std::mutex									m_mMutex;									/**< Guards the queues and the error message. */
			std::condition_variable								m_cvVideo;									/**< Signaled when a frame is queued or the session is closing. */
			std::condition_variable								m_cvAudio;									/**< Signaled when audio is queued or the session is closing. */
			std::condition_variable								m_cvAudioSpace;								/**< Signaled when the audio thread takes a block. */
			std::deque<size_t>									m_dFrames;									/**< Pool frames waiting to be written. */
			std::deque<std::vector<float>>						m_dAudio;									/**< Audio blocks waiting to be written. */
			std::wstring										m_wsError;									/**< The first error. */
			std::atomic<bool>									m_abFailed;									/**< Set on failure. */
			bool												m_bClosing = false;							/**< Set by End(). */
			bool												m_bStarted = false;							/**< Set once the threads are running. */


			// == Functions.
			/**
			 * Records a failure.  The first message is kept.
			 *
			 * \param _wsError The error message.
			 */
			void												SetFailed( const std::wstring &_wsError );

			/**
			 * The video thread.
			 *
			 * \param _psThis This object.
			 */
			static void											VideoThread( CSession * _psThis );

			/**
			 * The audio thread.
			 *
			 * \param _psThis This object.
			 */
			static void											AudioThread( CSession * _psThis );
		};


		// == Members.
		LSN_CAPTURE_BUFFER										m_cbPool[LSN_POOL_SIZE];					/**< The frames held in memory. */
		LSN_CAPTURE_BUFFER										m_cbConverted;								/**< Used to find the format of a frame after conversion. */
		std::vector<size_t>										m_vFree;									/**< Unused pool frames. */
		std::deque<size_t>										m_dIncoming;								/**< Pool frames submitted but not yet sent to a session. */
		mutable std::mutex										m_mPool;									/**< Guards m_vFree and m_dIncoming. */
		std::condition_variable									m_cvPool;									/**< Signaled when a pool frame is freed. */
		std::unique_ptr<CSession>								m_psSession;								/**< The current FFmpeg session. */
		std::vector<float>										m_vStage;									/**< Audio samples not yet sent to a session. */
		uint64_t												m_ui64SamplesThisFrame = 0;					/**< Samples received since the last FrameBoundary(). */
		std::wstring											m_wsFfmpeg;									/**< The FFmpeg executable. */
		std::wstring											m_wsError;									/**< The first error. */
		std::vector<std::wstring>								m_vMovies;									/**< Every movie made since Start(). */
		const LSN_VIDEO_FORMAT_DATA *							m_pvfdFormat = nullptr;						/**< The video format. */
		LSN_VIDEO_EXPORT_OPTIONS::LSN_FORMAT_OPTIONS			m_foOptions;								/**< The settings for the video format. */
		PfNewPath												m_pfNewPath = nullptr;						/**< Generates movie paths. */
		void *													m_pvNewPathParm = nullptr;					/**< The parameter for m_pfNewPath. */
		LSN_RATE												m_rLastRate;								/**< The emulation frame rate at the last frame boundary. */
		uint32_t												m_ui32LastAudioHz = 0;						/**< The audio rate at the last frame boundary. */
		std::atomic<bool>										m_abFailed;									/**< Set on failure. */
		std::atomic<bool>										m_abAbort;									/**< Set to make waits return immediately. */
		LSN_VIDEO_FORMAT										m_vfFormat = LSN_VF_H264;					/**< The video format. */
		bool													m_bInterpolate = true;						/**< Blend frames when changing the frame rate. */
		bool													m_bActive = false;							/**< Set between Start() and Stop(). */


		// == Functions.
		/**
		 * Records a failure.  The first message is kept.
		 *
		 * \param _wsError The error message.
		 */
		void													Fail( const std::wstring &_wsError );

		/**
		 * Returns a pool frame to the free list.
		 *
		 * \param _sIdx The index of the pool frame.
		 */
		void													ReleaseFrame( size_t _sIdx );

		/**
		 * Sends a submitted frame to the current session, starting a new session first if there is none or if the stream properties changed.
		 *
		 * \param _sIdx The index of the pool frame.
		 * \param _rEmuRate The emulation frame rate.
		 * \param _ui32AudioHz The audio rate.
		 * \return Returns false on failure.
		 */
		bool													ProcessFrame( size_t _sIdx, LSN_RATE _rEmuRate, uint32_t _ui32AudioHz );

		/**
		 * Sends staged audio to the current session:  as many samples as match the frames sent so far.  If no samples were produced during
		 *	the frame (audio is disabled), silence is sent instead so that the audio keeps pace with the video.
		 *
		 * \return Returns false on failure.
		 */
		bool													CommitAudio();

		/**
		 * Handles a failed CSession::QueueAudio():  if the session failed it is ended (recording the error); if waits were aborted the samples
		 *	simply stay staged.
		 *
		 * \return Returns false if the export has failed.
		 */
		bool													QueueFailed();

		/**
		 * Finishes the current session (if any).
		 *
		 * \return Returns false if the session failed.
		 */
		bool													EndSession();

		/**
		 * Finds FFmpeg and verifies that it supports the selected format, then runs a short test encode.
		 *
		 * \param _rEmuRate The emulation frame rate.
		 * \param _ui32AudioHz The audio rate.
		 * \param _wsError Receives an error message on failure.
		 * \return Returns true if FFmpeg can make the movie.
		 */
		bool													CheckFfmpeg( LSN_RATE _rEmuRate, uint32_t _ui32AudioHz, std::wstring &_wsError );

		/**
		 * Gets the number of audio samples that match a number of frames.
		 *
		 * \param _ui64Frames The number of frames.
		 * \param _rRate The frame rate.
		 * \param _ui32AudioHz The audio rate.
		 * \return Returns the number of audio samples that span the given number of frames.
		 */
		static uint64_t											SamplesForFrames( uint64_t _ui64Frames, LSN_RATE _rRate, uint32_t _ui32AudioHz );

		/**
		 * Gets the last lines of a log file.
		 *
		 * \param _wsPath The log file.
		 * \param _sMaxLines The maximum number of lines to return.
		 * \return Returns the last lines of the file.
		 */
		static std::wstring										LogTail( const std::wstring &_wsPath, size_t _sMaxLines );

		/**
		 * Gets the last lines of some text.
		 *
		 * \param _sText The text.
		 * \param _sMaxLines The maximum number of lines to return.
		 * \return Returns the last lines of the text.
		 */
		static std::wstring										TextTail( const std::string &_sText, size_t _sMaxLines );
	};

}	// namespace lsn
