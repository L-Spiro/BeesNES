/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Generates Gaussian-distributed thermal white noise based on RF thermal noise power.
 */


#pragma once

#include <random>
#include <vector>
#include <cmath>


namespace lsn {

	/**
	 * Class CRfWhiteNoiseGenerator
	 * \brief Generates Gaussian-distributed thermal white noise based on RF thermal noise power.
	 *
	 * The noise follows a zero-mean Gaussian distribution with a standard deviation equal to
	 * sqrt(k * T * B * R), where:
	 * - k is BoltzmannÅfs constant (1.38064852e-23 J/K)
	 * - T is the temperature in Kelvin
	 * - B is the bandwidth in Hz
	 * - R is the system resistance in ohms
	 *
	 * Use GetSample() for individual values or GenerateSamples() for a block.
	 */
	class CRfWhiteNoiseGenerator {
	public :
		/**
		 * \brief Constructor for the white noise generator.
		 *
		 * \param _dBandwidthHz    The noise bandwidth in Hz.
		 * \param _dResistanceOhms The system resistance in ohms (default 50.0).
		 * \param _dTempKelvin     The physical temperature in Kelvin (default 290.0).
		 */
		CRfWhiteNoiseGenerator( double _dBandwidthHz, double _dResistanceOhms = 50.0, double _dTempKelvin = 290.0 ) {
			constexpr double kdBoltzmann = 1.38064852e-23;	// J/K

			double dPowerWatts = kdBoltzmann * _dTempKelvin * _dBandwidthHz;
			m_dSigma = std::sqrt( dPowerWatts * _dResistanceOhms );

			std::random_device rdDevice;
			m_rnGen.seed( rdDevice() );
		}

		/**
		 * \brief Returns one sample of thermal noise in volts.
		 *
		 * \return Gaussian-distributed voltage sample (mean 0, stddev = sigma).
		 */
		double												GetSample() {
			return m_dSigma * m_dNormal( m_rnGen );
		}

		/**
		 * \brief Generates a vector of samples of Gaussian white noise.
		 *
		 * \param _stCount Number of samples to generate.
		 * \return Vector of noise samples.
		 */
		std::vector<double>									GenerateSamples( size_t _stCount ) {
			std::vector<double> vOut;
			vOut.reserve( _stCount );

			for ( size_t I = 0; I < _stCount; ++I ) {
				vOut.push_back( GetSample() );
			}

			return vOut;
		}

	protected:
		double												m_dSigma;	///< Calculated standard deviation in volts.
		std::mt19937										m_rnGen;	///< Random number generator.
		std::normal_distribution<double>					m_dNormal = std::normal_distribution<double>( 0.0, 1.0 ); ///< Standard normal distribution.
	};

}	// namespace lsn
