# BeesNES
A sub-cycle–accurate Nintendo Entertainment System emulator.
<br>Shawn (L. Spiro) Wilcoxen  

## Description
A Nintendo Entertainment System emulator with the goal of being “sub-cycle–accurate.” Many emulators claim to be “cycle-accurate” but are optimized to wait X cycles and then pump out all the work or otherwise do not actually divide all the work across all the cycles. In contrast, in this emulator, the work is correctly divided across the individual cycles for each instruction.

![image](https://user-images.githubusercontent.com/7362666/209457943-0ea5581c-2220-44bf-a046-ed2be6bf77d4.png)
![image](https://user-images.githubusercontent.com/7362666/209458107-919f63ed-2a4b-4bc0-ae65-46aff1c799b4.png)
![image](https://user-images.githubusercontent.com/7362666/209458143-d44796b1-c0cf-40e1-9f89-b71b8be555b1.png)
![image](https://user-images.githubusercontent.com/7362666/209458158-a01a7a05-bcee-494e-a0cc-8ee3e759cae8.png)
![image](https://user-images.githubusercontent.com/7362666/209458173-abb73203-21b0-4f74-8b76-e0113ed76cf7.png)
![image](https://user-images.githubusercontent.com/7362666/209458188-0fb8a5c5-3d25-4583-b618-e7ea92e3c75d.png)
![image](https://user-images.githubusercontent.com/7362666/209458230-92ef54ae-ce4e-4ceb-9a0d-e7e235c57feb.png)
![image](https://user-images.githubusercontent.com/7362666/209458242-87f07e69-22bf-44de-b3ba-ad15ddc157e0.png)
![image](https://user-images.githubusercontent.com/7362666/209458269-7ffe82aa-63fd-4327-b56a-fbbd392cbd38.png)
![image](https://user-images.githubusercontent.com/7362666/209458418-5e562f83-043b-4e38-aefc-cc8fb6eb568f.png)
![image](https://user-images.githubusercontent.com/7362666/209458490-10276135-25ad-40c0-930d-b0b273b6ef1d.png)
![image](https://user-images.githubusercontent.com/7362666/209479877-da6870b8-523b-4131-bbf8-7e472010be6f.png)



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
