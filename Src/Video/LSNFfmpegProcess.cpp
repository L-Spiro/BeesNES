/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Runs FFmpeg as a child process fed by 2 pipes (video and audio).  Windows uses named pipes; other platforms use FIFOs.
 */

#include "LSNFfmpegProcess.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <thread>

#if !defined( _WIN32 )
#include <cerrno>
#include <climits>
#include <csignal>
#include <cstdlib>
#include <fcntl.h>
#include <poll.h>
#include <spawn.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#if defined( __APPLE__ )
#include <mach-o/dyld.h>
#endif	// #if defined( __APPLE__ )

extern char ** environ;
#endif	// #if defined( _WIN32 )


namespace lsn {

	namespace {

		/** Distinguishes the pipes of successive sessions. */
		std::atomic<uint32_t> aui32PipeCounter{ 0 };

#if defined( _WIN32 )
		/**
		 * Appends an argument to a Windows command line, quoting it so that CommandLineToArgvW() and the CRT parse it back unchanged.
		 *
		 * \param _wsCmd The command line to which to append.
		 * \param _wsArg The argument to append.
		 */
		void													AppendQuoted( std::wstring &_wsCmd, const std::wstring &_wsArg ) {
			if ( !_wsCmd.empty() ) { _wsCmd += L' '; }
			if ( !_wsArg.empty() && _wsArg.find_first_of( L" \t\n\v\"" ) == std::wstring::npos ) {
				_wsCmd += _wsArg;
				return;
			}
			_wsCmd += L'"';
			for ( auto I = _wsArg.begin(); ; ++I ) {
				size_t sBackslashes = 0;
				while ( I != _wsArg.end() && (*I) == L'\\' ) {
					++I;
					++sBackslashes;
				}
				if ( I == _wsArg.end() ) {
					// Backslashes before the closing quote must be doubled.
					_wsCmd.append( sBackslashes * 2, L'\\' );
					break;
				}
				else if ( (*I) == L'"' ) {
					_wsCmd.append( sBackslashes * 2 + 1, L'\\' );
					_wsCmd += L'"';
				}
				else {
					_wsCmd.append( sBackslashes, L'\\' );
					_wsCmd += (*I);
				}
			}
			_wsCmd += L'"';
		}

		/**
		 * Starts a process whose standard handles are the given handles.  Only those handles are inherited.
		 *
		 * \param _wsExe The path to the executable.
		 * \param _vArgs The arguments (not including the executable).
		 * \param _hStdIn The standard input handle (must be inheritable).
		 * \param _hStdOut The standard output handle (must be inheritable).
		 * \param _hStdErr The standard error handle (must be inheritable).
		 * \return Returns the process handle or NULL.
		 */
		HANDLE													StartProcess( const std::wstring &_wsExe, const std::vector<std::wstring> &_vArgs, HANDLE _hStdIn, HANDLE _hStdOut, HANDLE _hStdErr ) {
			std::wstring wsCmd;
			try {
				AppendQuoted( wsCmd, _wsExe );
				for ( size_t I = 0; I < _vArgs.size(); ++I ) {
					AppendQuoted( wsCmd, _vArgs[I] );
				}
			}
			catch ( ... ) { return NULL; }

			// Restrict inheritance to the standard handles so that no other handle (such as the server end of a pipe) leaks into FFmpeg.
			HANDLE hInherit[3];
			DWORD dwInherit = 0;
			for ( HANDLE hThis : { _hStdIn, _hStdOut, _hStdErr } ) {
				if ( std::find( hInherit, hInherit + dwInherit, hThis ) == hInherit + dwInherit ) { hInherit[dwInherit++] = hThis; }
			}
			SIZE_T stAttrSize = 0;
			::InitializeProcThreadAttributeList( NULL, 1, 0, &stAttrSize );
			std::vector<uint8_t> vAttr;
			try {
				vAttr.resize( stAttrSize );
			}
			catch ( ... ) { return NULL; }
			LPPROC_THREAD_ATTRIBUTE_LIST paList = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(vAttr.data());
			if ( !::InitializeProcThreadAttributeList( paList, 1, 0, &stAttrSize ) ) { return NULL; }
			if ( !::UpdateProcThreadAttribute( paList, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, hInherit, dwInherit * sizeof( HANDLE ), NULL, NULL ) ) {
				::DeleteProcThreadAttributeList( paList );
				return NULL;
			}

			STARTUPINFOEXW siInfo = {};
			siInfo.StartupInfo.cb = sizeof( siInfo );
			siInfo.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
			siInfo.StartupInfo.hStdInput = _hStdIn;
			siInfo.StartupInfo.hStdOutput = _hStdOut;
			siInfo.StartupInfo.hStdError = _hStdErr;
			siInfo.lpAttributeList = paList;
			PROCESS_INFORMATION piInfo = {};
			BOOL bOk = ::CreateProcessW( _wsExe.c_str(), wsCmd.data(), NULL, NULL, TRUE,
				CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &siInfo.StartupInfo, &piInfo );
			::DeleteProcThreadAttributeList( paList );
			if ( !bOk ) { return NULL; }
			::CloseHandle( piInfo.hThread );
			return piInfo.hProcess;
		}

