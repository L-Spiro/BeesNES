/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The implementation of the Core Audio audio system.
 */


#pragma once

#include "../LSNLSpiroNes.h"

#ifdef LSN_APPLE

#include "LSNAudioBase.h"

namespace lsn {

    /**
     * Class CAudioCoreAudio
     * \brief The implementation of the Core Audio audio system.
     *
     * Description: The implementation of the Core Audio audio system.
     */
    class CAudioCoreAudio : public CAudioBase {
        public :
        // == Functions.
        /**
         * Initializes audio.
         *
         * \return Returns true if initialization was successful.
         **/
        virtual bool                                        InitializeAudio();
        
        /**
         * Shuts down the audio.
         *
         * \return Returns true if shutdown was successful.
         **/
        virtual bool                                        ShutdownAudio();
        
        /**
         * Called when emulation begins.  Resets the ring buffer of buckets.
         **/
        virtual void                                        BeginEmulation();
    };

}   // namespace lsn

#endif  // #ifdef LSN_APPLE
