/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Holds 6 adjacent samples to be used with 6-point 5th-order Hermite interpolation along with information about the timing of the
 *	group of samples.
 */


#pragma once

#include "../LSNLSpiroNes.h"

#define LSN_SAMPLER_BUCKET_SIZE			6

namespace lsn {

	/**
	 * Class LSN_SAMPLE_BUCKET
	 * \brief Holds 6 adjacent samples to be used with 6-point 5th-order Hermite interpolation along with information about the timing of the
	 *	group of samples.
	 *
	 * Description: Holds 6 adjacent samples to be used with 6-point 5th-order Hermite interpolation along with information about the timing of the
	 *	group of samples.
	 */
	struct LSN_SAMPLE_BUCKET {
		/** The starting APU cycle for the bucket. */
		uint64_t						ui64ApuStartCycle;
		/** The bucket of samples. */
		float							fBucket[LSN_SAMPLER_BUCKET_SIZE];
		/** The interpolation amount to pass to the sampler. */
		float							fInterp;
	};

}	// namespace lsn