		/**
		 * Opens the NUL device as an inheritable handle.
		 *
		 * \param _bWrite If true, the handle is opened for writing, otherwise for reading.
		 * \return Returns the handle or INVALID_HANDLE_VALUE.
		 */
		HANDLE													OpenNul( bool _bWrite ) {
			SECURITY_ATTRIBUTES saAttr = { sizeof( SECURITY_ATTRIBUTES ), NULL, TRUE };
			return ::CreateFileW( L"NUL", _bWrite ? GENERIC_WRITE : GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &saAttr, OPEN_EXISTING, 0, NULL );
		}
#else
		/**
		 * Builds a null-terminated argument array for posix_spawn().
		 *
		 * \param _sExe The executable.
		 * \param _vArgs The arguments.
		 * \param _vStorage Receives the UTF-8 strings (must outlive the returned array).
		 * \param _vArgv Receives the argument array.
		 */
		void													BuildArgv( const std::wstring &_wsExe, const std::vector<std::wstring> &_vArgs, std::vector<std::string> &_vStorage, std::vector<char *> &_vArgv ) {
			_vStorage.clear();
			_vStorage.push_back( CFfmpegProcess::ToUtf8( _wsExe ) );
			for ( size_t I = 0; I < _vArgs.size(); ++I ) {
				_vStorage.push_back( CFfmpegProcess::ToUtf8( _vArgs[I] ) );
			}
			_vArgv.clear();
			for ( size_t I = 0; I < _vStorage.size(); ++I ) {
				_vArgv.push_back( _vStorage[I].data() );
			}
			_vArgv.push_back( nullptr );
		}

		/**
		 * Determines whether a process has exited without reaping it.
		 *
		 * \param _pPid The process.
		 * \return Returns true if the process has exited (or does not exist).
		 */
		bool													HasExited( pid_t _pPid ) {
			siginfo_t siInfo;
			std::memset( &siInfo, 0, sizeof( siInfo ) );
			if ( ::waitid( P_PID, id_t( _pPid ), &siInfo, WEXITED | WNOHANG | WNOWAIT ) != 0 ) { return true; }
			return siInfo.si_pid != 0;
		}
#endif	// #if defined( _WIN32 )

	}	// namespace


	CFfmpegProcess::CFfmpegProcess() {
#if defined( _WIN32 )
		for ( size_t I = 0; I < LSN_P_TOTAL; ++I ) {
			m_hPipes[I] = INVALID_HANDLE_VALUE;
			m_hEvents[I] = NULL;
		}
		m_hCancel = NULL;
		m_hProcess = NULL;
#else
		for ( size_t I = 0; I < LSN_P_TOTAL; ++I ) {
			m_iFds[I] = -1;
		}
		m_pPid = -1;
		m_abExited = false;
		m_i32ExitCode = -1;
		m_abCancel = false;
#endif	// #if defined( _WIN32 )
	}
	CFfmpegProcess::~CFfmpegProcess() {
		Reset();
	}

	// == Functions.
	/**
	 * Creates the pipes.  Must be called before Launch() so that the paths can be passed to FFmpeg as inputs.
	 *
	 * \param _wsVideoPath Receives the path FFmpeg must open to read the video.
	 * \param _wsAudioPath Receives the path FFmpeg must open to read the audio.
	 * \return Returns true if both pipes were created.
	 */
	bool CFfmpegProcess::CreatePipes( std::wstring &_wsVideoPath, std::wstring &_wsAudioPath ) {
		Reset();
		uint32_t ui32Id = ++aui32PipeCounter;
#if defined( _WIN32 )
		m_hCancel = ::CreateEventW( NULL, TRUE, FALSE, NULL );
		if ( !m_hCancel ) { return false; }
		static const wchar_t * pwcNames[LSN_P_TOTAL] = { L"video", L"audio" };
		static const DWORD dwBufferSizes[LSN_P_TOTAL] = { 4 * 1024 * 1024, 256 * 1024 };
		std::wstring * pwsPaths[LSN_P_TOTAL] = { &_wsVideoPath, &_wsAudioPath };
		for ( size_t I = 0; I < LSN_P_TOTAL; ++I ) {
			try {
				(*pwsPaths[I]) = L"\\\\.\\pipe\\BeesNES_" + std::to_wstring( ::GetCurrentProcessId() ) + L"_" + std::to_wstring( ui32Id ) + L"_" +
					std::to_wstring( ::GetTickCount64() ) + L"_" + pwcNames[I];
			}
			catch ( ... ) { Reset(); return false; }
			m_hEvents[I] = ::CreateEventW( NULL, TRUE, FALSE, NULL );
			if ( !m_hEvents[I] ) { Reset(); return false; }
			m_hPipes[I] = ::CreateNamedPipeW( pwsPaths[I]->c_str(), PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED | FILE_FLAG_FIRST_PIPE_INSTANCE,
				PIPE_TYPE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS, 1, dwBufferSizes[I], 0, 0, NULL );
			if ( m_hPipes[I] == INVALID_HANDLE_VALUE ) { Reset(); return false; }
		}
		return true;
#else
		static const char * pcNames[LSN_P_TOTAL] = { "video", "audio" };
		std::wstring * pwsPaths[LSN_P_TOTAL] = { &_wsVideoPath, &_wsAudioPath };
		const char * pcTmp = std::getenv( "TMPDIR" );
		std::string sFolder = (pcTmp && pcTmp[0]) ? pcTmp : "/tmp";
		if ( sFolder.back() != '/' ) { sFolder += '/'; }
		for ( size_t I = 0; I < LSN_P_TOTAL; ++I ) {
			try {
				m_sFifoPaths[I] = sFolder + "BeesNES_" + std::to_string( ::getpid() ) + "_" + std::to_string( ui32Id ) + "_" + pcNames[I] + ".fifo";
				::unlink( m_sFifoPaths[I].c_str() );
				if ( ::mkfifo( m_sFifoPaths[I].c_str(), 0600 ) != 0 ) {
					m_sFifoPaths[I].clear();
					Reset();
					return false;
				}
				(*pwsPaths[I]) = FromUtf8( m_sFifoPaths[I] );
			}
			catch ( ... ) { Reset(); return false; }
		}
		// Writing to a pipe whose reader has gone raises SIGPIPE, which would end BeesNES.  Get EPIPE instead.
		std::signal( SIGPIPE, SIG_IGN );
		return true;
#endif	// #if defined( _WIN32 )
	}

