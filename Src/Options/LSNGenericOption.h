/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A generic option that can be of any type.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Utilities/LSNUtilities.h"



namespace lsn {

	/** Enumerations. */
	enum LSN_OPTION_TYPE {
		LSN_OT_INT8,										/**< An 8-bit integer. */
		LSN_OT_INT16,										/**< A 16-bit integer. */
		LSN_OT_INT32,										/**< A 32-bit integer. */
		LSN_OT_INT64,										/**< A 64-bit integer. */
		LSN_OT_UINT8,										/**< An unsigned 8-bit integer. */
		LSN_OT_UINT16,										/**< An unsigned 16-bit integer. */
		LSN_OT_UINT32,										/**< An unsigned 32-bit integer. */
		LSN_OT_UINT64,										/**< An unsigned 64-bit integer. */
		LSN_OT_FLOAT,										/**< A 32-bit float. */
		LSN_OT_DOUBLE,										/**< A 64-bit float. */
		LSN_OT_STRING,										/**< A string. */
	};

	/**
	 * Class LSN_GENERIC_OPTION
	 * \brief A generic option that can be of any type.
	 *
	 * Description: A generic option that can be of any type.
	 */
	struct LSN_GENERIC_OPTION {
		LSN_OPTION_TYPE										otType;
		union {
			int64_t											i64Value;
			uint64_t										ui64Value;
			double											dValue;
		};
		std::u16string										u16String;

		union {
			int64_t											i64Min;
			uint64_t										ui64Min;
			double											dMin;
		};
		union {
			int64_t											i64Max;
			uint64_t										ui64Max;
			double											dMax;
		};
		std::u16string										u16Name;
		std::u16string										u16Desc;


		// == Functions.
		/**
		 * Sets a value.
		 * 
		 * \param _tVal The value to set.  It is automatically cast to the appropriate type.
		 **/
		template <typename _tnType>
		void												Set( _tnType _tVal ) {
			switch ( otType ) {
				case LSN_OT_INT8 : {
					i64Value = static_cast<int8_t>(std::min( static_cast<int8_t>(std::max( static_cast<int8_t>(_tVal), static_cast<int8_t>(i64Min) )), static_cast<int8_t>(i64Max) ));
					break;
				}
				case LSN_OT_INT16 : {
					i64Value = static_cast<int16_t>(std::min( static_cast<int16_t>(std::max( static_cast<int16_t>(_tVal), static_cast<int16_t>(i64Min) )), static_cast<int16_t>(i64Max) ));
					break;
				}
				case LSN_OT_INT32 : {
					i64Value = static_cast<int32_t>(std::min( static_cast<int32_t>(std::max( static_cast<int32_t>(_tVal), static_cast<int32_t>(i64Min) )), static_cast<int32_t>(i64Max) ));
					break;
				}
				case LSN_OT_INT64 : {
					i64Value = static_cast<int64_t>(std::min( static_cast<int64_t>(std::max( static_cast<int64_t>(_tVal), static_cast<int64_t>(i64Min) )), static_cast<int64_t>(i64Max) ));
					break;
				}

				case LSN_OT_UINT8 : {
					ui64Value = static_cast<uint8_t>(std::min( static_cast<uint8_t>(std::max( static_cast<int8_t>(_tVal), static_cast<int8_t>(i64Min) )), static_cast<uint8_t>(ui64Max) ));
					break;
				}
				case LSN_OT_UINT16 : {
					ui64Value = static_cast<uint16_t>(std::min( static_cast<uint16_t>(std::max( static_cast<uint16_t>(_tVal), static_cast<uint16_t>(i64Min) )), static_cast<uint16_t>(ui64Max) ));
					break;
				}
				case LSN_OT_UINT32 : {
					ui64Value = static_cast<uint32_t>(std::min( static_cast<uint32_t>(std::max( static_cast<uint32_t>(_tVal), static_cast<uint32_t>(i64Min) )), static_cast<uint32_t>(ui64Max) ));
					break;
				}
				case LSN_OT_UINT64 : {
					ui64Value = static_cast<uint64_t>(std::min( static_cast<uint64_t>(std::max( static_cast<uint64_t>(_tVal), static_cast<uint64_t>(i64Min) )), static_cast<uint64_t>(ui64Max) ));
					break;
				}

				case LSN_OT_FLOAT : {
					dValue = static_cast<float>(std::min( static_cast<float>(std::max( static_cast<float>(_tVal), static_cast<float>(dMin) )), static_cast<float>(dMax) ));
					break;
				}
				case LSN_OT_DOUBLE : {
					dValue = static_cast<double>(std::min( static_cast<double>(std::max( static_cast<double>(_tVal), static_cast<double>(dMin) )), static_cast<double>(dMax) ));
					break;
				}
				case LSN_OT_STRING : {
					u16String = CUtilities::Append( std::u16string(), std::to_wstring( _tVal ).c_str() );
					break;
				}
			}
		}


	};

}	// namespace lsn
