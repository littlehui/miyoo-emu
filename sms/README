
 SMS+SDL (Yet another version of SMS Plus...)
 ----------------------------------------------------------------------------
 http://membres.lycos.fr/cyxdown/smssdl/
 2003.07.21


 Based on version 0.9.4a
 Emulator Core by Charles Mac Donald
 SDL Version by Gregory Montoir (cyx@frenchkiss.net)

 
 [What is it]

 SMS Plus is Sega Master System and Game Gear emulator. Originally,
 it was written to run under DOS. Since Charles Mac Donald released
 his emulator under the GPL terms, this emulator has been ported to
 different platforms (Windows, BeOS... refer to Charles' website
 to have the complete list). 
 Because I wanted to play with the SDL library and emulators, I began
 to write a SDL version of this emulator.
 If you want to have technical information about the emulation engine,
 refer to the original 'README.TXT', since I made no changes to the 
 emulation core. I only 'ported' it to SDL compliant platforms.
 I won't explain you how to run it, if you know what emulation is, you
 should be able to help yourself.


 [Features]

 - Emulation of Game Gear and Master System
 - Slots for saving/loading
 - Sound
 - Frameskip option for slow machines
 - Filtered rendering for fast machines


 [Hotkeys]

 F1:    screenshot in BMP format
 F2:    save state
 F3:    load state
 F4/F5: dec/inc frame skip value
 F6/F7: dec/inc state slot
 1..8:  switch rendering filter
 C:     button 1
 V:     button 2
 ENTER: start (GG) / pause (SMS)
 ARROW: directional pad
 TAB:   console hard reset


 [Compiling]

 You need SDL-1.2.X (maybe older versions will be fine), and a C compiler. 
 For the moment, I only tested GCC and Microsoft Visual C++ 6.

 GCC (MinGW seems ok, also): 
 A makefile is provided. Just cd to sdl/, type make and cross your fingers.
  
 Visual C++:
 Open the workspace sms_sdl.dsw. Enter the path to the SDL include files 
 in Tools|Options|Directories. Now it should compile.


 [Changelog]

 release 7.1 (2003/07/21)
  - stupid bug in joystick code fixed

 release 7 (2003/07/18)
  - zip'ed roms support (w/ zlib)
  - re-added joystick support
  - applied patch from Thomas Huth
    - fix filters issues on big endian machines 
    - may fix sound problems
  - added DotMatrix filter

 release 6 (2003/01/09)
  - updated core with latest version (0.9.4a)
  - compiled using SDL-1.2.5
  - rewritten the SDL sound code
  - added TV2x filter

 release 5 (2002/04/28)
  - compiled using SDL-1.2.4
  - merged with Jari's SDL port :
    - remapped/added some keys
    - new synchronization code
    - joystick support
  - added AdvanceMAME Scale2x effect
  - integrated 2xSaI engine :
    - 2xSaI effect
    - Super2xSaI effect
    - SuperEagle effect
  - added scanlines 100% effect
  - rewritten the SDL sound code
  - Gondos updated the MMX scanlines 50% effect

 release 4 (2002/03/18)
  - changed SDL sound code (thanks to Caz)
  - added scanlines 50% effect
  - added MMX scanlines 50% effect (thanks to Gondos)
  - changed Makefiles
  - added another audio core (based on SEAL), it sounds good now :)

 release 3 (2002/02/28)
  - sound quality is still quite bad but a little better
  - added screenshot feature (press F4)
  - when saving/loading, a message is printed to the console
  - bilinear filtering is a command line switch now
  - fixed the start button that wasn't updated after one press
  - fixed black gaps around screen in game gear mode

 release 2 (2002/02/15) 
  - added bilinear filtering (define USE_BILINEAR_FILTERING)
  - various fixes

 release 1 (2002/01/29)
  - first public release


 [Todo]

  - optimize and add bicubic filter
  - allow the user to configure his controls (keyboard & joystick)


 [Thanks]
  
 Charles Mac Donald - orignal author 
   http://cgfm2.emuviews.com/
 The SDL Library team
   http://www.libsdl.org/
 A. Schiffler - SDL_Framerate from SDL_gfx-2.0.3 (used in releases 1-4)
   http://www.ferzkopp.net/
 Carlos Hasan - Synthetic Audio Library (SEAL) (used in release 4)
   http://www.egerter.com/seal/
 Derek Liauw Kie Fa and The Snes9x team - 2xSaI engine
   http://www.snes9x.com/
 AdvanceMAME team - Scale2x effect specification
   http://advancemame.sourceforge.net/
 the zlib team
   http://www.gzip.org/zlib/ 

 Contributors:
   Jari Karppinen - original Linux/SDL port
   Thomas Huth    - fixes for sound / big endian issues


 [Legal stuff]

 Refer to the original 'README.TXT'.