	/**
	 * Launches FFmpeg.  Its standard input and output are discarded and its standard error is written to _wsLogPath.
	 *
	 * \param _wsExe The path to the FFmpeg executable.
	 * \param _vArgs The arguments (not including the executable).
	 * \param _wsLogPath The file to which to write FFmpeg's messages.
	 * \return Returns true if the process was started.
	 */
	bool CFfmpegProcess::Launch( const std::wstring &_wsExe, const std::vector<std::wstring> &_vArgs, const std::wstring &_wsLogPath ) {
#if defined( _WIN32 )
		if ( m_hProcess ) { return false; }
		SECURITY_ATTRIBUTES saAttr = { sizeof( SECURITY_ATTRIBUTES ), NULL, TRUE };
		HANDLE hLog = ::CreateFileW( _wsLogPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, &saAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		HANDLE hIn = OpenNul( false );
		HANDLE hOut = OpenNul( true );
		if ( hLog != INVALID_HANDLE_VALUE && hIn != INVALID_HANDLE_VALUE && hOut != INVALID_HANDLE_VALUE ) {
			m_hProcess = StartProcess( _wsExe, _vArgs, hIn, hOut, hLog );
		}
		if ( hLog != INVALID_HANDLE_VALUE ) { ::CloseHandle( hLog ); }
		if ( hIn != INVALID_HANDLE_VALUE ) { ::CloseHandle( hIn ); }
		if ( hOut != INVALID_HANDLE_VALUE ) { ::CloseHandle( hOut ); }
		return m_hProcess != NULL;
#else
		if ( m_pPid > 0 ) { return false; }
		std::vector<std::string> vStorage;
		std::vector<char *> vArgv;
		std::string sLog;
		try {
			BuildArgv( _wsExe, _vArgs, vStorage, vArgv );
			sLog = ToUtf8( _wsLogPath );
		}
		catch ( ... ) { return false; }

		posix_spawn_file_actions_t pfaActions;
		if ( ::posix_spawn_file_actions_init( &pfaActions ) != 0 ) { return false; }
		bool bOk = ::posix_spawn_file_actions_addopen( &pfaActions, 0, "/dev/null", O_RDONLY, 0 ) == 0 &&
			::posix_spawn_file_actions_addopen( &pfaActions, 1, "/dev/null", O_WRONLY, 0 ) == 0 &&
			::posix_spawn_file_actions_addopen( &pfaActions, 2, sLog.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644 ) == 0;
		pid_t pPid = -1;
		if ( bOk ) {
			bOk = ::posix_spawn( &pPid, vArgv[0], &pfaActions, nullptr, vArgv.data(), environ ) == 0;
		}
		::posix_spawn_file_actions_destroy( &pfaActions );
		if ( !bOk ) { return false; }
		m_pPid = pPid;
		m_abExited = false;
		m_i32ExitCode = -1;
		return true;
#endif	// #if defined( _WIN32 )
	}

	/**
	 * Waits for FFmpeg to open a pipe.
	 *
	 * \param _pPipe The pipe.
	 * \return Returns true if FFmpeg opened the pipe; false if FFmpeg exited first, Cancel() was called, or an error occurred.
	 */
	bool CFfmpegProcess::ConnectPipe( LSN_PIPE _pPipe ) {
		if ( _pPipe >= LSN_P_TOTAL ) { return false; }
#if defined( _WIN32 )
		if ( m_hPipes[_pPipe] == INVALID_HANDLE_VALUE || !m_hProcess ) { return false; }
		OVERLAPPED oOver = {};
		oOver.hEvent = m_hEvents[_pPipe];
		::ResetEvent( oOver.hEvent );
		if ( ::ConnectNamedPipe( m_hPipes[_pPipe], &oOver ) ) { return true; }
		DWORD dwErr = ::GetLastError();
		if ( dwErr == ERROR_PIPE_CONNECTED ) { return true; }
		if ( dwErr != ERROR_IO_PENDING ) { return false; }
		HANDLE hWait[3] = { oOver.hEvent, m_hProcess, m_hCancel };
		DWORD dwRes = ::WaitForMultipleObjects( 3, hWait, FALSE, INFINITE );
		DWORD dwTmp;
		if ( dwRes != WAIT_OBJECT_0 ) {
			::CancelIoEx( m_hPipes[_pPipe], &oOver );
			::GetOverlappedResult( m_hPipes[_pPipe], &oOver, &dwTmp, TRUE );
			return false;
		}
		return ::GetOverlappedResult( m_hPipes[_pPipe], &oOver, &dwTmp, FALSE ) != FALSE;
#else
		if ( m_sFifoPaths[_pPipe].empty() || m_pPid <= 0 ) { return false; }
		while ( true ) {
			if ( m_abCancel ) { return false; }
			int iFd = ::open( m_sFifoPaths[_pPipe].c_str(), O_WRONLY | O_NONBLOCK | O_CLOEXEC );
			if ( iFd >= 0 ) {
#if defined( F_SETPIPE_SZ )
				::fcntl( iFd, F_SETPIPE_SZ, _pPipe == LSN_P_VIDEO ? (4 * 1024 * 1024) : (256 * 1024) );
#endif	// #if defined( F_SETPIPE_SZ )
				m_iFds[_pPipe] = iFd;
				return true;
			}
			if ( errno != ENXIO && errno != EINTR ) { return false; }
			if ( !IsRunning() ) { return false; }
			std::this_thread::sleep_for( std::chrono::milliseconds( 2 ) );
		}
#endif	// #if defined( _WIN32 )
	}

	/**
	 * Writes to a pipe, blocking until all of the data has been written.
	 *
	 * \param _pPipe The pipe.
	 * \param _pvData The data to write.
	 * \param _sSize The number of bytes to write.
	 * \return Returns true if all of the data was written; false if FFmpeg exited or stopped reading, Cancel() was called, or an error occurred.
	 */
	bool CFfmpegProcess::WritePipe( LSN_PIPE _pPipe, const void * _pvData, size_t _sSize ) {
		if ( _pPipe >= LSN_P_TOTAL ) { return false; }
		const uint8_t * pui8Src = static_cast<const uint8_t *>(_pvData);
#if defined( _WIN32 )
		if ( m_hPipes[_pPipe] == INVALID_HANDLE_VALUE || !m_hProcess ) { return false; }
		while ( _sSize ) {
			if ( ::WaitForSingleObject( m_hCancel, 0 ) == WAIT_OBJECT_0 ) { return false; }
			DWORD dwToWrite = DWORD( std::min<size_t>( _sSize, 16 * 1024 * 1024 ) );
			OVERLAPPED oOver = {};
			oOver.hEvent = m_hEvents[_pPipe];
			::ResetEvent( oOver.hEvent );
			DWORD dwWritten = 0;
			if ( !::WriteFile( m_hPipes[_pPipe], pui8Src, dwToWrite, NULL, &oOver ) ) {
				if ( ::GetLastError() != ERROR_IO_PENDING ) { return false; }
				HANDLE hWait[3] = { oOver.hEvent, m_hProcess, m_hCancel };
				DWORD dwRes = ::WaitForMultipleObjects( 3, hWait, FALSE, INFINITE );
				if ( dwRes != WAIT_OBJECT_0 ) {
					::CancelIoEx( m_hPipes[_pPipe], &oOver );
					::GetOverlappedResult( m_hPipes[_pPipe], &oOver, &dwWritten, TRUE );
					return false;
				}
			}
			if ( !::GetOverlappedResult( m_hPipes[_pPipe], &oOver, &dwWritten, FALSE ) || !dwWritten ) { return false; }
			pui8Src += dwWritten;
			_sSize -= dwWritten;
		}
		return true;
#else
		if ( m_iFds[_pPipe] < 0 ) { return false; }
		while ( _sSize ) {
			if ( m_abCancel ) { return false; }
			ssize_t sWritten = ::write( m_iFds[_pPipe], pui8Src, std::min<size_t>( _sSize, 16 * 1024 * 1024 ) );
			if ( sWritten > 0 ) {
				pui8Src += sWritten;
				_sSize -= size_t( sWritten );
				continue;
			}
			if ( sWritten < 0 && errno == EINTR ) { continue; }
			if ( sWritten < 0 && errno != EAGAIN && errno != EWOULDBLOCK ) { return false; }		// EPIPE:  FFmpeg closed its end.
			// The pipe is full.  Wait for FFmpeg to read, checking that it is still alive.
			pollfd pfdPoll = { m_iFds[_pPipe], POLLOUT, 0 };
			int iRes = ::poll( &pfdPoll, 1, 20 );
			if ( iRes < 0 && errno != EINTR ) { return false; }
			if ( iRes > 0 && (pfdPoll.revents & (POLLERR | POLLNVAL)) ) { return false; }
			if ( iRes == 0 && !IsRunning() ) { return false; }
		}
		return true;
#endif	// #if defined( _WIN32 )
	}

	/**
	 * Closes a pipe, which FFmpeg sees as the end of that input.
	 *
	 * \param _pPipe The pipe.
	 */
	void CFfmpegProcess::ClosePipe( LSN_PIPE _pPipe ) {
		if ( _pPipe >= LSN_P_TOTAL ) { return; }
#if defined( _WIN32 )
		if ( m_hPipes[_pPipe] != INVALID_HANDLE_VALUE ) {
			// Closing the server end without disconnecting lets FFmpeg read whatever is still buffered before it sees the end of the input.
			::CloseHandle( m_hPipes[_pPipe] );
			m_hPipes[_pPipe] = INVALID_HANDLE_VALUE;
		}
#else
		if ( m_iFds[_pPipe] >= 0 ) {
			::close( m_iFds[_pPipe] );
			m_iFds[_pPipe] = -1;
		}
		if ( !m_sFifoPaths[_pPipe].empty() ) {
			// FFmpeg has the FIFO open (or never will), so its name is no longer needed.
			::unlink( m_sFifoPaths[_pPipe].c_str() );
			m_sFifoPaths[_pPipe].clear();
		}
#endif	// #if defined( _WIN32 )
	}

	/**
	 * Waits for FFmpeg to exit.
	 *
	 * \param _ui32Milliseconds The maximum time to wait.
	 * \param _i32ExitCode Receives the exit code.
	 * \return Returns true if the process exited within the time limit (or was never launched, in which case _i32ExitCode is -1).
	 */
	bool CFfmpegProcess::WaitForExit( uint32_t _ui32Milliseconds, int32_t &_i32ExitCode ) {
		_i32ExitCode = -1;
#if defined( _WIN32 )
		if ( !m_hProcess ) { return true; }
		if ( ::WaitForSingleObject( m_hProcess, _ui32Milliseconds ) != WAIT_OBJECT_0 ) { return false; }
		DWORD dwCode = DWORD( -1 );
		::GetExitCodeProcess( m_hProcess, &dwCode );
		_i32ExitCode = int32_t( dwCode );
		return true;
#else
		if ( m_pPid <= 0 ) { return true; }
		if ( m_abExited ) {
			_i32ExitCode = m_i32ExitCode;
			return true;
		}
		auto aStart = std::chrono::steady_clock::now();
		while ( true ) {
			int iStatus = 0;
			pid_t pRes = ::waitpid( m_pPid, &iStatus, WNOHANG );
			if ( pRes == m_pPid ) {
				m_i32ExitCode = WIFEXITED( iStatus ) ? WEXITSTATUS( iStatus ) : (WIFSIGNALED( iStatus ) ? (128 + WTERMSIG( iStatus )) : -1);
				m_abExited = true;
				_i32ExitCode = m_i32ExitCode;
				return true;
			}
			if ( pRes < 0 && errno != EINTR ) {
				m_abExited = true;
				return true;
			}
			if ( std::chrono::steady_clock::now() - aStart >= std::chrono::milliseconds( _ui32Milliseconds ) ) { return false; }
			std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
		}
#endif	// #if defined( _WIN32 )
	}

	/**
	 * Forcibly ends FFmpeg.
	 */
	void CFfmpegProcess::Kill() {
#if defined( _WIN32 )
		if ( m_hProcess ) { ::TerminateProcess( m_hProcess, 1 ); }
#else
		if ( m_pPid > 0 && !m_abExited ) { ::kill( m_pPid, SIGKILL ); }
#endif	// #if defined( _WIN32 )
	}

	/**
	 * Makes every pending and future ConnectPipe() and WritePipe() call fail immediately.
	 */
	void CFfmpegProcess::Cancel() {
#if defined( _WIN32 )
		if ( m_hCancel ) { ::SetEvent( m_hCancel ); }
#else
		m_abCancel = true;
#endif	// #if defined( _WIN32 )
	}

	/**
	 * Determines whether FFmpeg is still running.
	 *
	 * \return Returns true if the process was launched and has not exited.
	 */
	bool CFfmpegProcess::IsRunning() const {
#if defined( _WIN32 )
		return m_hProcess && ::WaitForSingleObject( m_hProcess, 0 ) == WAIT_TIMEOUT;
#else
		if ( m_pPid <= 0 || m_abExited ) { return false; }
		return !HasExited( m_pPid );
#endif	// #if defined( _WIN32 )
	}

	/**
	 * Closes the pipes and the process handles.  If the process is still running it is left to finish on its own.
	 */
	void CFfmpegProcess::Reset() {
		for ( size_t I = 0; I < LSN_P_TOTAL; ++I ) {
			ClosePipe( LSN_PIPE( I ) );
		}
#if defined( _WIN32 )
		for ( size_t I = 0; I < LSN_P_TOTAL; ++I ) {
			if ( m_hEvents[I] ) {
				::CloseHandle( m_hEvents[I] );
				m_hEvents[I] = NULL;
			}
		}
		if ( m_hCancel ) {
			::CloseHandle( m_hCancel );
			m_hCancel = NULL;
		}
		if ( m_hProcess ) {
			::CloseHandle( m_hProcess );
			m_hProcess = NULL;
		}
#else
		if ( m_pPid > 0 && !m_abExited ) {
			// Reap it if it is done so that it does not linger as a zombie.
			int iStatus;
			if ( ::waitpid( m_pPid, &iStatus, WNOHANG ) == m_pPid ) { m_abExited = true; }
		}
		m_pPid = -1;
		m_abExited = false;
		m_i32ExitCode = -1;
		m_abCancel = false;
#endif	// #if defined( _WIN32 )
	}

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
	bool CFfmpegProcess::RunAndCapture( const std::wstring &_wsExe, const std::vector<std::wstring> &_vArgs, uint32_t _ui32TimeoutMs,
		std::string &_sOutput, int32_t &_i32ExitCode ) {
		_sOutput.clear();
		_i32ExitCode = -1;
		auto aStart = std::chrono::steady_clock::now();
		auto aTimedOut = [&]() { return std::chrono::steady_clock::now() - aStart >= std::chrono::milliseconds( _ui32TimeoutMs ); };
		char szBuffer[4096];
#if defined( _WIN32 )
		SECURITY_ATTRIBUTES saAttr = { sizeof( SECURITY_ATTRIBUTES ), NULL, TRUE };
		HANDLE hRead = NULL, hWrite = NULL;
		if ( !::CreatePipe( &hRead, &hWrite, &saAttr, 0 ) ) { return false; }
		::SetHandleInformation( hRead, HANDLE_FLAG_INHERIT, 0 );
		HANDLE hIn = OpenNul( false );
		HANDLE hProcess = NULL;
		if ( hIn != INVALID_HANDLE_VALUE ) {
			hProcess = StartProcess( _wsExe, _vArgs, hIn, hWrite, hWrite );
			::CloseHandle( hIn );
		}
		// Only the child holds the write end now, so reads end when the child exits.
		::CloseHandle( hWrite );
		if ( !hProcess ) {
			::CloseHandle( hRead );
			return false;
		}
		bool bRet = true;
		while ( true ) {
			DWORD dwAvail = 0;
			if ( !::PeekNamedPipe( hRead, NULL, 0, NULL, &dwAvail, NULL ) ) { break; }		// Broken pipe:  the child exited and everything was read.
			if ( dwAvail ) {
				DWORD dwRead = 0;
				if ( !::ReadFile( hRead, szBuffer, std::min<DWORD>( dwAvail, DWORD( sizeof( szBuffer ) ) ), &dwRead, NULL ) || !dwRead ) { break; }
				try {
					_sOutput.append( szBuffer, dwRead );
				}
				catch ( ... ) { bRet = false; break; }
				continue;
			}
			if ( aTimedOut() ) {
				bRet = false;
				break;
			}
			::WaitForSingleObject( hProcess, 5 );
		}
		if ( !bRet ) { ::TerminateProcess( hProcess, 1 ); }
		if ( ::WaitForSingleObject( hProcess, bRet ? 5000 : 1000 ) == WAIT_OBJECT_0 ) {
			DWORD dwCode = DWORD( -1 );
			::GetExitCodeProcess( hProcess, &dwCode );
			_i32ExitCode = int32_t( dwCode );
		}
		else { bRet = false; }
		::CloseHandle( hProcess );
		::CloseHandle( hRead );
		return bRet;
#else
		int iPipe[2];
		if ( ::pipe( iPipe ) != 0 ) { return false; }
		::fcntl( iPipe[0], F_SETFD, FD_CLOEXEC );
		::fcntl( iPipe[1], F_SETFD, FD_CLOEXEC );
		std::vector<std::string> vStorage;
		std::vector<char *> vArgv;
		try {
			BuildArgv( _wsExe, _vArgs, vStorage, vArgv );
		}
		catch ( ... ) {
			::close( iPipe[0] );
			::close( iPipe[1] );
			return false;
		}
		posix_spawn_file_actions_t pfaActions;
		if ( ::posix_spawn_file_actions_init( &pfaActions ) != 0 ) {
			::close( iPipe[0] );
			::close( iPipe[1] );
			return false;
		}
		bool bOk = ::posix_spawn_file_actions_addopen( &pfaActions, 0, "/dev/null", O_RDONLY, 0 ) == 0 &&
			::posix_spawn_file_actions_adddup2( &pfaActions, iPipe[1], 1 ) == 0 &&
			::posix_spawn_file_actions_adddup2( &pfaActions, iPipe[1], 2 ) == 0;
		pid_t pPid = -1;
		if ( bOk ) {
			bOk = ::posix_spawn( &pPid, vArgv[0], &pfaActions, nullptr, vArgv.data(), environ ) == 0;
		}
		::posix_spawn_file_actions_destroy( &pfaActions );
		::close( iPipe[1] );
		if ( !bOk ) {
			::close( iPipe[0] );
			return false;
		}
		bool bRet = true;
		while ( true ) {
			pollfd pfdPoll = { iPipe[0], POLLIN, 0 };
			int iRes = ::poll( &pfdPoll, 1, 10 );
			if ( iRes < 0 && errno != EINTR ) { break; }
			if ( iRes > 0 ) {
				ssize_t sRead = ::read( iPipe[0], szBuffer, sizeof( szBuffer ) );
				if ( sRead > 0 ) {
					try {
						_sOutput.append( szBuffer, size_t( sRead ) );
					}
					catch ( ... ) { bRet = false; break; }
					continue;
				}
				if ( sRead == 0 ) { break; }			// The child exited.
				if ( errno != EINTR && errno != EAGAIN ) { break; }
			}
			if ( aTimedOut() ) {
				bRet = false;
				break;
			}
		}
		::close( iPipe[0] );
		if ( !bRet ) { ::kill( pPid, SIGKILL ); }
		int iStatus = 0;
		while ( ::waitpid( pPid, &iStatus, 0 ) < 0 && errno == EINTR ) {}
		_i32ExitCode = WIFEXITED( iStatus ) ? WEXITSTATUS( iStatus ) : -1;
		return bRet;
#endif	// #if defined( _WIN32 )
	}

	/**
	 * Finds the FFmpeg executable.  The folder containing BeesNES is checked first (and its ffmpeg/bin sub-folder), then the system
	 *	search path.
	 *
	 * \return Returns the full path to FFmpeg or an empty string if it could not be found.
	 */
	std::wstring CFfmpegProcess::FindFfmpeg() {
		try {
			std::wstring wsExeFolder = ExeFolder();
#if defined( _WIN32 )
			for ( const wchar_t * pwcSub : { L"ffmpeg.exe", L"ffmpeg\\bin\\ffmpeg.exe", L"ffmpeg\\ffmpeg.exe" } ) {
				std::wstring wsPath = wsExeFolder + pwcSub;
				DWORD dwAttr = ::GetFileAttributesW( wsPath.c_str() );
				if ( dwAttr != INVALID_FILE_ATTRIBUTES && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY) ) { return wsPath; }
			}
			wchar_t wszPath[MAX_PATH * 4];
			DWORD dwLen = ::SearchPathW( NULL, L"ffmpeg.exe", NULL, DWORD( std::size( wszPath ) ), wszPath, NULL );
			if ( dwLen && dwLen < std::size( wszPath ) ) { return std::wstring( wszPath, dwLen ); }
#else
			std::vector<std::string> vCandidates;
			std::string sExeFolder = ToUtf8( wsExeFolder );
			if ( !sExeFolder.empty() ) {
				vCandidates.push_back( sExeFolder + "ffmpeg" );
				vCandidates.push_back( sExeFolder + "ffmpeg/bin/ffmpeg" );
			}
			if ( const char * pcPath = std::getenv( "PATH" ) ) {
				std::string sPath = pcPath;
				size_t sStart = 0;
				while ( sStart <= sPath.size() ) {
					size_t sEnd = sPath.find( ':', sStart );
					if ( sEnd == std::string::npos ) { sEnd = sPath.size(); }
					if ( sEnd > sStart ) { vCandidates.push_back( sPath.substr( sStart, sEnd - sStart ) + "/ffmpeg" ); }
					sStart = sEnd + 1;
				}
			}
			// Applications started from the Finder do not get the shell's PATH.
			vCandidates.push_back( "/opt/homebrew/bin/ffmpeg" );
			vCandidates.push_back( "/usr/local/bin/ffmpeg" );
			vCandidates.push_back( "/usr/bin/ffmpeg" );
			for ( size_t I = 0; I < vCandidates.size(); ++I ) {
				struct stat sStat;
				if ( ::stat( vCandidates[I].c_str(), &sStat ) == 0 && S_ISREG( sStat.st_mode ) && ::access( vCandidates[I].c_str(), X_OK ) == 0 ) {
					return FromUtf8( vCandidates[I] );
				}
			}
#endif	// #if defined( _WIN32 )
		}
		catch ( ... ) {}
		return std::wstring();
	}

