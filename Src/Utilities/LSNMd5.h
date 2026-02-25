/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A class for generating MD5 hashes.
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <format>
#include <vector>


namespace lsn {

	/**
	 * Class CMd5
	 * \brief A class for generating MD5 hashes.
	 *
	 * Description: Computes the MD5 hash of a stream of data. Includes a stateful interface for streaming chunks of data.
	 */
	class CMd5 {
	public :
		// == Types.
		/**
		 * Structure LSN_MD5_HASH
		 * \brief Represents a 128-bit MD5 hash.
		 */
		struct LSN_MD5_HASH {
			uint8_t												ui8Bytes[16];
		};


		// == Functions.
		/**
		 * Computes the MD5 hash of a single contiguous buffer.
		 *
		 * \param _pui8Data Pointer to the data to hash.
		 * \param _sLen Length of the data in bytes.
		 * \return Returns the computed MD5 hash.
		 */
		static LSN_MD5_HASH										Compute( const uint8_t * _pui8Data, size_t _sLen ) {
			CMd5 mMd5;
			mMd5.Init();
			mMd5.Update( _pui8Data, _sLen );
			return mMd5.Final();
		}

		/**
		 * Initializes or resets the MD5 context.
		 */
		void													Init() {
			m_ui32Count[0] = m_ui32Count[1] = 0;
			
			m_ui32State[0] = 0x67452301;
			m_ui32State[1] = 0xEFCDAB89;
			m_ui32State[2] = 0x98BADCFE;
			m_ui32State[3] = 0x10325476;
		}

		/**
		 * Updates the MD5 context with a block of data.
		 *
		 * \param _pui8Data Pointer to the data to hash.
		 * \param _sLen Length of the data in bytes.
		 */
		void													Update( const uint8_t * _pui8Data, size_t _sLen ) {
			// Compute number of bytes mod 64.
			uint32_t ui32Index = static_cast<uint32_t>((m_ui32Count[0] >> 3) & 0x3F);

			// Update number of bits.
			if ( (m_ui32Count[0] += static_cast<uint32_t>(_sLen << 3)) < static_cast<uint32_t>(_sLen << 3) ) {
				m_ui32Count[1]++;
			}
			m_ui32Count[1] += static_cast<uint32_t>(_sLen >> 29);

			uint32_t ui32PartLen = 64 - ui32Index;
			uint32_t I = 0;

			// Transform as many times as possible.
			if ( _sLen >= ui32PartLen ) {
				std::memcpy( &m_ui8Buffer[ui32Index], _pui8Data, ui32PartLen );
				Transform( m_ui32State, m_ui8Buffer );

				for ( I = ui32PartLen; I + 63 < _sLen; I += 64 ) {
					Transform( m_ui32State, &_pui8Data[I] );
				}
				ui32Index = 0;
			}

			// Buffer remaining input.
			std::memcpy( &m_ui8Buffer[ui32Index], &_pui8Data[I], _sLen - I );
		}

		/**
		 * Finalizes the MD5 context and returns the hash.
		 *
		 * \return Returns the computed MD5 hash.
		 */
		LSN_MD5_HASH											Final() {
			uint8_t ui8Bits[8];
			Encode( ui8Bits, m_ui32Count, 8 );

			// Pad out to 56 mod 64.
			uint32_t ui32Index = static_cast<uint32_t>((m_ui32Count[0] >> 3) & 0x3F);
			uint32_t ui32PadLen = (ui32Index < 56) ? (56 - ui32Index) : (120 - ui32Index);
			
			static const uint8_t ui8Padding[64] = { 0x80 }; // Remaining bytes default initialized to 0.

			Update( ui8Padding, ui32PadLen );
			
			// Append length (before padding).
			Update( ui8Bits, 8 );

			LSN_MD5_HASH hHash;
			Encode( hHash.ui8Bytes, m_ui32State, 16 );

			// Clear context.
			Init();

			return hHash;
		}

