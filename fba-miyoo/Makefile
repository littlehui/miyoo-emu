# Makefile for FBA, for use with GNU make
#
# The first pass makes sure all intermediary targets are present. The second pass updates
# any targets, if necessary. (Intermediary) targets which have their own unique rules
# are generated as required.

#
#	Flags. Uncomment any of these declarations to enable their function.
#

# Specify the name of the executable file, without ".exe"
NAME = fbasdl

# Check for changes in header files
DEPEND = 1

# Include features for debugging drivers
#DEBUG = 1

# Perl is available
PERL = 1

# Choose emulation cores
BUILD_A68K = 1
BUILD_C68K = 1
#BUILD_M68K = 1
#USE_LIBAO = 1

#
#	Declare variables
#

# Make a special build, pass the quoted text as comment (use FORCE_UPDATE declaration below to force recompilation of resources)
# SPECIALBUILD = "This text will appear in the property sheet of the .exe file"

ifndef	CPUTYPE
	CPUTYPE	= i686
endif

ifeq	($(CPUTYPE),i686)
	ppro = ppro
endif

ifneq	($(CPUTYPE),i686)
	NAME := $(NAME)$(CPUTYPE)
endif

#
#	Specify paths/files
#

objdir	= obj/mingw/
srcdir	= src/

alldir	=	burn \
			burn/capcom \
			burn/cave \
			burn/cps3 \
			burn/misc \
			burn/misc/dec0 \
			burn/misc/post90s \
			burn/misc/pre90s \
			burn/konami \
			burn/neogeo \
			burn/pgm \
			burn/psikyo \
			burn/sega \
			burn/taito \
			burn/toaplan \
			cpu/a68k \
			cpu/c68k \
			cpu/cz80 \
			cpu/i8039 \
			cpu/m68k \
			cpu/m6502 \
			cpu/nec \
			cpu/sh2 \
			cpu/z80 \
			generated \
			sdl-dingux

incdir	= $(foreach dir,$(alldir),-I$(srcdir)$(dir)) -I$(objdir)generated -I$(srcdir) -I/local/include -I/local/include/SDL

ifeq ($(OS),Windows_NT)
lib = -static -lstdc++ -lmingw32 -Wl,-Bdynamic -lSDL -lSDL_image -lz
else
lib = -lstdc++ -lSDL -lSDL_image -lz
endif

ifdef USE_LIBAO
	lib += -lao
endif

drvobj	=	\
		d_dodonpachi.o d_donpachi.o d_esprade.o d_feversos.o d_gaia.o d_guwange.o \
		d_hotdogst.o d_mazinger.o d_metmqstr.o d_pwrinst2.o d_sailormn.o d_uopoko.o \
		\
		d_cps1.o \
		\
		d_cps2.o \
		\
		d_neogeo.o \
		\
		d_pgm.o \
		\
		d_hangon.o d_outrun.o d_sys16a.o d_sys16b.o d_sys18.o d_xbrd.o d_ybrd.o \
		\
	        d_batrider.o d_batsugun.o d_battleg.o d_bbakraid.o d_dogyuun.o d_hellfire.o d_kbash.o \
		d_mahoudai.o d_outzone.o d_shippumd.o d_snowbro2.o d_tekipaki.o d_truxton.o \
		d_truxton2.o d_vfive.o d_zerowing.o \
		\
		d_mia.o d_tmnt.o \
		\
		d_taitof2.o d_taitox.o d_taitoz.o d_taitomisc.o d_darius2.o \
		\
		d_4enraya.o d_1942.o d_1943.o d_ambush.o d_arkanoid.o d_bankp.o d_bionicc.o d_bombjack.o \
		d_dotrikun.o d_epos.o d_exedexes.o d_funkybee.o d_gberet.o d_gunsmoke.o d_higemaru.o \
		d_jack.o d_kyugo.o d_madgear.o d_meijinsn.o d_minivdr.o d_mole.o d_mrdo.o d_mrflea.o \
		d_mystston.o d_pacman.o d_pkunwar.o d_pooyan.o d_prehisle.o d_quizo.o d_route16.o d_scregg.o \
		d_solomon.o d_sys1.o d_tigerheli.o d_tnzs.o d_vulgus.o d_wallc.o d_wc90.o \
		\
		d_1945kiii.o d_aerofgt.o d_biomtoy.o d_ddragon3.o d_drtomy.o d_fstarfrc.o d_gaiden.o \
		d_galpanic.o d_hyperpac.o d_kaneko16.o d_m90.o d_m92.o d_news.o d_ohmygod.o d_powerins.o \
		d_raiden.o d_seta2.o d_shadfrce.o d_silkroad.o d_tumbleb.o d_wwfwfest.o irem_cpu.o \
		\
		d_parent.o \
		\
		#d_psikyo.o \
		#\
		#d_baddudes.o d_robocop.o \
		#\
		#d_megadrive.o
		#\
		#d_cps3.o \

