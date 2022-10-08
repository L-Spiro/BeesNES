# L. Spiro NES
A sub-cycle–accurate Nintendo Entertainment System emulator.
<br>Shawn (L. Spiro) Wilcoxen  

## Description
A Nintendo Entertainment System emulator with the goal of being “sub-cycle–accurate.” Many emulators claim to be “cycle-accurate” but are optimized to wait X cycles and then pump out all the work or otherwise do not actually divide all the work across all the cycles. In contrast, in this emulator, the work is correctly divided across the individual cycles for each instruction.  

## Accuracy
Most emulators have thus far have aimed for “Cycle Accuracy”: https://emulation.gametechwiki.com/index.php/Emulation_accuracy#Cycle_accuracy  
We are aiming for “Sub-cycle Accuracy”: https://emulation.gametechwiki.com/index.php/Emulation_accuracy#Subcycle_accuracy  
	
This means that multi-byte writes are correctly partitioned across cycles and partial data updates are possible, allowing for the more esoteric features of the system to be accurately emulated.  This means we should be able to support interrupt hijacking and any other cases that rely heavily on the cycle timing of the system.  

Additional options/features to facilitate accurate emulation:  
* Start-Up: Start from known state or from random state.  Helps the random seed in some games.  
* Hardware bugs will be emulated in both their buggy and fixed states (OAMADDR bugs (writing fewer than 8 bytes on the 2C02G) are examples of this).  
* Unofficial opcodes used by games will be optionally supported.  
* The bus will be open and correctly maintain the last floating read.  
* Etc.  

If behavior differes from the actual hardware result, it is considered a bug.  Hacks are to be avoided as much as possible.

The CPU should be completely cycle-accurate, as every individual cycle is documented there. The same should apply to the PPU (questions surround PAL differences at the cycle level) and probably only loosely applies to the APU.  In the case of the APU, while each individual cycle may not be doing exactly the correct work, the goal is for the output to be exactly correct. This means envelope counters and sweep units etc. get updated within the correct cycles and then perhaps only minor adjustments to match the output exactly to the real thing.  

Timing will not be based off audio as is done in many emulators. We will use a real clock (with at minimum microsecond accuracy) and try to match real timings to real time units, which we can speed up and slow down as options.  The NTSC version’s CPU will need to pump out ~29,780.506887 cycles per frame at 60.098814 FPS, while the PAL will need to pump out ~33,247.485977 cycles at 50.006979 FPS.

## Performance
Performance should be decent enough for real-time (though this remains an uncertainty at the moment). We are adding work to frames by adding a dereference and function-pointer call for each cycle, but the functions never leave instruction cache. If performance becomes a problem, it may warrant the creation of the approximate loops used in other emulators, leading to the scenario in which the approximate loops are used by most for gameplay and the slower original version of the emulator is used as a reference.  

## Other Features
Other features will include:  
* A debugger.  
* A disassembler.  
* An assembler.  
* 1-877-Tools-4-TAS.  
* * Stepping and keylogging.  
* * Movie-making.  