		/**
		 * Print a hash to a string.  Call within a try/catch block.
		 * 
		 * \param _mhHash The hash to print.
		 * \return Returns the printed string.
		 **/
		template <typename _tOutType>
		static _tOutType										ToString( const LSN_MD5_HASH &_mhHash ) {
			_tOutType otOut;
			otOut.reserve( 32 );
			if constexpr ( std::is_same_v<_tOutType, std::string> ) {
				static const char pcHex[] = "0123456789abcdef";
				for ( size_t I = 0; I < std::size( _mhHash.ui8Bytes ); ++I ) {
					otOut.push_back( pcHex[_mhHash.ui8Bytes[I] >> 4] );
					otOut.push_back( pcHex[_mhHash.ui8Bytes[I] & 0x0F] );
				}
			}
			else if constexpr ( std::is_same_v<_tOutType, std::wstring> ) {
				static const wchar_t pwcHex[] = L"0123456789abcdef";
				for ( size_t I = 0; I < std::size( _mhHash.ui8Bytes ); ++I ) {
					otOut.push_back( pwcHex[_mhHash.ui8Bytes[I] >> 4] );
					otOut.push_back( pwcHex[_mhHash.ui8Bytes[I] & 0x0F] );
				}
			}
			else if constexpr ( std::is_same_v<_tOutType, std::u8string> ) {
				static const char8_t pu8cHex[] = u8"0123456789abcdef";
				for ( size_t I = 0; I < std::size( _mhHash.ui8Bytes ); ++I ) {
					otOut.push_back( pu8cHex[_mhHash.ui8Bytes[I] >> 4] );
					otOut.push_back( pu8cHex[_mhHash.ui8Bytes[I] & 0x0F] );
				}
			}
			else if constexpr ( std::is_same_v<_tOutType, std::u16string> ) {
				static const char16_t pu16cHex[] = u"0123456789abcdef";
				for ( size_t I = 0; I < std::size( _mhHash.ui8Bytes ); ++I ) {
					otOut.push_back( pu16cHex[_mhHash.ui8Bytes[I] >> 4] );
					otOut.push_back( pu16cHex[_mhHash.ui8Bytes[I] & 0x0F] );
				}
			}
			return otOut;
		}


	protected :
		// == Members.
		uint32_t												m_ui32State[4];						/**< State (ABCD). */
		uint32_t												m_ui32Count[2];						/**< Number of bits, modulo 2^64 (lsb first). */
		uint8_t													m_ui8Buffer[64];					/**< Input buffer. */


