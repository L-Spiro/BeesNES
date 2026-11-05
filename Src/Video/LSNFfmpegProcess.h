/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Runs FFmpeg as a child process fed by 2 pipes (video and audio).  Windows uses named pipes; other platforms use FIFOs.
 */


#pragma once

#include "../LSNLSpiroNes.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

#if !defined( _WIN32 )
#include <sys/types.h>
#endif	// #if !defined( _WIN32 )


namespace lsn {

	/**
	 * Class CFfmpegProcess
	 * \brief Runs FFmpeg as a child process fed by 2 pipes (video and audio).
	 *
	 * Description: Runs FFmpeg as a child process fed by 2 pipes (video and audio).  Windows uses named pipes; other platforms use FIFOs.
	 *	Every blocking operation (connecting a pipe, writing to it, waiting for the process) returns early with a failure if FFmpeg exits or
	 *	Cancel() is called, so a dead or stuck FFmpeg can never hang the caller forever.
	 */
	class CFfmpegProcess {
	public :
		CFfmpegProcess();
		~CFfmpegProcess();


		// == Enumerations.
		/** The pipes. */
		enum LSN_PIPE : uint32_t {
			LSN_P_VIDEO,																						/**< The video pipe. */
			LSN_P_AUDIO,																						/**< The audio pipe. */
			LSN_P_TOTAL,																						/**< The number of pipes. */
		};


		// == Functions.
		/**
		 * Creates the pipes.  Must be called before Launch() so that the paths can be passed to FFmpeg as inputs.
		 *
		 * \param _wsVideoPath Receives the path FFmpeg must open to read the video.
		 * \param _wsAudioPath Receives the path FFmpeg must open to read the audio.
		 * \return Returns true if both pipes were created.
		 */
		bool													CreatePipes( std::wstring &_wsVideoPath, std::wstring &_wsAudioPath );

		/**
		 * Launches FFmpeg.  Its standard input and output are discarded and its standard error is written to _wsLogPath.
		 *
		 * \param _wsExe The path to the FFmpeg executable.
		 * \param _vArgs The arguments (not including the executable).
		 * \param _wsLogPath The file to which to write FFmpeg's messages.
		 * \return Returns true if the process was started.
		 */
		bool													Launch( const std::wstring &_wsExe, const std::vector<std::wstring> &_vArgs, const std::wstring &_wsLogPath );

		/**
		 * Waits for FFmpeg to open a pipe.
		 *
		 * \param _pPipe The pipe.
		 * \return Returns true if FFmpeg opened the pipe; false if FFmpeg exited first, Cancel() was called, or an error occurred.
		 */
		bool													ConnectPipe( LSN_PIPE _pPipe );

		/**
		 * Writes to a pipe, blocking until all of the data has been written.
		 *
		 * \param _pPipe The pipe.
		 * \param _pvData The data to write.
		 * \param _sSize The number of bytes to write.
		 * \return Returns true if all of the data was written; false if FFmpeg exited or stopped reading, Cancel() was called, or an error occurred.
		 */
		bool													WritePipe( LSN_PIPE _pPipe, const void * _pvData, size_t _sSize );

		/**
		 * Closes a pipe, which FFmpeg sees as the end of that input.
		 *
		 * \param _pPipe The pipe.
		 */
		void													ClosePipe( LSN_PIPE _pPipe );

		/**
		 * Waits for FFmpeg to exit.
		 *
		 * \param _ui32Milliseconds The maximum time to wait.
		 * \param _i32ExitCode Receives the exit code.
		 * \return Returns true if the process exited within the time limit (or was never launched, in which case _i32ExitCode is -1).
		 */
		bool													WaitForExit( uint32_t _ui32Milliseconds, int32_t &_i32ExitCode );

		/**
		 * Forcibly ends FFmpeg.
		 */
		void													Kill();

		/**
		 * Makes every pending and future ConnectPipe() and WritePipe() call fail immediately.
		 */
		void													Cancel();

		/**
		 * Determines whether FFmpeg is still running.
		 *
		 * \return Returns true if the process was launched and has not exited.
		 */
		bool													IsRunning() const;

		/**
		 * Closes the pipes and the process handles.  If the process is still running it is left to finish on its own.
		 */
		void													Reset();

		/**
		 * Runs a program to completion, capturing everything it writes to its standard output and standard error.
		 *
		 * \param _wsExe The path to the executable.
		 * \param _vArgs The arguments (not including the executable).
		 * \param _ui32TimeoutMs The maximum time to let the program run.  It is killed after that.
		 * \param _sOutput Receives the program's output.
		 * \param _i32ExitCode Receives the exit code.
		 * \return Returns true if the program ran and exited within the time limit.
		 */
		static bool												RunAndCapture( const std::wstring &_wsExe, const std::vector<std::wstring> &_vArgs, uint32_t _ui32TimeoutMs,
			std::string &_sOutput, int32_t &_i32ExitCode );

		/**
		 * Finds the FFmpeg executable.  The folder containing BeesNES is checked first (and its ffmpeg/bin sub-folder), then the system
		 *	search path.
		 *
		 * \return Returns the full path to FFmpeg or an empty string if it could not be found.
		 */
		static std::wstring										FindFfmpeg();

		/**
		 * Finds the FFmpeg executable given the path set by the user.  An empty path searches for FFmpeg (see FindFfmpeg()).  A folder is
		 *	searched for FFmpeg (and so is its bin sub-folder).  A file is used as-is if it exists.
		 *
		 * \param _wsPath The path set by the user.
		 * \return Returns the full path to FFmpeg or an empty string if it could not be found.
		 */
		static std::wstring										ResolveFfmpeg( const std::wstring &_wsPath );

		/**
		 * Converts a wide string to UTF-8.
		 *
		 * \param _wsString The string to convert.
		 * \return Returns the UTF-8 string.
		 */
		static std::string										ToUtf8( const std::wstring &_wsString );

		/**
		 * Converts a UTF-8 string to a wide string.
		 *
		 * \param _sString The string to convert.
		 * \return Returns the wide string.
		 */
		static std::wstring										FromUtf8( const std::string &_sString );


	protected :
		// == Members.
#if defined( _WIN32 )
		void *													m_hPipes[LSN_P_TOTAL];						/**< The server ends of the named pipes (HANDLE). */
		void *													m_hEvents[LSN_P_TOTAL];						/**< The events for overlapped operations on each pipe (HANDLE). */
		void *													m_hCancel;									/**< Signaled by Cancel() (HANDLE). */
		void *													m_hProcess;									/**< The FFmpeg process (HANDLE). */
#else
		std::string												m_sFifoPaths[LSN_P_TOTAL];					/**< The paths of the FIFOs. */
		int														m_iFds[LSN_P_TOTAL];						/**< The write ends of the FIFOs. */
		pid_t													m_pPid;										/**< The FFmpeg process. */
		mutable std::atomic<bool>								m_abExited;									/**< Set once the process has been reaped. */
		int32_t													m_i32ExitCode;								/**< The exit code once reaped. */
		std::atomic<bool>										m_abCancel;									/**< Set by Cancel(). */
#endif	// #if defined( _WIN32 )


		// == Functions.
		/**
		 * Gets the folder containing the running executable, including the trailing separator.
		 *
		 * \return Returns the folder containing the running executable.
		 */
		static std::wstring										ExeFolder();
	};

}	// namespace lsn