ifdef USE_LIBAO
	depobj = ao_audio.o 
endif

depobj	+=	\
		bzip.o config.o drv.o font.o input.o main.o paths.o \
		run.o tchar.o \
		\
		sdl_audio.o sdl_input.o sdl_menu.o sdl_progress.o \
		sdl_run.o sdl_video.o snd.o \
		\
		unzip.o dat.o state.o zipfn.o \
		\
		gui_config.o gui_gfx.o gui_main.o gui_romlist.o gui_setpath.o \
		\
		$(drvobj) \
		\
		burn.o cheat.o burn_gun.o load.o \
		\
		sek.o zet.o vez.o eeprom_93cxx.o \
		burn_sound.o burn_sound_c.o timer.o \
		burn_ym2151.o burn_ym3812.o burn_ym2608.o burn_ym2610.o burn_ymf278b.o burn_ym2203.o burn_ym2612.o \
		ay8910.o ym2151.o fm.o fmopl.o ymdeltat.o \
		dac.o ics2115.o msm5205.o msm6295.o rf5c68.o segapcm.o sn76496.o upd7759.o ymf278b.o ymz280b.o \
		x1010.o \
		\
		i8039.o m6502.o m6502_intf.o nec.o sh2.o z80.o z80daisy.o cz80.o \
		\
		cave.o cave_tile.o cave_sprite.o cave_palette.o \
		\
		cps.o cps_config.o cps_draw.o cps_mem.o cps_obj.o cps_pal.o cps_run.o \
		cps2_crpt.o cps_rw.o cps_scr.o cpsr.o cpsrd.o \
		cpst.o ctv.o ps.o ps_m.o ps_z.o qs.o qs_c.o qs_z.o \
		kabuki.o \
		\
		neogeo.o neo_run.o neo_decrypt.o neo_text.o neo_sprite.o neo_palette.o neo_upd4990a.o \
		rom_save.o \
		\
		pgm_crypt.o pgm_draw.o pgm_prot.o pgm_run.o \
		\
		sys16_run.o sys16_gfx.o sys16_fd1094.o fd1089.o fd1094.o mc8123.o sys16_ppi.o genesis_vid.o \
		\
		toaplan.o toa_gp9001.o toa_extratext.o toa_palette.o \
		\
		toa_bcu2.o toaplan1.o \
		\
		K052109.o tmnt_inp.o tmnt_pal.o tmnt_run.o tmnt_til.o \
		\
		tc0110pcr.o tc0220ioc.o tc0150rod.o tc0140syt.o tc0100scn.o tc0510nio.o tc0480scp.o tc0360pri.o \
		tc0280grd.o pc080sn.o pc090oj.o cchip.o taito_ic.o taito.o \
		\
		tiles_generic.o \
		#\
		#megadrive.o
		#\
		#cps3run.o cps3snd.o \
		#\
		#psikyo_tile.o psikyo_sprite.o psikyo_palette.o \
		#\
		#dec_aud.o dec_misc.o dec_vid.o \


autobj += $(depobj)

ifdef	BUILD_A68K
a68k.o	= $(objdir)cpu/a68k/a68k.o
endif

ifdef	BUILD_C68K
autobj += c68k.o
endif

