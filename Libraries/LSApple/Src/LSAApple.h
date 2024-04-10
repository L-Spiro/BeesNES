/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: General Apple (macOS/tvOS/iOS) support.
 */


#pragma once

#ifdef __APPLE__

#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CoreFoundation.h>

namespace lsa {

	struct LSA_STRINGREF {
		~LSA_STRINGREF() {
			if ( NULL != srRef ) {
				::CFRelease( srRef );
				srRef = NULL;
			}
		}
		
		
		// == Functions.
		/**
		 * Converts the string reference to an std::u8string object.  Should be done inside try/catch to catch the string-allocation error.
		 *
		 * \return Returns the string converted to std::u8string.
		 **/
		std::u8string				ToString_u8() const {
			if ( NULL == srRef ) { return {}; }

			// Determine the length of the buffer needed for the conversion.
			CFIndex length = ::CFStringGetLength( srRef );
			CFIndex maxSize = ::CFStringGetMaximumSizeForEncoding( length, kCFStringEncodingUTF8 ) + 1;

			// Allocate the buffer for UTF-8 characters.
			std::string utf8String( maxSize, '\0' );

			// Perform the conversion
			if ( ::CFStringGetCString( srRef, &utf8String[0], maxSize, kCFStringEncodingUTF8 ) ) {
				// Resize the std::string to the actual size used.
				utf8String.resize( std::strlen( utf8String.c_str() ) );
				return std::u8string( reinterpret_cast<const char8_t *>(utf8String.data()) );
			}
			// Conversion failed.
			return {};
		}
		
		
		// == Members.
		CFStringRef					srRef = NULL;					/**< The object we manage. */
	};
	
}	// namespace lsa

#endif	// #ifdef __APPLE__

