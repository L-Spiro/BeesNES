#ifdef LSN_DX12

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Loads HLSL #include files from disk for D3DCompile().
 */

#pragma once

#include "../../LSNLSpiroNes.h"

#include <cstdint>
#include <d3dcompiler.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace lsn {

	/**
	 * Loads HLSL #include files from disk for D3DCompile().
	 *
	 * \brief Rooted at a directory (typically <ExeDir>\Shaders) and supports nested includes relative
	 * to the including file, with fallback to the root directory.
	 */
	class CDirectX12DiskInclude final : public ID3DInclude {
	public :
		/**
		 * Creates a disk-based include handler.
		 *
		 * \brief The root directory is used when an include cannot be resolved relative to the parent.
		 *
		 * \param _pRoot The root directory for include resolution.
		 */
		explicit											CDirectX12DiskInclude( const std::filesystem::path &_pRoot ) :
			m_pRoot( _pRoot ) {
		}

		/**
		 * Destroys the include handler.
		 *
		 * \brief Any outstanding include buffers should have been closed by the compiler.
		 */
		~CDirectX12DiskInclude() = default;


		// == Functions.
		/**
		 * Opens an include file and returns its contents in memory.
		 *
		 * \brief The returned buffer must remain valid until Close() is called by the compiler.
		 *
		 * \param _itIncludeType The include type (local/system). Unused.
		 * \param _pcFileName The include file name.
		 * \param _pvParentData The parent include buffer pointer (used for relative path resolution).
		 * \param _ppvData Receives the include file contents pointer.
		 * \param _puiBytes Receives the include file size in bytes.
		 * \return Returns S_OK on success; otherwise an HRESULT error code.
		 */
		STDMETHOD( Open )(
			D3D_INCLUDE_TYPE /*_itIncludeType*/,
			LPCSTR _pcFileName,
			LPCVOID _pvParentData,
			LPCVOID * _ppvData,
			UINT * _puiBytes ) override {

			if ( !_pcFileName || !_ppvData || !_puiBytes ) { return E_INVALIDARG; }
			(*_ppvData) = nullptr;
			(*_puiBytes) = 0;

			std::filesystem::path pIncName;
			try {
				pIncName = std::filesystem::path( ToWide( _pcFileName ) );
			}
			catch ( const std::bad_alloc & ) {
				return E_OUTOFMEMORY;
			}

			const std::filesystem::path pParentDir = ParentDirFromData( _pvParentData );

			// Try relative to the including file first, then fall back to root.
			std::filesystem::path pTry = pParentDir / pIncName;

			std::error_code ecErr;
			if ( !std::filesystem::exists( pTry, ecErr ) || ecErr ) {
				ecErr.clear();
				pTry = m_pRoot / pIncName;
				if ( !std::filesystem::exists( pTry, ecErr ) || ecErr ) {
					return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );
				}
			}

			ecErr.clear();
			const uintmax_t uiMaxSize = std::filesystem::file_size( pTry, ecErr );
			if ( ecErr || !uiMaxSize ) { return E_FAIL; }

			LSN_INCLUDE_DATA idData;
			try {
				idData.vData.resize( static_cast<size_t>(uiMaxSize) );
			}
			catch ( const std::bad_alloc & ) {
				return E_OUTOFMEMORY;
			}

			std::ifstream iFile( pTry, std::ios::binary );
			if ( !iFile.good() ) { return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ); }

			iFile.read( reinterpret_cast<char *>(idData.vData.data()), std::streamsize( idData.vData.size() ) );
			if ( !iFile.good() ) { return E_FAIL; }

			idData.pDir = pTry.parent_path();

			const void * pvKey = idData.vData.data();
			m_mIncludes.emplace( pvKey, std::move( idData ) );

			(*_ppvData) = pvKey;
			(*_puiBytes) = static_cast<UINT>(uiMaxSize);
			return S_OK;
		}

		/**
		 * Closes an include file and frees any associated memory.
		 *
		 * \brief Only buffers allocated by Open() are freed here.
		 *
		 * \param _pvData The buffer pointer previously returned by Open().
		 * \return Returns S_OK.
		 */
		STDMETHOD( Close )( LPCVOID _pvData ) override {
			auto aIt = m_mIncludes.find( _pvData );
			if ( aIt != m_mIncludes.end() ) {
				m_mIncludes.erase( aIt );
			}
			return S_OK;
		}

		/**
		 * Gets the shaders directory located next to the executable: <ExeDir>\Shaders.
		 *
		 * \brief Does not depend on the process working directory.
		 *
		 * \return Returns the full path to the shaders directory.
		 */
		static inline std::filesystem::path				GetExeShadersDir() {
			std::wstring wsBuffer;
			const DWORD dwSize = 0xFFFF;
			wsBuffer.resize( dwSize + 1 );
			const DWORD dwLen = ::GetModuleFileNameW( NULL, wsBuffer.data(), dwSize );
			if ( !dwLen || dwLen >= dwSize ) {
				return std::filesystem::current_path() / L"Shaders";
			}
			return std::filesystem::path( wsBuffer ).parent_path() / L"Shaders";
		}

	private :
		// == Types.
		/** Data for #include's. */
		struct LSN_INCLUDE_DATA {
			std::vector<uint8_t>						vData;							/**< File bytes returned to the compiler. */
			std::filesystem::path						pDir;							/**< Directory of the including file. */
		};


		// == Members.
		std::filesystem::path							m_pRoot;						/**< Root include directory. */
		std::unordered_map<LPCVOID, LSN_INCLUDE_DATA>	m_mIncludes;					/**< Live include storage (stable backing memory). */


		// == Functions.
		/**
		 * Converts a UTF-8 (preferred) or ANSI string to UTF-16.
		 *
		 * \brief Used to interpret include file names.
		 *
		 * \param _pcStr The input string.
		 * \return Returns the converted UTF-16 string.
		 */
		static std::wstring								ToWide( const char * _pcStr ) {
			if ( !_pcStr ) { return std::wstring(); }

			// Try UTF-8 first.
			int iLen = ::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, _pcStr, -1, nullptr, 0 );
			if ( iLen > 0 ) {
				std::wstring wsStr;
				wsStr.resize( static_cast<size_t>(iLen) );
				::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, _pcStr, -1, wsStr.data(), iLen );
				if ( !wsStr.empty() && wsStr.back() == L'\0' ) { wsStr.pop_back(); }
				return wsStr;
			}

			// Fallback to ANSI codepage.
			iLen = ::MultiByteToWideChar( CP_ACP, 0, _pcStr, -1, nullptr, 0 );
			if ( iLen <= 0 ) { return std::wstring(); }

			std::wstring wsStr;
			wsStr.resize( static_cast<size_t>(iLen) );
			::MultiByteToWideChar( CP_ACP, 0, _pcStr, -1, wsStr.data(), iLen );
			if ( !wsStr.empty() && wsStr.back() == L'\0' ) { wsStr.pop_back(); }
			return wsStr;
		}

		/**
		 * Determines the parent directory for resolving nested includes.
		 *
		 * \brief Uses the directory associated with the parent include buffer when available.
		 *
		 * \param _pvParentData The parent include buffer pointer.
		 * \return Returns the directory to search first for the next include.
		 */
		std::filesystem::path							ParentDirFromData( LPCVOID _pvParentData ) const {
			if ( _pvParentData ) {
				auto aIt = m_mIncludes.find( _pvParentData );
				if ( aIt != m_mIncludes.end() ) { return aIt->second.pDir; }
			}
			return m_pRoot;
		}
	};

}	// namespace lsn

#endif	// #ifdef LSN_DX12