		// == Functions.
		/**
		 * MD5 basic transformation. Transforms state based on block.
		 *
		 * \param _pui32State The state to update.
		 * \param _pui8Block The 64-byte block.
		 */
		static void												Transform( uint32_t _pui32State[4], const uint8_t _pui8Block[64] ) {
			uint32_t A = _pui32State[0], B = _pui32State[1], C = _pui32State[2], D = _pui32State[3];
			uint32_t X[16];

			Decode( X, _pui8Block, 64 );

			// Round 1
			FF( A, B, C, D, X[ 0],  7, 0xD76AA478 );
			FF( D, A, B, C, X[ 1], 12, 0xE8C7B756 );
			FF( C, D, A, B, X[ 2], 17, 0x242070DB );
			FF( B, C, D, A, X[ 3], 22, 0xC1BDCEEE );
			FF( A, B, C, D, X[ 4],  7, 0xF57C0FAF );
			FF( D, A, B, C, X[ 5], 12, 0x4787C62A );
			FF( C, D, A, B, X[ 6], 17, 0xA8304613 );
			FF( B, C, D, A, X[ 7], 22, 0xFD469501 );
			FF( A, B, C, D, X[ 8],  7, 0x698098D8 );
			FF( D, A, B, C, X[ 9], 12, 0x8B44F7AF );
			FF( C, D, A, B, X[10], 17, 0xFFFF5BB1 );
			FF( B, C, D, A, X[11], 22, 0x895CD7BE );
			FF( A, B, C, D, X[12],  7, 0x6B901122 );
			FF( D, A, B, C, X[13], 12, 0xFD987193 );
			FF( C, D, A, B, X[14], 17, 0xA679438E );
			FF( B, C, D, A, X[15], 22, 0x49B40821 );

			// Round 2
			GG( A, B, C, D, X[ 1],  5, 0xF61E2562 );
			GG( D, A, B, C, X[ 6],  9, 0xC040B340 );
			GG( C, D, A, B, X[11], 14, 0x265E5A51 );
			GG( B, C, D, A, X[ 0], 20, 0xE9B6C7AA );
			GG( A, B, C, D, X[ 5],  5, 0xD62F105D );
			GG( D, A, B, C, X[10],  9, 0x02441453 );
			GG( C, D, A, B, X[15], 14, 0xD8A1E681 );
			GG( B, C, D, A, X[ 4], 20, 0xE7D3FBC8 );
			GG( A, B, C, D, X[ 9],  5, 0x21E1CDE6 );
			GG( D, A, B, C, X[14],  9, 0xC33707D6 );
			GG( C, D, A, B, X[ 3], 14, 0xF4D50D87 );
			GG( B, C, D, A, X[ 8], 20, 0x455A14ED );
			GG( A, B, C, D, X[13],  5, 0xA9E3E905 );
			GG( D, A, B, C, X[ 2],  9, 0xFCEFA3F8 );
			GG( C, D, A, B, X[ 7], 14, 0x676F02D9 );
			GG( B, C, D, A, X[12], 20, 0x8D2A4C8A );

			// Round 3
			HH( A, B, C, D, X[ 5],  4, 0xFFFA3942 );
			HH( D, A, B, C, X[ 8], 11, 0x8771F681 );
			HH( C, D, A, B, X[11], 16, 0x6D9D6122 );
			HH( B, C, D, A, X[14], 23, 0xFDE5380C );
			HH( A, B, C, D, X[ 1],  4, 0xA4BEEA44 );
			HH( D, A, B, C, X[ 4], 11, 0x4BDECFA9 );
			HH( C, D, A, B, X[ 7], 16, 0xF6BB4B60 );
			HH( B, C, D, A, X[10], 23, 0xBEBFBC70 );
			HH( A, B, C, D, X[13],  4, 0x289B7EC6 );
			HH( D, A, B, C, X[ 0], 11, 0xEAA127FA );
			HH( C, D, A, B, X[ 3], 16, 0xD4EF3085 );
			HH( B, C, D, A, X[ 6], 23, 0x04881D05 );
			HH( A, B, C, D, X[ 9],  4, 0xD9D4D039 );
			HH( D, A, B, C, X[12], 11, 0xE6DB99E5 );
			HH( C, D, A, B, X[15], 16, 0x1FA27CF8 );
			HH( B, C, D, A, X[ 2], 23, 0xC4AC5665 );

			// Round 4
			II( A, B, C, D, X[ 0],  6, 0xF4292244 );
			II( D, A, B, C, X[ 7], 10, 0x432AFF97 );
			II( C, D, A, B, X[14], 15, 0xAB9423A7 );
			II( B, C, D, A, X[ 5], 21, 0xFC93A039 );
			II( A, B, C, D, X[12],  6, 0x655B59C3 );
			II( D, A, B, C, X[ 3], 10, 0x8F0CCC92 );
			II( C, D, A, B, X[10], 15, 0xFFEFF47D );
			II( B, C, D, A, X[ 1], 21, 0x85845DD1 );
			II( A, B, C, D, X[ 8],  6, 0x6FA87E4F );
			II( D, A, B, C, X[15], 10, 0xFE2CE6E0 );
			II( C, D, A, B, X[ 6], 15, 0xA3014314 );
			II( B, C, D, A, X[13], 21, 0x4E0811A1 );
			II( A, B, C, D, X[ 4],  6, 0xF7537E82 );
			II( D, A, B, C, X[11], 10, 0xBD3AF235 );
			II( C, D, A, B, X[ 2], 15, 0x2AD7D2BB );
			II( B, C, D, A, X[ 9], 21, 0xEB86D391 );

			_pui32State[0] += A;
			_pui32State[1] += B;
			_pui32State[2] += C;
			_pui32State[3] += D;
		}

		/**
		 * Encodes input (uint32_t) into output (uint8_t). Assumes length is a multiple of 4.
		 *
		 * \param _pui8Output The destination byte array.
		 * \param _pui32Input The source 32-bit integer array.
		 * \param _ui32Len The number of bytes to encode.
		 */
		static void												Encode( uint8_t * _pui8Output, const uint32_t * _pui32Input, uint32_t _ui32Len ) {
			for ( uint32_t I = 0, J = 0; J < _ui32Len; I++, J += 4 ) {
				_pui8Output[J] = static_cast<uint8_t>(_pui32Input[I] & 0xFF);
				_pui8Output[J + 1] = static_cast<uint8_t>((_pui32Input[I] >> 8) & 0xFF);
				_pui8Output[J + 2] = static_cast<uint8_t>((_pui32Input[I] >> 16) & 0xFF);
				_pui8Output[J + 3] = static_cast<uint8_t>((_pui32Input[I] >> 24) & 0xFF);
			}
		}