autdep	= $(autobj:.o=.d)

driverlist.h = $(srcdir)generated/driverlist.h
ctv.h	= $(srcdir)generated/ctv.h
toa_gp9001_func.h = $(srcdir)generated/toa_gp9001_func.h
neo_sprite_func.h = $(srcdir)generated/neo_sprite_func.h
cave_tile_func.h = $(srcdir)generated/cave_tile_func.h
cave_sprite_func.h = $(srcdir)generated/cave_sprite_func.h
psikyo_tile_func.h = $(srcdir)generated/psikyo_tile_func.h
#pgm_sprite.h = $(srcdir)generated/pgm_sprite.h
#build_details.h = $(srcdir)generated/build_details.h

ifdef BUILD_M68K
allobj = $(objdir)cpu/m68k/m68kcpu.o $(objdir)cpu/m68k/m68kopnz.o $(objdir)cpu/m68k/m68kopdm.o $(objdir)cpu/m68k/m68kopac.o $(objdir)cpu/m68k/m68kops.o
endif

allobj += \
	  $(foreach file,$(autobj:.o=.c), \
		$(foreach dir,$(alldir),$(subst $(srcdir),$(objdir), \
		$(firstword $(subst .c,.o,$(wildcard $(srcdir)$(dir)/$(file))))))) \
	  $(foreach file,$(autobj:.o=.cpp), \
		$(foreach dir,$(alldir),$(subst $(srcdir),$(objdir), \
		$(firstword $(subst .cpp,.o,$(wildcard $(srcdir)$(dir)/$(file))))))) \
	  $(foreach file,$(autobj:.o=.asm), \
		$(foreach dir,$(alldir),$(subst $(srcdir),$(objdir), \
		$(firstword $(subst .asm,.o,$(wildcard $(srcdir)$(dir)/$(file)))))))

ifdef BUILD_A68K
allobj += $(a68k.o)
endif

alldep	= $(foreach file,$(autobj:.o=.c), \
		$(foreach dir,$(alldir),$(subst $(srcdir),$(objdir), \
		$(firstword $(subst .c,.d,$(wildcard $(srcdir)$(dir)/$(file))))))) \
	  $(foreach file,$(autobj:.o=.cpp), \
		$(foreach dir,$(alldir),$(subst $(srcdir),$(objdir), \
		$(firstword $(subst .cpp,.d,$(wildcard $(srcdir)$(dir)/$(file)))))))

#
#
#	Specify compiler/linker/assembler
#
#

HOSTCC = gcc
HOSTCXX = g++
CC	= gcc
CXX	= g++
LD	= $(CXX)
AS	= nasm

HOSTCFLAGS = $(incdir)
CFLAGS   = -O2 -fomit-frame-pointer -Wno-write-strings \
		-DLSB_FIRST
CXXFLAGS = -O2 -fomit-frame-pointer -Wno-write-strings \
		-DLSB_FIRST

ifneq ($(OS),Windows_NT)
CFLAGS += -D__cdecl="" -D__fastcall=""
CXXFLAGS += -D__cdecl="" -D__fastcall=""
endif

ifdef USE_LIBAO
	CFLAGS += -DUSE_LIBAO
	CXXFLAGS += -DUSE_LIBAO
endif

DEF = -DCPUTYPE=$(CPUTYPE) -DBUILD_SDL -DUSE_SPEEDHACKS -DOOPSWARE_FIX

ifdef SPECIALBUILD
	DEF += -DSPECIALBUILD=$(SPECIALBUILD)
endif

ifdef	DEBUG
	DEF += -DFBA_DEBUG -g
	HOSTCFLAGS += -DFBA_DEBUG -g
endif

ifdef BUILD_A68K
	DEF += -DBUILD_A68K
endif

ifdef BUILD_C68K
	DEF += -DBUILD_C68K
endif

ifdef BUILD_M68K
	DEF += -DBUILD_M68K
endif

DEF += -DFILENAME=$(NAME)

ifdef PROFILE
	CFLAGS	+= -pg
else
	LDFLAGS	=
endif

