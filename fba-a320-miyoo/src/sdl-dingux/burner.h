/*
 * FinalBurn Alpha for Dingux/OpenDingux
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _BURNER_H_
#define _BURNER_H_

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

#ifndef MAX_PATH
#define MAX_PATH 511
#endif

#define BZIP_MAX (8)								// Maximum zip files to search through
#define DIRS_MAX (8)								// Maximum number of directories to search

// Define macros for appliction title and description
#ifdef FBA_DEBUG
 #define APP_TITLE "FB Alpha [DEBUG]"
#else
 #define APP_TITLE "FB Alpha"
#endif

#define APP_DESCRIPTION "Emulator for arcade games"

#include "tchar.h"
#include "version.h"
#include "burn.h"

// ---------------------------------------------------------------------------
// OS independent functionality

#define _s(A) #A
#define _a(A) _s(A)
#define VERSION _a(VER_MAJOR.VER_MINOR.VER_BETA.VER_ALPHA)

// Macros for parsing text
#define SKIP_WS(s) while (_istspace(*s)) { s++; }			// Skip whitespace
#define FIND_WS(s) while (*s && !_istspace(*s)) { s++; }	// Find whitespace
#define FIND_QT(s) while (*s && *s != _T('\"')) { s++; }	// Find quote

// dat.cpp
char* DecorateGameName(unsigned int nBurnDrv);
int write_datfile(int nDatType, FILE* fDat);
int create_datfile(TCHAR* szFilename, int nDatType);

// state.cpp
int BurnStateLoadEmbed(FILE* fp, int nOffset, int bAll, int (*pLoadGame)());
int BurnStateLoad(const char * szName, int bAll, int (*pLoadGame)());
int BurnStateSaveEmbed(FILE* fp, int nOffset, int bAll);
int BurnStateSave(const char * szName, int bAll);
extern int nSavestateSlot;
int StatedLoad(int nSlot);
int StatedSave(int nSlot);


// zipfn.cpp
struct ZipEntry { char* szName;	unsigned int nLen; unsigned int nCrc; };

int ZipOpen(char* szZip);
int ZipClose();
int ZipGetList(struct ZipEntry** pList, int* pnListCount);
int ZipLoadFile(unsigned char* Dest, int nLen, int* pnWrote, int nEntry);

// bzip.cpp
#define BZIP_STATUS_OK		(0)
#define BZIP_STATUS_BADDATA	(1)
#define BZIP_STATUS_ERROR	(2)

int BzipOpen(bool);
int BzipClose();
int BzipInit();
int BzipExit();
int BzipStatus();

// paths.cpp
extern char szAppHomePath[MAX_PATH];
extern char szAppSavePath[MAX_PATH];
extern char szAppConfigPath[MAX_PATH];
extern char szAppSamplesPath[MAX_PATH]; // for burn/snd/samples.cpp
extern char szAppPreviewPath[MAX_PATH];
extern char szAppRomPaths[DIRS_MAX][MAX_PATH];

void BurnPathsInit();

// config.cpp
typedef struct
{
	int sound;
	int samplerate;
	int vsync;
	int rescale;
	int rotate;
	int showfps;
	int frameskip;
	int m68kcore;
	int z80core;
	int sense;
	int create_lists;
} CFG_OPTIONS;

typedef struct
{
	int up;
	int down;
	int left;
	int right;
	int fire1;
	int fire2;
	int fire3;
	int fire4;
	int fire5;
	int fire6;
	int coin1;
	int coin2;
	int start1;
	int start2;
	int pause;
	int quit;
	int qsave;
	int qload;
} CFG_KEYMAP;

extern CFG_OPTIONS options;
extern CFG_KEYMAP keymap;

int ConfigAppLoad();
int ConfigAppSave();
void ConfigGameDefault();
int ConfigGameLoad();
int ConfigGameSave();

// drv.cpp
extern int bDrvOkay; // 1 if the Driver has been initted okay, and it's okay to use the BurnDrv functions
int DrvInit(int nDrvNum, bool bRestore);
int DrvInitCallback(); // Used when Burn library needs to load a game. DrvInit(nBurnSelect, false)
int DrvExit();

// main.cpp
extern char szAppBurnVer[16];

// run.cpp
extern bool bShowFPS;
extern bool bPauseOn;
int RunReset();
int RunOneFrame(bool bDraw, int fps);

// input.cpp
extern int nAnalogSpeed;
int InpInit();
int InpExit();
void InpDIP();

#endif // _BURNER_H_