		/**
		 * Decodes input (uint8_t) into output (uint32_t). Assumes length is a multiple of 4.
		 *
		 * \param _pui32Output The destination 32-bit integer array.
		 * \param _pui8Input The source byte array.
		 * \param _ui32Len The number of bytes to decode.
		 */
		static void												Decode( uint32_t * _pui32Output, const uint8_t * _pui8Input, uint32_t _ui32Len ) {
			for ( uint32_t I = 0, J = 0; J < _ui32Len; I++, J += 4 ) {
				_pui32Output[I] = (static_cast<uint32_t>(_pui8Input[J])) |
								  ((static_cast<uint32_t>(_pui8Input[J + 1])) << 8) |
								  ((static_cast<uint32_t>(_pui8Input[J + 2])) << 16) |
								  ((static_cast<uint32_t>(_pui8Input[J + 3])) << 24);
			}
		}


		// == MD5 Core Operations.
		static inline uint32_t									RotateLeft( uint32_t _ui32X, uint32_t _ui32N ) {
			return (_ui32X << _ui32N) | (_ui32X >> (32 - _ui32N));
		}
		
		static inline uint32_t									F( uint32_t _ui32X, uint32_t _ui32Y, uint32_t _ui32Z ) { return (_ui32X & _ui32Y) | (~_ui32X & _ui32Z); }
		static inline uint32_t									G( uint32_t _ui32X, uint32_t _ui32Y, uint32_t _ui32Z ) { return (_ui32X & _ui32Z) | (_ui32Y & ~_ui32Z); }
		static inline uint32_t									H( uint32_t _ui32X, uint32_t _ui32Y, uint32_t _ui32Z ) { return _ui32X ^ _ui32Y ^ _ui32Z; }
		static inline uint32_t									I( uint32_t _ui32X, uint32_t _ui32Y, uint32_t _ui32Z ) { return _ui32Y ^ (_ui32X | ~_ui32Z); }

		static inline void										FF( uint32_t &_ui32A, uint32_t _ui32B, uint32_t _ui32C, uint32_t _ui32D, uint32_t _ui32X, uint32_t _ui32S, uint32_t _ui32Ac ) {
			_ui32A += F( _ui32B, _ui32C, _ui32D ) + _ui32X + _ui32Ac;
			_ui32A = RotateLeft( _ui32A, _ui32S );
			_ui32A += _ui32B;
		}
		
		static inline void										GG( uint32_t &_ui32A, uint32_t _ui32B, uint32_t _ui32C, uint32_t _ui32D, uint32_t _ui32X, uint32_t _ui32S, uint32_t _ui32Ac ) {
			_ui32A += G( _ui32B, _ui32C, _ui32D ) + _ui32X + _ui32Ac;
			_ui32A = RotateLeft( _ui32A, _ui32S );
			_ui32A += _ui32B;
		}
		
		static inline void										HH( uint32_t &_ui32A, uint32_t _ui32B, uint32_t _ui32C, uint32_t _ui32D, uint32_t _ui32X, uint32_t _ui32S, uint32_t _ui32Ac ) {
			_ui32A += H( _ui32B, _ui32C, _ui32D ) + _ui32X + _ui32Ac;
			_ui32A = RotateLeft( _ui32A, _ui32S );
			_ui32A += _ui32B;
		}
		
		static inline void										II( uint32_t &_ui32A, uint32_t _ui32B, uint32_t _ui32C, uint32_t _ui32D, uint32_t _ui32X, uint32_t _ui32S, uint32_t _ui32Ac ) {
			_ui32A += I( _ui32B, _ui32C, _ui32D ) + _ui32X + _ui32Ac;
			_ui32A = RotateLeft( _ui32A, _ui32S );
			_ui32A += _ui32B;
		}
	};

}	// namespace lsn