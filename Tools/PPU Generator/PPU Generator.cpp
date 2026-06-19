// PPU Generator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../../Src/LSNLSpiroNes.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>


std::string		GenFunc( uint32_t _ui32RegCode, bool _bOddFrameShenanigans, uint32_t _ui32PreRender, uint32_t _ui32Render, uint32_t _ui32PostRender, uint32_t _ui16DotHeight, uint32_t _ui16DotWidth,
	uint16_t _ui16X, uint16_t _ui16Y ) {
	std::string sRet;

	// _ui16X counts half-cycles, so it goes from 0 to (_ui16DotWidth * 2 - 1).
	// Convert to actual dot.
	uint16_t ui16Dot = _ui16X >> 1;
	bool bIsPhi2 = bool( _ui16X & 1 );
	
	// DOT: The integer dot count from 0 to 340.
	// DELAY: Half-cycle delay as written in the signals page: https://www.nesdev.org/wiki/PPU_signals
	// The comparison should be made against _ui16X, not ui16Dot.
#define PPU_CYCLE( DOT, DELAY )		uint16_t( DOT * 2 + uint16_t( std::round( DELAY * 2.0 ) ) )


	// Remove opening new lines.
	while ( sRet.size() >= 2 && sRet[0] == '\r' && sRet[1] == '\n' ) {
		sRet.erase( sRet.begin(), sRet.begin() + 2 );
	}

	return sRet;
#undef PPU_CYCLE
}


void			GenerateCycleFuncs( uint32_t _ui32RegCode, bool _bOddFrameShenanigans, uint32_t _ui32PreRender, uint32_t _ui32Render, uint32_t _ui32PostRender, uint16_t _ui16DotHeight, uint16_t _ui16DotWidth, const char * _pcFile ) {
	struct LSN_TABLE_INDICES {
		uint16_t								ui16X;
		uint16_t								ui16Y;
	};
	typedef std::vector<LSN_TABLE_INDICES>		CStringList;
	std::map<std::string, CStringList> mMap;

	LSN_TABLE_INDICES tiI = { 0, 0 };
	while ( tiI.ui16Y < _ui16DotHeight ) {
		while ( tiI.ui16X < _ui16DotWidth * 2 ) {
			std::string sThis = GenFunc( _ui32RegCode, _bOddFrameShenanigans, _ui32PreRender, _ui32Render, _ui32PostRender, _ui16DotHeight, _ui16DotWidth,
				tiI.ui16X, tiI.ui16Y );
					
			auto aInMap = mMap.find( sThis );
			if ( aInMap == mMap.end() ) {
				CStringList slList;
				slList.push_back( tiI );
				mMap.insert( std::make_pair( sThis, slList ) );
			}
			else {
				aInMap->second.push_back( tiI );
			}
			++tiI.ui16X;
		}
		++tiI.ui16Y;
		tiI.ui16X = 0;
	}

	auto FuncName = [_ui32RegCode]( const CStringList &_slDots ) {
		std::string sName = "Cycle_" + std::to_string( _ui32RegCode ) + "_";
		size_t I = 0;
		for ( ; I < _slDots.size() && I < 10; ++I ) {
			sName += "_" + std::to_string( _slDots[I].ui16X ) + "x" + std::to_string( _slDots[I].ui16Y );
		}
		if ( I < _slDots.size() ) {
			sName += "_X";
		}
		return sName;
	};


	std::string sTmp = "if constexpr ( _ui32RegCode == " + std::to_string( _ui32RegCode ) + " ) {\r\n";


	for ( auto I = mMap.begin(); I != mMap.end(); ++I ) {
		sTmp += "	{\r\n";
		sTmp += "		PfCycles pfTmp = &CPpu2C0X::" + FuncName( (*I).second ) + ";\r\n";
		for ( size_t J = 0; J < (*I).second.size(); ++J ) {
			sTmp += "		m_cCycle[" + std::to_string( (*I).second[J].ui16Y * _ui16DotWidth + (*I).second[J].ui16X ) +
				"] = pfTmp;\r\n";
		}
		//sTmp += "&CPpu2C0X::" + FuncName( (*I).second ) + ";\r\n";
		sTmp += "	}\r\n";
	}
	sTmp += "}\r\n";
	std::string sFile = "LSNCreateCycleTable" + std::string( _pcFile ) + ".inl";
	lsn::DebugLine( sFile );
	lsn::DebugA( sTmp.c_str() );
	{
		FILE * pfFile = std::fopen( sFile.c_str(), "w" );
		if ( pfFile ) {
			std::fwrite( sTmp.data(), sTmp.size(), 1, pfFile );
			std::fclose( pfFile );
		}
	}

	lsn::DebugA( "\r\n\r\n\r\n" );

	sTmp = "";
	for ( auto I = mMap.begin(); I != mMap.end(); ++I ) {
		sTmp += "void LSN_FASTCALL								" + FuncName( (*I).second ) + "() {\r\n" +
			(*I).first +
			"}\r\n\r\n";

	}
	sFile = "LSNGenFuncs" + std::string( _pcFile ) + ".inl";
	lsn::DebugLine( sFile );
	lsn::DebugA( sTmp.c_str() );
	{
		FILE * pfFile = std::fopen( sFile.c_str(), "w" );
		if ( pfFile ) {
			std::fwrite( sTmp.data(), sTmp.size(), 1, pfFile );
			std::fclose( pfFile );
		}
	}
	volatile int gjghg = 0;
	(void)gjghg;
}

int main() {

	struct LSN_PPU_REVISIONS {
		uint32_t					ui32RegCode;
		uint32_t					ui32PreRender;
		uint32_t					ui32Render;
		uint32_t					ui32PostRender;
		uint16_t					ui16DotHeight;
		uint16_t					ui16DotWidth;
		bool						bOddFrameShenanigans;

		const char *				pcFile;
	} prRevisions[] = {
		//ui32RegCode				ui32PreRender						ui32Render								ui32PostRender							ui16DotHeight						_ui16DotWidth				bOddFrameShenanigans	pcFile
		{ lsn::LSN_PM_NTSC,			lsn::LSN_PM_NTSC_PRERENDER,			lsn::LSN_PM_NTSC_RENDER_LINES,			lsn::LSN_PM_NTSC_POSTRENDER_LINES,		lsn::LSN_PM_NTSC_SCANLINES,			lsn::LSN_PM_NTSC_DOTS_X,	true,					"Ntsc" },
		{ lsn::LSN_PM_PAL,			lsn::LSN_PM_PAL_PRERENDER,			lsn::LSN_PM_PAL_RENDER_LINES,			lsn::LSN_PM_PAL_POSTRENDER_LINES,		lsn::LSN_PM_PAL_SCANLINES,			lsn::LSN_PM_PAL_DOTS_X,		false,					"Pal" },
		{ lsn::LSN_PM_DENDY,		lsn::LSN_PM_DENDY_PRERENDER,		lsn::LSN_PM_DENDY_RENDER_LINES,			lsn::LSN_PM_DENDY_POSTRENDER_LINES,		lsn::LSN_PM_DENDY_SCANLINES,		lsn::LSN_PM_DENDY_DOTS_X,	false,					"Dendy" },
		{ lsn::LSN_PM_PALM,			lsn::LSN_PM_PALM_PRERENDER,			lsn::LSN_PM_PALM_RENDER_LINES,			lsn::LSN_PM_PALM_POSTRENDER_LINES,		lsn::LSN_PM_PALM_SCANLINES,			lsn::LSN_PM_PALM_DOTS_X,	false,					"PalM" },
		{ lsn::LSN_PM_PALN,			lsn::LSN_PM_PALN_PRERENDER,			lsn::LSN_PM_PALN_RENDER_LINES,			lsn::LSN_PM_PALN_POSTRENDER_LINES,		lsn::LSN_PM_PALN_SCANLINES,			lsn::LSN_PM_PALN_DOTS_X,	false,					"PalN" },
	};

	for ( size_t I = 0; I < std::size( prRevisions ); ++I ) {
		GenerateCycleFuncs( prRevisions[I].ui32RegCode,
			prRevisions[I].bOddFrameShenanigans,
			prRevisions[I].ui32PreRender, prRevisions[I].ui32Render, prRevisions[I].ui32PostRender,
			prRevisions[I].ui16DotHeight, prRevisions[I].ui16DotWidth,
			prRevisions[I].pcFile );
	}
    
	return 0;
}