ifdef DEBUG
	CFLAGS		+= -g
	CXXFLAGS	+= -g
else
	LDFLAGS		+= -s
endif

CFLAGS += $(DEF) $(incdir)
CXXFLAGS += $(DEF) $(incdir)
LDFLAGS += 
ASFLAGS = -O1

ifeq ($(OS),Windows_NT)
ASFLAGS += -f coff
else
ASFLAGS += -f elf
endif

#
#
#	Specify paths
#
#

vpath %.asm	$(foreach dir,$(alldir),$(srcdir)$(dir)/ )
vpath %.cpp	$(foreach dir,$(alldir),$(srcdir)$(dir)/ )
vpath %.c	$(foreach dir,$(alldir),$(srcdir)$(dir)/ )
vpath %.h	$(foreach dir,$(alldir),$(srcdir)$(dir)/ )

vpath %.o 	$(foreach dir,$(alldir),$(objdir)$(dir)/ )
vpath %.d 	$(foreach dir,$(alldir),$(objdir)$(dir)/ )

#
#
#	Rules
#
#

.PHONY: all init cleandep clean

ifeq ($(MAKELEVEL),0)
ifdef DEPEND

all:	init $(autdep) $(autobj)
	@$(MAKE) -f Makefile
else

all:	init $(autobj)
	@$(MAKE) -f Makefile
endif
else

all:	$(NAME)

endif

#
#
#	Rule for linking the executable
#
#

ifeq ($(MAKELEVEL),1)

