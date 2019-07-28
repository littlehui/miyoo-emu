
Final Burn Alpha SDL for OpenDingux
-----------------------------------

FB Alpha is an arcade emulator supporting the following hardware platforms;

 - Capcom CPS-1
 - Capcom CPS-2
 - Cave
 - Neo Geo
 - Sega System 16 (and similar), System 18, X-Board, Y-Board
 - Toaplan
 - Taito Rainbow Islands/Operation Wolf/Rastan
 - Psikyo 68EC020 based hardware
 - misc stuff we like


Supported games
---------------

Refer to gamelist.txt for the list of supported roms


Version
-------

FBA SDL is based on FBA 0.2.96.86 and is intended for both Dingux/OpenDingux.
It can be run on Dingoo a320, Dingoo a380 and Ritmix rzx50. Screen size is
detected automatically.

Romset is synchronized with MAME 0.126


How to use FB Alpha
-------------------

FBA SDL is now merged with the frontend based on Capex in one application. Just
run it, press START and choose Rom Show mode: all, available or non-available.
If needed, supplementary rom paths could be set.

FBA SDL could be used as a standalone application when invoked with
rom zip with full path as a parameter:

./fbasdl.dge ./roms/dino.zip

Full path and extension are obligatory.


Commandline interface
---------------------

FBA SDL can also be invoked with command line options.
The options are as follows;

./fbasdl.dge <game> [<parameters>]

<game>                  - The game's romname with full path and extension.
--sound-sdl             - Use SDL for sound
--sound-sdl-old         - Use SDL for sound (old mutex sync)
--sound-ao              - Use libao for sound (OpenDingux only)
--no-sound              - Just be silent
--samplerate=<Hz>       - Valid values are: 11025, 22050 and 44100
--showfps               - Show frames per second while play
--68kcore=<value>       - Choose Motorola 68000 emulation core.
                          0 - C68k, 1 - Musashi M68k, 2 - A68k
--z80core=<value>       - Choose Z80 emulation core.
                          0 - cz80, 1 - mame_z80
--vsync                 - Use vertical sync for video

Example:

./fbasdl.dge ./roms/dino.zip --sound-sdl --samplerate=44100 --68kcore=0 --z80core=0


Controls and hotkeys
--------------------

Standard keymapping:
D-PAD        - D-PAD
SELECT       - Coin1
START        - Start1
SELECT+START - Start2
A            - Fire1
B            - Fire2
X            - Fire3
Y            - Fire4
L            - Fire5
R            - Fire6

L+R+Y        - Show/hide fps
L+R+A        - Quick state load
L+R+B        - Quick state save
L+R+SELECT   - Service menu
L+R+START    - FBA menu (key config and load/save states)

All fire keys could be redefined in the menu.


Usage/optimisation tips
-----------------------

If you are running on OpenDingux on Dingoo a320 you should do the following:

 - Overclock fbasdl to 408MHz (link properties)
 - Enable swap support:
   Create or edit the config file local/etc/swap.conf and add a line containing
   SWAP_SD_SIZE_MB=megabytes. The path can be changed by adding a line
   containing SWAP_SD_FILE=path.

If you are running on legacy Dingux on Dingoo a320/a380 or Ritmix rzx50
you can use the following bash script to enable swap

<<<< - cut here
    #! /bin/sh

    if [ ! -e ./fba.swp ]; then
        dd if=/dev/zero of=./.fba/fba.swp bs=2048 count=65536
    fi

    mkswap ./fba.swp
    swapon ./fba.swp

    # start actual emulator
    ./fbasdl.dge "$1" --sound-sdl --samplerate=22050

    swapoff ./fba.swp
<<<< - cut here


Thanks go to
------------

Slaanesh for not releasing FBA320 sources which gave me enough motivation to
replicate his work. :)


Source code
-----------

https://github.com/dmitrysmagin/fba-a320
https://github.com/dmitrysmagin/fba-sdl

Dmitry Smagin
exmortis [at] yandex [dot] ru

FB Alpha is written by FB Alpha Team
http://fbalpha.com/

Acknowledgements
----------------

Thanks to: Dave, ElSemi, Gangta, OG, Razoola, Logiqx, TRAC, CrashTest,
           Andrea Mazzoleni, Derek Liauw Kie Fa, Dirk Stevens, Maxim Stepin,
           the MAME team, Shawn and Craig at XGaming.

Thanks also to everyone who has made suggestions, submitted code, or helped
in any other way.
