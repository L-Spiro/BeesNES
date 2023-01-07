# BeesNES
A sub-cycle–accurate Nintendo Entertainment System emulator.
<br>Shawn (L. Spiro) Wilcoxen  

## Description
A Nintendo Entertainment System emulator with the goal of being “sub-cycle–accurate.” Many emulators claim to be “cycle-accurate” but are optimized to wait X cycles and then pump out all the work or otherwise do not actually divide all the work across all the cycles. In contrast, in this emulator, the work is correctly divided across the individual cycles for each instruction.

![image](https://user-images.githubusercontent.com/7362666/211026948-68194729-b835-40c5-892f-72eb54905a73.png)
![image](https://user-images.githubusercontent.com/7362666/211027329-47ca3326-269e-47dd-a557-719a4b62cd2f.png)
![image](https://user-images.githubusercontent.com/7362666/211027660-ac956e3c-1059-4137-9cd0-ee2505ff76a0.png)
![BeesNES_CPLlY70HG2](https://user-images.githubusercontent.com/7362666/211028518-5c59b5f3-4526-4c74-acce-8449618db1c5.png)
![image](https://user-images.githubusercontent.com/7362666/211028697-30f99250-1117-4bc4-a697-485b61a620e3.png)
![image](https://user-images.githubusercontent.com/7362666/211028992-31f8bad2-8a69-414d-99ef-a4237fb4de28.png)
![image](https://user-images.githubusercontent.com/7362666/211029287-d75e41d7-b7d1-4bb2-959d-ac1ebe7531ce.png)
![image](https://user-images.githubusercontent.com/7362666/211029387-4f8383a6-016d-44b8-b184-d4a9de47e546.png)
![image](https://user-images.githubusercontent.com/7362666/211029444-5807bb93-72f6-4735-bcc7-e024e6c18eac.png)
![image](https://user-images.githubusercontent.com/7362666/211127303-54596a2d-375f-4f0e-a00e-3bff1a7a2c2a.png)
![image](https://user-images.githubusercontent.com/7362666/211127385-83c95239-b6a3-4a8f-8c01-6b459030b221.png)

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

The CPU should be completely cycle-accurate, as every individual cycle is documented there. The same should apply to the PPU (questions surround PAL differences at the cycle level) and the APU.

Timing is not based off audio or monitor refresh rates as is done in many emulators. We use a real clock (with at-minimum microsecond accuracy) and match real timings to real time units, which we can speed up and slow down as options.  The NTSC version’s CPU will need to pump out ~29,780.506887 cycles per frame at 60.098814 FPS, while the PAL will need to pump out ~33,247.485977 cycles at 50.006979 FPS.  This means there is no noticeable visual delay (rendered frames are presented essentially immediately, rather than waiting for a monitor refresh, doing a frame’s worth of work, and then providing the visible frame after a delay) and that input is polled with exactly the same timing as in a real console, eliminating all input lag.  It should both look and _feel_ like a real console, with responsive controls that feel identical to how they do on real machines.

## Performance
There were initially some concerns that being sub-cycle–accurate would mean extra overhead—other emulators may skip useless redundant opcode fetches, but not here, and each fetch is accompanied by an entire CPU tick and all the work that goes into updating the CPU state, etc.  For this reason, most systems were implemented in an entirely branchless fashion—there are no “if”/“else” statements, “%” operations, “&” operations, “>=”/“<” checks, etc. when accessing memory; address mirroring, address mapping to registers, etc., is all handled entirely without branching, and most CPU, PPU, and APU cycles are branchless as well.  This more-than made up for the sub-cycle–accuracy overhead, resulting in a sub-cycle–accurate emulator that is more efficient than it would have been as a typical cycle-accurate emulator.

## Other Features
Other features will include:  
* A debugger.  
* A disassembler.  
* An assembler.  
* 1-877-Tools-4-TAS.  
* * Stepping and keylogging.  
* * Movie-making.  
