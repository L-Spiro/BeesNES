# BeesNES
A sub–cycle-accurate Nintendo Entertainment System emulator.
<br>Shawn (L. Spiro) Wilcoxen  

## Description
A “sub–cycle-accurate” Nintendo Entertainment System emulator with the goal of being as authentic of an experience as possible.  It should look, sound, and _feel_ like real hardware, with convincing visuals, clean and accurate audio, and real-time input response.  No visual or audible delays.  BeesNES also represents the under-served regions with support for a wide range of console variants, currently including NTSC, PAL, PAL “Dendy” Famiclone, PAL-M Brazilian Famiclone, and PAL-N Argentinian Famiclone.

## Visual Samples
![image](https://user-images.githubusercontent.com/7362666/215368977-2cec6ea5-c09e-4824-99e5-0afe3b76409f.png)
![image](https://github.com/L-Spiro/BeesNES/assets/7362666/bab6ad83-b7ee-4835-894b-a905741efda8)
![image](https://user-images.githubusercontent.com/7362666/215369800-608a6db6-fddc-4a46-9b5f-77c501adab5a.png)
![image](https://user-images.githubusercontent.com/7362666/215370725-3092a546-b8f7-488b-ae4e-8d7c7f108cad.png)
![image](https://user-images.githubusercontent.com/7362666/215370366-33903c20-0e75-489a-bb4d-571b08f33bee.png)
![image](https://user-images.githubusercontent.com/7362666/215371089-3480dc0a-a80c-4cc3-8ca4-4a957b25fd0e.png)
![image](https://user-images.githubusercontent.com/7362666/215371867-63a951cb-303a-4222-8094-6a20b5b9999b.png)
![image](https://user-images.githubusercontent.com/7362666/215371958-b742960a-ec5f-47f8-8b8a-7dc55162ffb5.png)
![image](https://github.com/L-Spiro/BeesNES/assets/7362666/0b615d51-0bde-419f-bf91-76e7c91ae991)
![image](https://user-images.githubusercontent.com/7362666/216515134-d5c67d0a-eb4b-4571-84a0-df58dd4a0659.png)
<img width="2560" height="1440" alt="image" src="https://github.com/user-attachments/assets/a1ce1de5-7c26-48b3-acbb-ce845792a355" />
<img width="2560" height="1440" alt="image" src="https://github.com/user-attachments/assets/b11cfb08-0d7d-4176-9fc2-d8e96e550c43" /><br>
Authentic Phospher Decay:
![image](https://github.com/user-attachments/assets/1d0e8df2-2f8d-4363-aa55-4ba60d0f5743)
![image](https://github.com/user-attachments/assets/7afa4747-34f2-44a6-b266-4a47393fb6a8)
![image](https://github.com/user-attachments/assets/ecdda2bd-585a-4907-9f8f-7080841ce8c6)
![image](https://github.com/user-attachments/assets/0d14087a-9489-4f74-b1bf-b24fa7afd9c8)




RF Cables:<br>
![image](https://github.com/L-Spiro/BeesNES/assets/7362666/3a596c53-168b-48bd-ace4-00b262c8e10f)<br>
Composite:<br>
![image](https://github.com/user-attachments/assets/6a5adb3b-8400-4b57-943d-1282ab76b222)<br>
HDMI:<br>
![image](https://github.com/L-Spiro/BeesNES/assets/7362666/8732d426-64cc-4c7c-9238-e9f70cf6bf26)<br>
HDMI Mod:<br>
![image](https://github.com/L-Spiro/BeesNES/assets/7362666/692e989b-ec8a-4266-913a-ebb921e5e60e)<br>

PAL (Composite:):<br>
![image](https://github.com/user-attachments/assets/68e7131a-437f-47f8-8f35-6b95ebc7cfd4)<br>
PAL-B (“Dendy” Famiclone) (Composite):<br>
![image](https://github.com/user-attachments/assets/bf9e8302-88fd-456b-946c-3fd7e772e824)<br>
PAL-M (Brazilian Famiclone) (Composite):<br>
![image](https://github.com/user-attachments/assets/4ee47323-d23d-46e0-91ae-3d52569c2584)<br>
PAL-N (Argentinian Famiclone) (Composite):<br>
![image](https://github.com/user-attachments/assets/89777f31-5b1d-410e-a80c-1347c6114731)<br>


## Audio Accuracy
![image](https://github.com/user-attachments/assets/38cff73e-e3e6-4d3e-81ee-99d7ee0af7a9)<br>
A wide array of options is available for tailoring the audio to specific hardware devices.

![image](https://github.com/user-attachments/assets/3b3e5ddd-b1e2-414f-beff-bfdce2ba7891)<br>
Top: Hardware reference; Bottom: BeesNES.  The missing area hasn’t been implemented yet, but everything that has been implemented matches hardware exactly, minus the noise.

![image](https://github.com/user-attachments/assets/5c1727b8-e1cd-4c36-88a4-9684375f2752)<br>
Top: Hardware reference; Bottom: BeesNES.  Excluding mains-hum noise and the unsupported feature, the hardware reference is matched down to the tiniest details.  This is actual unedited output from BeesNES (no additional filtering).  Note the lack of aliasing in the high frequencies.

![image](https://github.com/user-attachments/assets/c03dde8d-0ed3-4247-9154-c88db2ca8a49)<br>
Top: Hardware reference; Bottom: BeesNES.  The frequency response is matched exactly.

![image](https://github.com/user-attachments/assets/1c526347-c891-44e3-8ffd-682966fab346)<br>
MDFourier.  The dip at the end is the anti-aliasing filter.

[Listen to MDFourier Test Audio](https://www.dropbox.com/scl/fi/pjjrs6j3k7vabfww8xi9h/MDFourTest.wav?rlkey=dhspadervmhr2b4vl3jldpdlc&dl=0)

## Videos
YouTube Video: Castlevania Demo Play (Low Noise)<br>
[![Watch the video](https://img.youtube.com/vi/HyLtecKOjLM/maxresdefault.jpg)](https://www.youtube.com/watch?v=HyLtecKOjLM&list=PLM2QRzvCtV12TZcpXrUm1LQnyCgHy5Uxa&index=7)<br>


YouTube Video: 1943: The Battle of Midway (RF Cables)<br>
[![Watch the video](https://img.youtube.com/vi/DeiT0dbBs44/maxresdefault.jpg)](https://www.youtube.com/watch?v=DeiT0dbBs44&list=PLM2QRzvCtV12TZcpXrUm1LQnyCgHy5Uxa&index=19)<br>


YouTube Video: Probotector (PAL, Composite Cables)<br>
[![Watch the video](https://img.youtube.com/vi/5qk1oU4lAKU/maxresdefault.jpg)](https://www.youtube.com/watch?v=5qk1oU4lAKU&list=PLM2QRzvCtV12TZcpXrUm1LQnyCgHy5Uxa&index=20)<br>


YouTube Video: Battletoads Opening (Extreme Noise)<br>
[![Watch the video](https://img.youtube.com/vi/K3sVkZFxkvs/maxresdefault.jpg)](https://www.youtube.com/watch?v=K3sVkZFxkvs&list=PLM2QRzvCtV12TZcpXrUm1LQnyCgHy5Uxa&index=6)<br>


YouTube Video: Double Dragon (Composite Cables)<br>
[![Watch the video](https://img.youtube.com/vi/C-4q72KqOzM/maxresdefault.jpg)](https://www.youtube.com/watch?v=C-4q72KqOzM&list=PLM2QRzvCtV12TZcpXrUm1LQnyCgHy5Uxa&index=21)<br>


YouTube Video: Akira Opening (Extreme Noise)<br>
[![Watch the video](https://img.youtube.com/vi/mSZlMw0cPEY/maxresdefault.jpg)](https://www.youtube.com/watch?v=mSZlMw0cPEY&list=PLM2QRzvCtV12TZcpXrUm1LQnyCgHy5Uxa&index=4)<br>


YouTube Video: Battletoads (RF Cables)<br>
[![Watch the video](https://img.youtube.com/vi/A-8DUre9LXA/maxresdefault.jpg)](https://www.youtube.com/watch?v=A-8DUre9LXA&list=PLM2QRzvCtV12TZcpXrUm1LQnyCgHy5Uxa&index=22)<br>

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
My custom filters and custom image-resizing routines are AVX/SSE-enhanced, and AVX/SSE is also used to put heavy work into audio processing while remaining blazingly fast.  On my laptop, the authentic CRT filters with 100% clean audio can run at 90 FPS, while the L. Spiro filters can run at 120-144 FPS.  Even though max settings are still able to cleanly maintain 60.098… FPS, both audio and video can be reduced in quality to run even faster.  Low-power machines should have no problems running BeesNES, and this is all still running in software.  GPU support will eventually make everything even faster.

## Other Features
Other features will include:  
* A debugger.  
* A disassembler.  
* An assembler.  
* 1-877-Tools-4-TAS.  
* * Stepping and keylogging.  
* * Movie-making.


## Building
BeesNES does not use any 3rd-party libraries outside of OpenAL.  Simply install the OpenAL SDK and BeesNES should build without a problem.
**Microsoft Visual Studio Community 2022 (64-bit) - Current
Version 17.4.4**
