# BeesNES
A sub-cycle–accurate Nintendo Entertainment System emulator.
<br>Shawn (L. Spiro) Wilcoxen  

## Description
A “sub-cycle–accurate” Nintendo Entertainment System emulator with the goal of being as authentic of an experience as possible.  It should look, sound, and _feel_ like real hardware, with convincing visuals, clean and accurate audio, and real-time input response.  No visual or audible delays.  BeesNES also represents the under-served regions with support for a wide range of console variants, currently including NTSC, PAL, PAL “Dendy” Famiclone, PAL-M Brazilian Famiclone, and PAL-N Argentinian Famiclone.

## Visual Samples
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/74c2a421-b4a4-41d7-a572-7c8d1a2f530a" /><br>AccuracyCoin results.<br><br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/ab107073-3271-4715-99d3-525b107349f7" /><br>General NTSC filter.<br><br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/08bc0c44-e2fd-403f-ab29-3c952161e35a" /><br>Gamma-aware resampling.<br><br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/5efff799-1a2d-4aaf-833e-4e1e75e372b7" /><br>Measured CRT gamma adjusted for display on sRGB (etc.) monitors.<br><br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/e969cdc2-3fbe-4903-ad04-3939498b4b14" /><br>Signal noise physically accurate and can be based off your CPU’s actual temperature (Requires “Run as administrator”).<br><br>
<img width="295" height="292" alt="image" src="https://github.com/user-attachments/assets/94453b94-6827-4b2e-ac3f-6fa186b7a396" /><img width="295" height="292" alt="image" src="https://github.com/user-attachments/assets/476cc632-c193-4423-ae23-348990da87ff" /><img width="295" height="292" alt="image" src="https://github.com/user-attachments/assets/6b84b85d-1ae0-4ee6-a86b-2e966d33dd21" /><img width="295" height="292" alt="image" src="https://github.com/user-attachments/assets/7438be32-c709-44e5-8956-d358d011c7be" /><br>Image remains sharp at low resolutions—no blurry pixels—thanks to manual resampling via convolution.<br><br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/6a53aea2-0dc2-4669-b8cf-961d2b47b711" /><br>Authentic phosphor decay.<br><br><br>


RF Cables:<br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/a787e80f-6406-40c3-b717-bcca865413e6" /><br>
Composite:<br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/db6b2d09-d64d-46b1-990f-f7247b28a8ac" /><br>
HDMI:<br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/951fd224-a10e-4a96-828d-200b3410bbe2" /><br>
HDMI Mod:<br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/b967db94-73ce-41c3-a815-dce8171d17d3" /><br>

PAL-B (RF Cables):<br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/57ccfd27-9b08-47c2-a8d6-e9441ed5445e" /><br>
PAL-B (Composite):<br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/cb036bf4-9bad-4b5f-a1f8-c6db6b97aa1f" /><br>
PAL-D (“Dendy” Famiclone) (Composite):<br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/76df84b7-e2c8-41e1-ad7b-eddb5a1ec998" /><br>
PAL-M (Brazilian Famiclone) (Composite):<br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/b003fc22-3cdb-4e28-8421-79ce43449074" /><br>
PAL-N (Argentinian Famiclone) (Composite):<br>
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/7dde87fd-8f12-4530-9027-a9470ef4aabf" /><br><br><br>


## Audio Accuracy
<img width="547" height="744" alt="image" src="https://github.com/user-attachments/assets/321a5ee3-9695-4d9f-9cd5-08b70e9e9236" /><br>
A wide array of options is available for tailoring the audio to specific hardware devices.

<img width="1920" height="1040" alt="image" src="https://github.com/user-attachments/assets/8d821097-dcfb-4c17-bfb6-29fc9fe57edc" /><br>
Top: Hardware reference; Bottom: BeesNES.  Hardware is matched exactly, minus the noise.

<img width="1920" height="1040" alt="image" src="https://github.com/user-attachments/assets/57a19978-56ac-4d2c-8d0f-47de269d1a1d" /><br>
Top: Hardware reference; Bottom: BeesNES.  Excluding mains-hum noise, the hardware reference is matched down to the tiniest details.

