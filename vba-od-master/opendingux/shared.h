#ifndef SHARED_H
#define SHARED_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>

#include <SDL/SDL.h>

// defines and macros
#define MAX__PATH 1024
#define FILE_LIST_ROWS 19

#define SYSVID_WIDTH	240
#define SYSVID_HEIGHT	160

#define GF_GAMEINIT    1
#define GF_MAINUI      2
#define GF_GAMEQUIT    3
#define GF_GAMERUNNING 4

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define PIX_TO_RGB(fmt, r, g, b) (((r>>fmt->Rloss)<<fmt->Rshift)| ((g>>fmt->Gloss)<<fmt->Gshift)|((b>>fmt->Bloss)<<fmt->Bshift))

// VBA dependencies
#include "../vba/System.h"
#include "../vba/gb/gbGlobals.h"

extern struct EmulatedSystem emulator;
extern u8 *rom;

#define cartridge_IsLoaded() ( (rom != NULL) || (gbRom != NULL))

typedef struct {
  unsigned int sndLevel;
  unsigned int m_ScreenRatio; // 0 = original show, 1 = full screen
  unsigned int OD_Joy[12]; // each key mapping
  unsigned int m_DisplayFPS;
  char current_dir_rom[MAX__PATH];
  unsigned int m_SuperGB;
} gamecfg;

extern unsigned int m_Flag;

extern SDL_Surface* screen;						// Main program screen
extern SDL_Surface* actualScreen;						// Main program screen

extern SDL_Surface *layer,*layerback,*layerbackgrey;

extern SDL_Event event;
extern unsigned char *keys;

extern gamecfg GameConf;

extern char gameName[512];
extern char current_conf_app[MAX__PATH];

extern unsigned int gameCRC;

extern unsigned long newTick,lastTick;
extern int FPS; 
extern int pastFPS; 

extern void system_loadcfg(char *cfg_name);
extern void system_savecfg(char *cfg_name);

//extern unsigned long crc32 (unsigned int crc, const unsigned char *buf, unsigned int len);

extern unsigned long SDL_UXTimerRead(void);

extern bool vba_init(char * szFile);
extern void systemInitialisePalette();
extern void gbgbaApplyScreenPreferences(int type, bool sgb);

// menu
extern void screen_showtopmenu(void);
extern void print_string_video(int x, int y, const char *s);

#endif