	/**
	 * Finds the FFmpeg executable given the path set by the user.  An empty path searches for FFmpeg (see FindFfmpeg()).  A folder is
	 *	searched for FFmpeg (and so is its bin sub-folder).  A file is used as-is if it exists.
	 *
	 * \param _wsPath The path set by the user.
	 * \return Returns the full path to FFmpeg or an empty string if it could not be found.
	 */
	std::wstring CFfmpegProcess::ResolveFfmpeg( const std::wstring &_wsPath ) {
		if ( _wsPath.empty() ) { return FindFfmpeg(); }
		try {
			std::error_code ecErr;
			std::filesystem::path pPath( _wsPath );
			if ( std::filesystem::is_directory( pPath, ecErr ) ) {
#if defined( _WIN32 )
				const wchar_t * pwcExe = L"ffmpeg.exe";
#else
				const wchar_t * pwcExe = L"ffmpeg";
#endif	// #if defined( _WIN32 )
				for ( const std::filesystem::path &pCandidate : { pPath / pwcExe, pPath / L"bin" / pwcExe } ) {
					if ( std::filesystem::is_regular_file( pCandidate, ecErr ) ) { return pCandidate.wstring(); }
				}
				return std::wstring();
			}
			if ( std::filesystem::is_regular_file( pPath, ecErr ) ) { return pPath.wstring(); }
		}
		catch ( ... ) {}
		return std::wstring();
	}