![image](https://github.com/user-attachments/assets/c03dde8d-0ed3-4247-9154-c88db2ca8a49)<br>
Top: Hardware reference; Bottom: BeesNES.  The frequency response is matched exactly.

![image](https://github.com/user-attachments/assets/1c526347-c891-44e3-8ffd-682966fab346)<br>
MDFourier.  The dip at the end is the anti-aliasing filter.

[Listen to MDFourier Test Audio](https://www.dropbox.com/scl/fi/pjjrs6j3k7vabfww8xi9h/MDFourTest.wav?rlkey=dhspadervmhr2b4vl3jldpdlc&dl=0)


NTSC-CRT library: https://github.com/LMP88959/NTSC-CRT<br>
PAL-CRT library: https://github.com/LMP88959/PAL-CRT<br>
Persune palgen: https://github.com/Gumball2415/palgen-persune

## Accuracy 
We are aiming for “Sub-Cycle Accuracy”: https://emulation.gametechwiki.com/index.php/Emulation_accuracy#Subcycle_accuracy  
	
This means that multi-byte writes are correctly partitioned across cycles and partial data updates are possible, allowing for the more esoteric features of the system to be accurately emulated.  This means we should be able to support interrupt hijacking and any other cases that rely heavily on the cycle timing of the system.  

Additional options/features to facilitate accurate emulation:  
* Start-Up: Start from known state or from random state.  Helps the random seed in some games.  
* Hardware bugs will be emulated in both their buggy and fixed states (OAMADDR bugs (writing fewer than 8 bytes on the 2C02G) are examples of this).  
* Unofficial opcodes used by games will be supported.  
* The bus will be open and correctly maintain the last floating read/write.  
* Etc.  

If behavior differes from the actual hardware result, it is considered a bug.  Hacks are to be avoided as much as possible.

The CPU should be completely sub–cycle-accurate, as every individual cycle is documented there. The same should apply to the PPU (questions surround PAL differences at the cycle level) and the APU.

Timing is not based off audio or monitor refresh rates as is done in many emulators. We use a real clock (with at-minimum microsecond accuracy) and match real timings to real time units, which we can speed up and slow down as options.  The NTSC version’s CPU will need to pump out ~29,780.506887 cycles per frame at 60.098814 FPS, while the PAL will need to pump out ~33,247.485977 cycles at 50.006979 FPS.  This means there is no noticeable visual delay (rendered frames are presented essentially immediately, rather than waiting for a monitor refresh, doing a frame’s worth of work, and then providing the visible frame after a delay) and that input is polled with exactly the same timing as in a real console, eliminating all input lag.  It should both look and _feel_ like a real console, with responsive controls that feel identical to how they do on real machines.

## Performance
There were initially some concerns that being sub–cycle-accurate would mean extra overhead—other emulators may skip useless redundant opcode fetches, but not here, and each fetch is accompanied by an entire CPU tick and all the work that goes into updating the CPU state, etc.  For this reason, most systems were implemented in an entirely branchless fashion—there are no “if”/“else” statements, “%” operations, “&” operations, “>=”/“<” checks, etc. when accessing memory; address mirroring, address mapping to registers, etc., is all handled entirely without branching, and most CPU, PPU, and APU cycles are branchless as well.  This more-than made up for the cycle-accuracy overhead. <br>
My custom filters and custom image-resizing routines are AVX/SSE-enhanced, and AVX/SSE is also used to put heavy work into audio processing while remaining blazingly fast.  On my laptop, the authentic CRT filters with 100% clean audio can run at 90 FPS, while the L. Spiro filters can run at 120-144 FPS.  Even though max settings are still able to cleanly maintain 60.098… FPS, both audio and video can be reduced in quality to run even faster.  Low-power machines should have no problems running BeesNES, even if software filters must be used.
GPU acceleration is provided for Microsoft® Direct3D® 9, Microsoft® Direct3D® 12, and Vulkan® 1.

## Other Features
Other features will include:  
* A debugger.  
* A disassembler.  
* An assembler.  
* 1-877-Tools-4-TAS.  
* * Stepping and keylogging.  
* * Movie-making.
* Ray-tracing.
* * For realistic CRT TV rendering around the game screen, and realistic illumination of the game screen onto the TV borders.
* WebCam access.
* * Real-time eye-tracking for a 3D effect on the TV border render.
* * Real-time lighting on the TV border.
* * Real-time screen reflections on the CRT glass.


## Building
BeesNES does not use any 3rd-party libraries outside of OpenAL.  Simply install the OpenAL SDK and BeesNES should build without a problem.
**Microsoft Visual Studio Community 2022 (64-bit) - Current
Version 17.4.4**
