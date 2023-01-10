# BeesNES
A sub-cycle–accurate Nintendo Entertainment System emulator.
<br>Shawn (L. Spiro) Wilcoxen  

## Description
A Nintendo Entertainment System emulator with the goal of being “sub-cycle–accurate.” Many emulators claim to be “cycle-accurate” but are optimized to wait X cycles and then pump out all the work or otherwise do not actually divide all the work across all the cycles. In contrast, in this emulator, the work is correctly divided across the individual cycles for each instruction.

![image](https://user-images.githubusercontent.com/7362666/211444553-cb95e2d5-1ac4-43cf-8146-55e155d7cbd6.png)
![image](https://user-images.githubusercontent.com/7362666/211444733-ee658c92-427e-407d-8524-d493a1dd520f.png)
![image](https://user-images.githubusercontent.com/7362666/211444928-6071119a-6c90-4086-a232-4c427bfa89a2.png)
![image](https://user-images.githubusercontent.com/7362666/211445283-ed68fc99-f856-4ca1-8762-598a801f8997.png)
![image](https://user-images.githubusercontent.com/7362666/211445534-eb06f929-0457-4a18-8f62-5788fe7a6d60.png)
![image](https://user-images.githubusercontent.com/7362666/211445739-855bc0b7-558b-42a3-99ce-f0a680368962.png)
![image](https://user-images.githubusercontent.com/7362666/211446083-e5ff2703-3bc2-401d-b472-68c8036c2fb9.png)
![image](https://user-images.githubusercontent.com/7362666/211446153-0b54ab3d-5476-468f-bcf8-97ec62ba4dd1.png)
![image](https://user-images.githubusercontent.com/7362666/211446221-22992f29-9d23-4c7e-9f85-6380c9f03259.png)

YouTube Video: Castlevania Demo Play (Low Noise)
https://www.youtube.com/watch?v=HyLtecKOjLM&list=PLM2QRzvCtV12TZcpXrUm1LQnyCgHy5Uxa&index=7
YouTube Video: Battletoads Opening (Extreme Noise)
https://www.youtube.com/watch?v=K3sVkZFxkvs&list=PLM2QRzvCtV12TZcpXrUm1LQnyCgHy5Uxa&index=6
YouTube Video: Akira Opening (Extreme Noise)
https://www.youtube.com/watch?v=mSZlMw0cPEY&list=PLM2QRzvCtV12TZcpXrUm1LQnyCgHy5Uxa&index=4

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