	/**
	 * Converts a wide string to UTF-8.
	 *
	 * \param _wsString The string to convert.
	 * \return Returns the UTF-8 string.
	 */
	std::string CFfmpegProcess::ToUtf8( const std::wstring &_wsString ) {
		std::string sRet;
		sRet.reserve( _wsString.size() );
		for ( size_t I = 0; I < _wsString.size(); ++I ) {
			uint32_t ui32Cp = uint32_t( _wsString[I] );
			if constexpr ( sizeof( wchar_t ) == 2 ) {
				// UTF-16 surrogate pair.
				if ( ui32Cp >= 0xD800 && ui32Cp <= 0xDBFF && I + 1 < _wsString.size() ) {
					uint32_t ui32Lo = uint32_t( _wsString[I+1] );
					if ( ui32Lo >= 0xDC00 && ui32Lo <= 0xDFFF ) {
						ui32Cp = 0x10000 + ((ui32Cp - 0xD800) << 10) + (ui32Lo - 0xDC00);
						++I;
					}
				}
			}
			if ( ui32Cp < 0x80 ) { sRet += char( ui32Cp ); }
			else if ( ui32Cp < 0x800 ) {
				sRet += char( 0xC0 | (ui32Cp >> 6) );
				sRet += char( 0x80 | (ui32Cp & 0x3F) );
			}
			else if ( ui32Cp < 0x10000 ) {
				sRet += char( 0xE0 | (ui32Cp >> 12) );
				sRet += char( 0x80 | ((ui32Cp >> 6) & 0x3F) );
				sRet += char( 0x80 | (ui32Cp & 0x3F) );
			}
			else {
				sRet += char( 0xF0 | (ui32Cp >> 18) );
				sRet += char( 0x80 | ((ui32Cp >> 12) & 0x3F) );
				sRet += char( 0x80 | ((ui32Cp >> 6) & 0x3F) );
				sRet += char( 0x80 | (ui32Cp & 0x3F) );
			}
		}
		return sRet;
	}