$(NAME):	$(allobj)
	@echo
	@echo Linking executable $(NAME)...
	@mkdir -p bin
	@$(LD) $(CFLAGS) $(LDFLAGS) -o bin/$@ $^ $(lib)
	@mkdir -p bin/skin
	@cp src/sdl-dingux/skin/*.png bin/skin/
endif

#
#	Generate the gamelist
#

burn.o burn.d:	driverlist.h

$(driverlist.h): $(drvobj) $(srcdir)scripts/gamelist.pl
ifdef	PERL
	@perl $(srcdir)scripts/gamelist.pl -o $@ -l gamelist.txt \
		$(filter %.cpp,$(foreach file,$(drvobj:.o=.cpp),$(foreach dir,$(alldir), \
		$(firstword $(wildcard $(srcdir)$(dir)/$(file))))))
else
ifeq ($(MAKELEVEL),1)
	@echo
	@echo Warning: Perl is not available on this system.
	@echo $@ cannot be updated or created!
	@echo
endif
endif

#
#	Compile 68000 cores
#

# A68K

ifdef	BUILD_A68K
$(a68k.o):	fba_make68k.c
	@echo Compiling A68K MC68000 core...
	@$(HOSTCC) $(HOSTCFLAGS) $(LDFLAGS) -DWIN32 -Wno-unused -Wno-conversion -Wno-missing-prototypes \
		-s $< -o $(objdir)generated/fba_make68k
	@$(objdir)generated/fba_make68k $(@:.o=.asm) \
		$(@D)/a68k_tab.asm 00 $(ppro)
	@echo Assembling A68K MC68000 core...
	@$(AS) $(ASFLAGS) $(@:.o=.asm) -o $@
endif

# Musashi

ifdef BUILD_M68K
$(objdir)cpu/m68k/m68kcpu.o: $(srcdir)cpu/m68k/m68kcpu.c $(objdir)generated/m68kops.h $(srcdir)cpu/m68k/m68k.h $(srcdir)cpu/m68k/m68kconf.h
	@echo Compiling Musashi MC680x0 core \(m68kcpu.c\)...
	@$(CC) $(CFLAGS) -c $(srcdir)cpu/m68k/m68kcpu.c -o $(objdir)cpu/m68k/m68kcpu.o

$(objdir)cpu/m68k/m68kops.o: $(objdir)cpu/m68k/m68kmake $(objdir)generated/m68kops.h $(objdir)generated/m68kops.c $(srcdir)cpu/m68k/m68k.h $(srcdir)cpu/m68k/m68kconf.h
	@echo Compiling Musashi MC680x0 core \(m68kops.c\)...
	@$(CC) $(CFLAGS) -c $(objdir)generated/m68kops.c -o $(objdir)cpu/m68k/m68kops.o

$(objdir)cpu/m68k/m68kopac.o: $(objdir)cpu/m68k/m68kmake $(objdir)generated/m68kops.h $(objdir)generated/m68kopac.c $(srcdir)cpu/m68k/m68k.h $(srcdir)cpu/m68k/m68kconf.h
	@echo Compiling Musashi MC680x0 core \(m68kopac.c\)...
	@$(CC) $(CFLAGS) -c $(objdir)generated/m68kopac.c -o $(objdir)cpu/m68k/m68kopac.o

$(objdir)cpu/m68k/m68kopdm.o: $(objdir)cpu/m68k/m68kmake $(objdir)generated/m68kops.h $(objdir)generated/m68kopdm.c $(srcdir)cpu/m68k/m68k.h $(srcdir)cpu/m68k/m68kconf.h
	@echo Compiling Musashi MC680x0 core \(m68kopdm.c\)...
	@$(CC) $(CFLAGS) -c $(objdir)generated/m68kopdm.c -o $(objdir)cpu/m68k/m68kopdm.o

$(objdir)cpu/m68k/m68kopnz.o: $(objdir)cpu/m68k/m68kmake $(objdir)generated/m68kops.h $(objdir)generated/m68kopnz.c $(srcdir)cpu/m68k/m68k.h $(srcdir)cpu/m68k/m68kconf.h
	@echo Compiling Musashi MC680x0 core \(m68kopnz.c\)...
	@$(CC) $(CFLAGS) -c $(objdir)generated/m68kopnz.c -o $(objdir)cpu/m68k/m68kopnz.o

$(objdir)generated/m68kops.h: $(objdir)cpu/m68k/m68kmake $(srcdir)cpu/m68k/m68k_in.c
	$(objdir)/cpu/m68k/m68kmake $(objdir)generated/ $(srcdir)cpu/m68k/m68k_in.c

$(objdir)cpu/m68k/m68kmake: $(srcdir)cpu/m68k/m68kmake.c
	@echo Compiling Musashi MC680x0 core \(m68kmake.c\)...
	@$(HOSTCC) $(HOSTCFLAGS) $(srcdir)cpu/m68k/m68kmake.c -o $(objdir)cpu/m68k/m68kmake
endif

#
#	Extra rules for generated header file cvt.h, needed by ctv.cpp
#

ctv.d ctv.o:	$(ctv.h)

$(ctv.h):	ctv_make.cpp
	@echo Generating $(srcdir)generated/$(@F)...
	@$(HOSTCXX) $(LDFLAGS) $< -o $(objdir)generated/ctv_make
	@$(objdir)generated/ctv_make >$@

#
#	Extra rules for generated header file toa_gp9001_func.h, needed by toa_gp9001.cpp
#

toa_bcu2.d toa_bcu2.o toa_gp9001.d toa_gp9001.o: $(toa_gp9001_func.h)

$(toa_gp9001_func.h):	$(srcdir)scripts/toa_gp9001_func.pl
	@$(srcdir)scripts/toa_gp9001_func.pl -o $(toa_gp9001_func.h)

#
#	Extra rules for generated header file neo_sprite_func.h, needed by neo_sprite.cpp
#

neo_sprite.d neo_sprite.o: $(neo_sprite_func.h)

$(neo_sprite_func.h):	$(srcdir)scripts/neo_sprite_func.pl
	@$(srcdir)scripts/neo_sprite_func.pl -o $(neo_sprite_func.h)

#
#	Extra rules for generated header file cave_tile_func.h, needed by cave_tile.cpp
#

cave_tile.d cave_tile.o: $(cave_tile_func.h)

$(cave_tile_func.h):	$(srcdir)scripts/cave_tile_func.pl
	@perl $(srcdir)scripts/cave_tile_func.pl -o $(cave_tile_func.h)

#
#	Extra rules for generated header file cave_sprite_func.h, needed by cave_sprite.cpp
#

cave_sprite.d cave_sprite.o: $(cave_sprite_func.h)

$(cave_sprite_func.h):	$(srcdir)scripts/cave_sprite_func.pl
	@perl $(srcdir)scripts/cave_sprite_func.pl -o $(cave_sprite_func.h)

#
#	Extra rules for generated header file psikyo_tile_func.h / psikyo_sprite_func.h, needed by psikyo_tile.cpp / psikyo_sprite.cpp
#

psikyo_tile.d psikyo_tile.o psikyosprite.d psikyo_sprite.o: $(psikyo_tile_func.h)

$(psikyo_tile_func.h):	$(srcdir)scripts/psikyo_tile_func.pl
	@perl $(srcdir)scripts/psikyo_tile_func.pl -o $(psikyo_tile_func.h)

#
#	Extra rules for generated header file pgm_sprite.h, needed by pgm_draw.cpp
#

#pgm_draw.d pgm_draw.o:	$(pgm_sprite.h)

#$(pgm_sprite.h):	pgm_sprite_create.cpp
#	@echo Generating $(srcdir)generated/$(@F)...
#	@$(HOSTCXX) $(LDFLAGS) $< -o $(objdir)dep/generated/pgm_sprite_create
#	@$(objdir)dep/generated/pgm_sprite_create >$@

ifeq ($(MAKELEVEL),1)
ifdef DEPEND

include	$(alldep)

endif
endif

#
#	Generic rules for C/C++ files
#

ifeq ($(MAKELEVEL),0)

%.o:	%.cpp
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) -c $< -o $(subst $(srcdir),$(objdir),$(<D))/$(@F)

%.o:	%.c
	@echo Compiling $<...
	@$(CC) $(CFLAGS) -Wno-unused -Wno-conversion -Wno-missing-prototypes -c $< -o $(subst $(srcdir),$(objdir),$(<D))/$(@F)

%.o:	%.asm
	@echo Assembling $<...
	@$(AS) $(ASFLAGS) $< -o $(subst $(srcdir),$(objdir),$(<D))/$(@F)

else

%.o:	%.c
	@echo Compiling $<...
	@$(CC) $(CFLAGS) -Wno-unused -Wno-conversion -Wno-missing-prototypes -c $< -o $@

%.o:	%.asm
	@echo Assembling $<...
	@$(AS) $(ASFLAGS) $< -o $@

%.o:
	@echo Compiling $<...
	@$(CC) $(CXXFLAGS) -c $< -o $@

endif

#
#	Generate dependencies for C/C++ files
#

ifdef DEPEND

%.d:	%.c
	@echo Generating depend file for $<...
	@$(CC) -MM -MT "$(subst $(srcdir),$(objdir),$(<D))/$(*F).o $(subst $(srcdir),$(objdir),$(<D))/$(@F)" -x c++ $(CFLAGS) $< >$(subst $(srcdir),$(objdir),$(<D))/$(@F)

%.d:	%.cpp
	@echo Generating depend file for $<...
	@$(CXX) -MM -MT "$(subst $(srcdir),$(objdir),$(<D))/$(*F).o $(subst $(srcdir),$(objdir),$(<D))/$(@F)" -x c++ $(CXXFLAGS) $< >$(subst $(srcdir),$(objdir),$(<D))/$(@F)

endif

#
#	Phony targets
#

init:

ifdef	DEBUG
	@echo Making debug build...
else
	@echo Making normal build...
endif
	@echo
	@mkdir -p $(foreach dir, $(alldir),$(objdir)$(dir))
	@mkdir -p $(srcdir)generated

cleandep:
	@echo Removing depend files from $(objdir)...
	@for dir in $(alldir); do rm -f $(objdir)$$dir/*.d; done

clean:
	@echo Removing all files from $(objdir)...
	@rm -f -r $(objdir)
	@rm -f -r $(ctv.h)

ifdef	PERL
	@echo Removing all files generated with perl scripts...
	@rm -f -r $(driverlist)
endif
	@echo Removing executable file...
	@rm -f $(NAME)

#
#	Rule to force recompilation of any target that depends on it
#

FORCE:
