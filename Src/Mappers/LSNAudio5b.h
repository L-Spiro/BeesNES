/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Sunsoft 5B audio.
 */


#pragma once

#include "../LSNLSpiroNes.h"
#include "../Bus/LSNBus.h"
#include "../Utilities/LSNUtilities.h"

#include <cassert>


#pragma warning( push )
#pragma warning( disable : 4201 )	// warning C4201: nonstandard extension used: nameless struct/union

#define LSN_SUNSOFT_5B_REL_VOL							0.4177518188953399658203125f	// 0.36758463101626792646214880733168683946132659912109375f


namespace lsn {

#pragma pack( push, 1 )
	struct LSN_5B_REGS {
		uint16_t										ui16Tone[3];							/**< Tone channels A, B, and C. */
		uint8_t											ui8NoisePeriod;							/**< 5-bit noise period. */
		uint8_t											ui8Disable;								/**< Noise and tone disablers. */
		uint8_t											ui8EnvAndVol[3];						/**< Envelopes and olume levels for tone channels. */
		uint16_t										ui16EnvPeriod;							/**< Envelope period. */
		uint8_t											ui8EnvShape;							/**< Envelope shape, continue, attack, alternate, and hold. */
		uint8_t											ui8Io[2];								/**< IO ports. */
	};
#pragma pack( pop )

	/**
	 * Class CAudio5b
	 * \brief Sunsoft 5B audio.
	 *
	 * Description: Sunsoft 5B audio.
	 */
	class CAudio5b {
	public :
		CAudio5b() {
			GenVolTable();
			ResetFull();
		}
		virtual ~CAudio5b() {
		}


		// == Functions.
		/**
		 * Applies mapping to the CPU bus.
		 *
		 * \param _pbCpuBus A pointer to the CPU bus.
		 */
		void											ApplyMap( CCpuBus * _pbCpuBus ) {
			for ( uint32_t I = 0xC000; I < 0xE000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudio5b::SelectRegisterC000_DFFF, this, 0 );
			}
			for ( uint32_t I = 0xE000; I < 0x10000; ++I ) {
				_pbCpuBus->SetWriteFunc( uint16_t( I ), &CAudio5b::WriteRegisterE000_FFFF, this, 0 );
			}
		}

		/**
		 * Ticks with the CPU.
		 */
		virtual void									Tick() {
			if LSN_UNLIKELY( (++m_ui8Divider & 0xF) == 0 ) {
				// Easy to unroll loop.
				m_tTones[0].Tick( m_rRegs.ui16Tone[0] );
				m_tTones[1].Tick( m_rRegs.ui16Tone[1] );
				m_tTones[2].Tick( m_rRegs.ui16Tone[2] );
			}
			if LSN_UNLIKELY( (m_ui8Divider & 0x1F) == 0 ) {
				m_nNoise.Tick( m_rRegs.ui8NoisePeriod );
			}
		}

		/**
		 * Does a full power-on reset
		 **/
		void											ResetFull( bool _bToKnown = false ) {
			if ( _bToKnown ) {
				std::memset( m_ui8Registers, 0, sizeof( m_ui8Registers ) );
				std::memset( m_tTones, 0, sizeof( m_tTones ) );
				//m_ui16Counter = 0;
				m_ui8Reg = 0;
			}
			m_rRegs.ui16Tone[0] &= 0x0FFF;
			m_rRegs.ui16Tone[1] &= 0x0FFF;
			m_rRegs.ui16Tone[2] &= 0x0FFF;

		}

		/**
		 * Does a soft reset.
		 **/
		void											ResetSoft() {
			/*std::memset( m_ui8Registers, 0, sizeof( m_ui8Registers ) );
			m_ui16Counter = 0;
			m_ui8Reg = 0;*/
		}