	/**
	 * Converts a UTF-8 string to a wide string.
	 *
	 * \param _sString The string to convert.
	 * \return Returns the wide string.
	 */
	std::wstring CFfmpegProcess::FromUtf8( const std::string &_sString ) {
		std::wstring wsRet;
		wsRet.reserve( _sString.size() );
		for ( size_t I = 0; I < _sString.size(); ) {
			uint8_t ui8C = uint8_t( _sString[I] );
			uint32_t ui32Cp;
			size_t sLen;
			if ( ui8C < 0x80 ) { ui32Cp = ui8C; sLen = 1; }
			else if ( (ui8C & 0xE0) == 0xC0 ) { ui32Cp = ui8C & 0x1F; sLen = 2; }
			else if ( (ui8C & 0xF0) == 0xE0 ) { ui32Cp = ui8C & 0x0F; sLen = 3; }
			else if ( (ui8C & 0xF8) == 0xF0 ) { ui32Cp = ui8C & 0x07; sLen = 4; }
			else { ui32Cp = 0xFFFD; sLen = 1; }
			if ( I + sLen > _sString.size() ) {
				ui32Cp = 0xFFFD;
				sLen = 1;
			}
			else {
				for ( size_t J = 1; J < sLen; ++J ) {
					ui32Cp = (ui32Cp << 6) | (uint8_t( _sString[I+J] ) & 0x3F);
				}
			}
			I += sLen;
			if constexpr ( sizeof( wchar_t ) == 2 ) {
				if ( ui32Cp >= 0x10000 ) {
					ui32Cp -= 0x10000;
					wsRet += wchar_t( 0xD800 + (ui32Cp >> 10) );
					wsRet += wchar_t( 0xDC00 + (ui32Cp & 0x3FF) );
					continue;
				}
			}
			wsRet += wchar_t( ui32Cp );
		}
		return wsRet;
	}

	/**
	 * Gets the folder containing the running executable, including the trailing separator.
	 *
	 * \return Returns the folder containing the running executable.
	 */
	std::wstring CFfmpegProcess::ExeFolder() {
#if defined( _WIN32 )
		std::wstring wsBuffer;
		wsBuffer.resize( 0xFFFF + 1 );
		DWORD dwLen = ::GetModuleFileNameW( NULL, wsBuffer.data(), 0xFFFF );
		wsBuffer.resize( dwLen );
		size_t sPos = wsBuffer.find_last_of( L"\\/" );
		return sPos == std::wstring::npos ? std::wstring() : wsBuffer.substr( 0, sPos + 1 );
#else
		char szBuffer[PATH_MAX];
		std::string sPath;
#if defined( __APPLE__ )
		uint32_t ui32Size = sizeof( szBuffer );
		if ( ::_NSGetExecutablePath( szBuffer, &ui32Size ) == 0 ) {
			char szReal[PATH_MAX];
			sPath = ::realpath( szBuffer, szReal ) ? szReal : szBuffer;
		}
#elif defined( __linux__ )
		ssize_t sLen = ::readlink( "/proc/self/exe", szBuffer, sizeof( szBuffer ) - 1 );
		if ( sLen > 0 ) {
			szBuffer[sLen] = '\0';
			sPath = szBuffer;
		}
#endif	// #if defined( __APPLE__ )
		size_t sPos = sPath.find_last_of( '/' );
		return sPos == std::string::npos ? std::wstring() : FromUtf8( sPath.substr( 0, sPos + 1 ) );
#endif	// #if defined( _WIN32 )
	}

}	// namespace lsn