		/**
		 * Gets the extended-audio sample.
		 * 
		 * \return Returns the current sample.
		 **/
		inline float									Sample() {
			float fRet = 0.0f;
			if ( (m_rRegs.ui8Disable & 0b00001001) == 0b00001001 ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[0]&0xF)];
			}
			else if ( (m_rRegs.ui8Disable & 0b00001001) == 0b00000000 ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[0]&0xF)*(uint8_t(m_tTones[0].bOnOff)&m_nNoise.Value())];
			}
			else if ( !(m_rRegs.ui8Disable & 0b00000001) ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[0]&0xF)*m_tTones[0].bOnOff];
			}
			else if ( !(m_rRegs.ui8Disable & 0b00001000) ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[0]&0xF)*m_nNoise.Value()];
			}

			if ( (m_rRegs.ui8Disable & 0b00010010) == 0b00010010 ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[1]&0xF)];
			}
			else if ( (m_rRegs.ui8Disable & 0b00010010) == 0b00000000 ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[1]&0xF)*(uint8_t(m_tTones[1].bOnOff)&m_nNoise.Value())];
			}
			else if ( !(m_rRegs.ui8Disable & 0b00000010) ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[1]&0xF)*m_tTones[1].bOnOff];
			}
			else if ( !(m_rRegs.ui8Disable & 0b00010000) ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[1]&0xF)*m_nNoise.Value()];
			}

			if ( (m_rRegs.ui8Disable & 0b00100100) == 0b00100100 ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[2]&0xF)];
			}
			else if ( (m_rRegs.ui8Disable & 0b00100100) == 0b00000000 ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[2]&0xF)*(uint8_t(m_tTones[2].bOnOff)&m_nNoise.Value())];
			}
			else if ( !(m_rRegs.ui8Disable & 0b00000100) ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[2]&0xF)*m_tTones[2].bOnOff];
			}
			else if ( !(m_rRegs.ui8Disable & 0b00100000) ) {
				fRet += m_fToneVolTable[(m_rRegs.ui8EnvAndVol[2]&0xF)*m_nNoise.Value()];
			}

			return fRet * -LSN_SUNSOFT_5B_REL_VOL;
		}

		/**
		 * Post-process an output sample.  Applies the volume crunch.
		 * 
		 * \param _fSample The sample to modify.
		 * \return Returns the volume-crunched sample.
		 **/
		inline float									PostProcessSample( float _fSample, float _fHz ) {
			constexpr float fReNorm = 1.5f * (1.0f / LSN_SUNSOFT_5B_REL_VOL);
			//constexpr double dInvReNorm = 1.0 / fReNorm;
			float dAbs = std::abs( _fSample );
			if ( dAbs > 0.0 ) {
				auto fThis = dAbs * (fReNorm);
				float fXsqr = fThis * fThis;
				float fX1 = -0.1712609231472015380859375f * fXsqr * fThis - 0.0505211390554904937744140625f * fXsqr + 0.9762413501739501953125f * fThis;
				float fX2 = (1.0f - std::expf( -1.399f * fThis ));
				float fF = std::clamp( (fThis - 0.5f) / (1.28f - 0.5f), 0.0f, 1.0f );
				float fThisScale = (fX1 * (1.0f - fF) + fX2 * fF);
				fThisScale = fThisScale / fThis;
				m_fVolAvg = std::min( fThisScale, CUtilities::UpdateRunningAvg( m_fVolAvg, fThisScale, 0.03125f * _fHz ) );
				_fSample *= m_fVolAvg;
			}
			else {
				m_fVolAvg = CUtilities::UpdateRunningAvg( m_fVolAvg, 1.0f, 0.03125f * _fHz );
			}
			return _fSample;
		}

	protected :
		// == Types.
		/** The tone channels. */
		struct LSN_TONE {
			uint16_t									ui16Counter;							/**< The channel counter. */
			//uint8_t										u8Divisor;								/**< The count to 16. */
			bool										bOnOff;									/**< Whether the pulse is 0 or non-0. */


			// == Functions.
			/**
			 * Advances the tone state by 1 cycle.
			 * 
			 * \param _ui1Period The tone period.
			 **/
			inline void									Tick( uint16_t _ui1Period ) {
				//if LSN_UNLIKELY( (++u8Divisor & 0xF) == 0 ) {
				//	u8Divisor = 0;
					if LSN_UNLIKELY( ++ui16Counter >= _ui1Period ) {
						ui16Counter = 0;
						bOnOff = !bOnOff;
					}
				//}
			}
		};

		/** The noise channels. */
		struct LSN_NOISE {
			uint32_t									ui32Noise = 1;							/**< The noise register. */
			uint16_t									ui16Counter;							/**< The channel counter. */


			// == Functions.
			/**
			 * Advances the tone state by 1 cycle.
			 * 
			 * \param _ui1Period The tone period.
			 **/
			inline void									Tick( uint16_t _ui1Period ) {
				//if LSN_UNLIKELY( (++u8Divisor & 0xF) == 0 ) {
				//	u8Divisor = 0;
					if LSN_UNLIKELY( ++ui16Counter >= _ui1Period ) {
						ui16Counter = 0;
						ui32Noise ^= (((ui32Noise & 1) ^ ((ui32Noise >> 3) & 1)) << 17);
						ui32Noise >>= 1;
					}
				//}
			}

			/**
			 * Gets the current noise value.
			 * 
			 * \return Gets the current noise value.
			 **/
			inline uint32_t								Value() const { return ui32Noise & 1; }
		};


		// == Members.
		LSN_TONE										m_tTones[3];							/**< The tone channels. */
		float											m_fVolTable[31*3+1];					/**< The volume look-up table. */
		float											m_fToneVolTable[16];					/**< Standard volume tables. */
		float											m_fVolAvg = 1.0f;						/**< Running-average volume. */
		LSN_NOISE										m_nNoise;								/**< The current noise shift values. */

		union {
			uint8_t										m_ui8Registers[16];						/**< Each register. */
			LSN_5B_REGS									m_rRegs;								/**< Alternate names for registers. */
		};

		static_assert( offsetof( lsn::LSN_5B_REGS, ui16EnvPeriod ) == 11, "ui16EnvPeriod must be at offset 11." );
		static_assert( offsetof( lsn::LSN_5B_REGS, ui8Io ) == 14, "ui8Io must be at offset 14." );

		uint8_t											m_ui8Divider;							/**< The divider. */
		uint8_t											m_ui8Reg;								/**< The register to which to write. */
		

		// == Functions.
		/**
		 * Generates the volume table.
		 **/
		void											GenVolTable() {
			std::vector<double> vX( 16 ), vY( 16 ), vY2;
			for ( size_t I = 0; I < 16; ++I ) { vX[I] = double( I ); }
			for ( size_t I = 0; I < 16; ++I ) {
				vY[I] = double( m_fToneVolTable[I] = PolynomialVol( I ) );
			}

			PrepareNaturalCubicSpline( vX, vY, vY2 );

			for ( size_t I = 0; I <= ((14 + 1) * 2 - 1); ++I ) {
				m_fVolTable[I] = float( EvaluateSpline( vX, vY, vY2, (I + 1.0) / 2.0 - 1.0 ) );
			}
			m_fVolTable[0] = m_fVolTable[1] = 0.0f;

			for ( size_t I = ((14 + 1) * 2 - 1) + 1; I < LSN_ELEMENTS( m_fVolTable ); ++I ) {
				double dVal = double( I ) - ((14 + 1) * 2 - 1);
				m_fVolTable[I] = float( std::pow( dVal / 2.0, 0.236461423950310223318638236378319561481475830078125 ) * 0.236461423950310223318638236378319561481475830078125 + 1.0 );
			}
			return;
		}

		/**
		 * Gets the volume at a given level for indices 0-29 of the volume table.
		 * 
		 * \param _sIdx The index of the volume to get.
		 * \return Returns the volume of the given index.
		 **/
		static float									PolynomialVol( size_t _sIdx ) {
			if ( _sIdx == 0 ) { return 0.0f; }
			if ( _sIdx == 15 ) { return 0.924697458744049072265625f; }
			static const double adP[] = {
				-6.8560006831366094e-09,   // p0
				-1.6670136239606261e+00,   // p1
				 5.2884291057830275e+00,   // p2
				-7.0221945870618967e+00,   // p3
				 5.2770778907577878e+00,   // p4
				-2.5333104540253575e+00,   // p5
				 8.2738435415533151e-01,   // p6
				-1.9038549217499176e-01,   // p7
				 3.1416961081884964e-02,   // p8
				-3.7342408373029520e-03,   // p9
				 3.1692330066205568e-04,   // p10
				-1.8726135942948538e-05,   // p11
				 7.3146194848386232e-07,   // p12
				-1.6968901013101078e-08,   // p13
				 1.7693493519638962e-10    // p14
			};
			double fX = double( _sIdx );
			// Evaluate via Horner's method from highest degree down.
			double fY = adP[14];
			for ( int32_t I = 13; I >= 0; --I ) {
				fY = fY * fX + adP[I];
			}
			return float( fY ) * 0.707106769084930419921875f;
		}

		/**
		 * Gets the ideal volume at a given level for the 4-bit volume.
		 * 
		 * \param _sIdx The index of the volume to get.
		 * \return Returns the volume of the given index.
		 **/
		static float									Ideal4Bit( size_t _sIdx ) {
			if ( _sIdx == 0 ) { return 0.0f; }
			return std::powf( 10.0f, (std::log10f( std::sqrtf( 0.5f ) ) * 20.0f * float( 15 - _sIdx )) / 20.0f );
		}

		/**
		 * \brief Prepare second‐derivatives for a natural cubic spline.
		 * 
		 * \param _rvX   Strictly increasing knot abscissas (x-coords).
		 * \param _rvY   Knot ordinates (y-values).
		 * \param _rvY2  [out] Computed second‐derivatives at each knot.
		 */
		static void										PrepareNaturalCubicSpline( const std::vector<double> &_rvX,
			const std::vector<double> &_rvY,
			std::vector<double> &_rvY2 ) {
			size_t sN = _rvX.size();
			if ( sN < 2 || _rvY.size() != sN ) {
				throw std::runtime_error( "PrepareNaturalCubicSpline: invalid input sizes" );
			}
			_rvY2.assign( sN, 0.0 );
			if ( sN < 3 ) {
				return; // No internal knots to enforce.
			}

			std::vector<double> vU( sN - 1, 0.0 );
			// Natural boundary: second derivative zero at ends.
			for ( size_t I = 1; I < sN - 1; ++I ) {
				double dSig		= (_rvX[I] - _rvX[I-1]) / (_rvX[I+1] - _rvX[I-1]);
				double dP		= dSig * _rvY2[I-1] + 2.0;
				_rvY2[I]		= (dSig - 1.0) / dP;

				double dDy1		= (_rvY[I]   - _rvY[I-1]) / (_rvX[I]   - _rvX[I-1]);
				double dDy2		= (_rvY[I+1] - _rvY[I]) / (_rvX[I+1]   - _rvX[I]);
				vU[I]			= (6.0 * (dDy2 - dDy1) / (_rvX[I+1]    - _rvX[I-1])
									- dSig * vU[I-1]) / dP;
			}
			// Back‐substitute for second derivatives.
			for ( size_t I = sN - 2; I-- > 0; ) {
				_rvY2[I] = _rvY2[I] * _rvY2[I+1] + vU[I];
			}
		}

		/**
		 * \brief Evaluate the natural cubic spline at a given x.
		 * 
		 * \param _rvX   Knot abscissas (x-coords), same as used in PrepareNaturalCubicSpline.
		 * \param _rvY   Knot ordinates (y-values).
		 * \param _rvY2  Second derivatives from PrepareNaturalCubicSpline.
		 * \param _dX    Query x-value (must lie between _rvX.front() and _rvX.back()).
		 * \returns      Interpolated y-value.
		 */
		static double									EvaluateSpline( const std::vector<double> &_rvX,
			const std::vector<double> &_rvY,
			const std::vector<double> &_rvY2,
			double _dX ) {
			size_t sN = _rvX.size();
			if ( sN == 0 || _rvY.size() != sN || _rvY2.size() != sN ) {
				throw std::runtime_error( "EvaluateSpline: Invalid input sizes." );
			}
			// Clamp to interval.
			if ( _dX <= _rvX.front() ) { return _rvY.front(); }
			if ( _dX >= _rvX.back() ) { return _rvY.back(); }

			// Binary search for right interval.
			size_t sLo = 0, sHi = sN - 1;
			while ( sHi - sLo > 1 ) {
				size_t sMid = (sLo + sHi) >> 1;
				if ( _rvX[sMid] > _dX ) { sHi = sMid; }
				else { sLo = sMid; }
			}

			double dH = _rvX[sHi]  - _rvX[sLo];
			if ( dH <= 0.0 ) { throw std::runtime_error( "EvaluateSpline: Zero interval length." ); }
			double dA = (_rvX[sHi] - _dX) / dH;
			double dB = (_dX       - _rvX[sLo]) / dH;

			// Cubic spline polynomial formula.
			return dA * _rvY[sLo]
				 + dB * _rvY[sHi]
				 + ( (dA * dA * dA - dA) * _rvY2[sLo]
				   + (dB * dB * dB - dB) * _rvY2[sHi]) * (dH * dH) / 6.0;
		}

		/**
		 * Selects an audio register.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						SelectRegisterC000_DFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CAudio5b * pmThis = reinterpret_cast<CAudio5b *>(_pvParm0);
			pmThis->m_ui8Reg = _ui8Val;
		}
		/**
		 * Writes to the selected audio register.
		 *
		 * \param _pvParm0 A data value assigned to this address.
		 * \param _ui16Parm1 A 16-bit parameter assigned to this address.  Typically this will be the address to write to _pui8Data.
		 * \param _pui8Data The buffer to which to write.
		 * \param _ui8Val The value to write.
		 */
		static void LSN_FASTCALL						WriteRegisterE000_FFFF( void * _pvParm0, uint16_t /*_ui16Parm1*/, uint8_t * /*_pui8Data*/, uint8_t _ui8Val ) {
			CAudio5b * pmThis = reinterpret_cast<CAudio5b *>(_pvParm0);
			if ( (pmThis->m_ui8Reg & 0b11110000) == 0 ) {
				uint8_t ui8Reg = pmThis->m_ui8Reg & 0b00001111;
				switch ( ui8Reg ) {
					case 0x01 : {							// Channel A high period.
						pmThis->m_ui8Registers[0x01] = _ui8Val & 0b00001111;
						break;
					}
					case 0x03 : {							// Channel B high period.
						pmThis->m_ui8Registers[0x03] = _ui8Val & 0b00001111;
						break;
					}
					case 0x05 : {							// Channel C high period.
						pmThis->m_ui8Registers[0x05] = _ui8Val & 0b00001111;
						break;
					}
					case 0x06 : {							// Noise period.
						pmThis->m_ui8Registers[0x06] = _ui8Val & 0b00011111;
						break;
					}
					case 0x07 : {							// Noise disable on channels C/B/A, Tone disable on channels c/b/a.
						pmThis->m_ui8Registers[0x07] = _ui8Val & 0b00111111;
						break;
					}
					case 0x08 : {							// Channel A envelope enable (E), volume (V).
						pmThis->m_ui8Registers[0x08] = _ui8Val & 0b00011111;
						break;
					}
					case 0x09 : {							// Channel B envelope enable (E), volume (V).
						pmThis->m_ui8Registers[0x09] = _ui8Val & 0b00011111;
						break;
					}
					case 0x0A : {							// Channel C envelope enable (E), volume (V).
						pmThis->m_ui8Registers[0x0A] = _ui8Val & 0b00011111;
						break;
					}
					case 0x0D : {							// Envelope reset and shape: continue (C), attack (A), alternate (a), hold (H).
						pmThis->m_ui8Registers[0x0D] = _ui8Val & 0b00001111;
						break;
					}
					default : {
						pmThis->m_ui8Registers[ui8Reg] = _ui8Val;
					}
				}
			}
		}
	};

}	// namespace lsn

#pragma warning( pop )
