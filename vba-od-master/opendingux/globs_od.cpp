#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include <SDL/SDL.h>

#include "shared.h"
#include "gamesettings.h"

#include "../vba/Util.h"
#include "../vba/common/Port.h"
#include "../vba/common/Patch.h"
#include "../vba/common/SoundSDL.h"
#include "../vba/gba/Flash.h"
#include "../vba/gba/RTC.h"
#include "../vba/gba/Sound.h"
#include "../vba/gba/Cheats.h"
#include "../vba/gba/GBA.h"
#include "../vba/gba/agbprint.h"
#include "../vba/gb/gb.h"
#include "../vba/gb/gbGlobals.h"
#include "../vba/gb/gbCheats.h"
#include "../vba/gb/gbSound.h"

int speed;

static u32 start;

int cartridgeType = 0;
u32 RomIdCode;
char RomTitle[17];

int emulating = 0;

int systemFrameSkip = 0;
int systemVerbose = 0;

int SunBars = 3;

static bool cartridgeRumble = false, possibleCartridgeRumble = false;

struct EmulatedSystem emulator =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	false,
	0
};

/****************************************************************************
 * Read time
 ***************************************************************************/
unsigned long SDL_UXTimerRead(void) {
	struct timeval tval; // timing
  
  	gettimeofday(&tval, 0);
	
	return (((tval.tv_sec*1000000) + (tval.tv_usec )));
}

/****************************************************************************
 * VBA Globals
 ***************************************************************************/
int systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

int systemRedShift = 0;
int systemBlueShift = 0;
int systemGreenShift = 0;
int systemColorDepth = 0;
u16 systemGbPalette[24];
#ifdef _VIDOD16_
u16 systemColorMap16[0x10000];
#endif
#ifdef _VIDOD32_
u32 systemColorMap32[0x10000];
#endif
//u32 *systemColorMap32 = NULL;

/****************************************************************************
* System
****************************************************************************/
static int srcWidth = 0;
static int srcHeight = 0;
static int srcPitch = 0;

int frameskipadjust = 0;

void systemGbPrint(u8 *data,int pages,int feed,int palette, int contrast) {}
void debuggerOutput(const char *s, u32 addr) {}
void (*dbgOutput)(const char *s, u32 addr) = debuggerOutput;
void systemMessage(int num, const char *msg, ...) {}

// Returns number of milliseconds since program start
u32 systemGetClock( void )
{
	return (SDL_UXTimerRead()-start)/1000;
}

void systemFrame() {
	pastFPS++;
	newTick = SDL_UXTimerRead();
	if ((newTick-lastTick)>1000000) {
		FPS = pastFPS;
		pastFPS = 0;
		lastTick = newTick;
	}
}

void systemScreenCapture(int a) {}
void systemShowSpeed(int speed) {}
void systemGbBorderOn() {}

void systemDrawScreen() {
#ifdef _VIDOD32_
	unsigned int *buffer_scr = (unsigned int *) actualScreen->pixels;
#else
	unsigned short *buffer_scr = (unsigned short *) actualScreen->pixels;
#endif
	unsigned int W,H,ix,iy,x,y,xfp,yfp;
	static char buffer[32];

	if(SDL_MUSTLOCK(actualScreen)) SDL_LockSurface(actualScreen);
	
	if (GameConf.m_ScreenRatio) { // Full screen
		x=0;
		y=0; 
		W=320;
		H=240;
		ix=(srcWidth<<16)/W;
		iy=(srcHeight<<16)/H;
		xfp=300-52;yfp=1;

		do   
		{
#ifdef _VIDOD32_
			unsigned int *buffer_mem=(unsigned int *) (pix+((y>>16)*srcPitch));
#else
			unsigned short *buffer_mem=(unsigned short *) (pix+((y>>16)*srcPitch));
#endif
			W=320; x=0;
			do {
				//prefetch(buffer_scr+1, 1);
				*buffer_scr++=buffer_mem[x>>16];
				x+=ix;
			} while (--W);
			y+=iy;
		} while (--H);
	}
	else { // Original show
		x=((actualScreen->w - srcWidth)/2);
		y=((actualScreen->h - srcHeight)/2); 
		W=srcWidth;
		H=srcHeight;
		ix=(srcWidth<<16)/W;
		iy=(srcHeight<<16)/H;
		xfp = (x+srcWidth)-72;yfp = y+1;
		
#ifdef _VIDOD32_
		buffer_scr += (y)*actualScreen->pitch/4;
#else
		buffer_scr += (y)*actualScreen->pitch/2;
#endif
		buffer_scr += (x);
		do   
		{
#ifdef _VIDOD32_
			unsigned int *buffer_mem=(unsigned int *) (pix+((y>>16)*srcPitch));
#else
			unsigned short *buffer_mem=(unsigned short *) (pix+((y>>16)*srcPitch));
#endif
			W=srcWidth; x=((actualScreen->w - srcWidth)/2);
			do {
				//prefetch(buffer_scr+1, 1);
				*buffer_scr++=buffer_mem[x>>16];
				x+=ix;
			} while (--W);
			y+=iy;
			buffer_scr +=  (actualScreen->w - srcWidth);
		} while (--H);
	}

	if (GameConf.m_DisplayFPS) {
		sprintf(buffer,"%03d %02d %02d",speed, systemFrameSkip, FPS);
		print_string_video(xfp,yfp,buffer);
	}

	if (SDL_MUSTLOCK(actualScreen)) SDL_UnlockSurface(actualScreen);
	SDL_Flip(actualScreen);
}

static u32 lastTime = 0;
#define RATE60HZ 166666 // 1/6 second or 166666.67 usec
//#define RATE60HZ 16666 // 1/6 second or 166666.67 usec

void system10Frames(void)
{
	unsigned long time = SDL_UXTimerRead();
	unsigned long diff = (time-lastTime);
	if (diff == 0) diff = RATE60HZ;

	// expected diff - actual diff
	long timeOff = RATE60HZ - diff;

	if(timeOff > 0 && timeOff < 100000) // we're running ahead!
		usleep(timeOff); // let's take a nap

	/*ALEK int */ speed = (RATE60HZ/diff)*100;
	if (cartridgeType == 2) // GBA games require frameskipping
	{
		// consider increasing skip
		if(speed < 60)
			systemFrameSkip += 4;
		else if(speed < 70)
			systemFrameSkip += 3;
		else if(speed < 80)
			systemFrameSkip += 2;
		else if(speed < 98)
			++systemFrameSkip;

		// consider decreasing skip
		else if(speed > 185)
			systemFrameSkip -= 3;
		else if(speed > 145)
			systemFrameSkip -= 2;
		//else if(speed > 125)
		//	systemFrameSkip -= 1;
		else if(speed > 98)
			systemFrameSkip -= 1;

		// correct invalid frame skip values
		if(systemFrameSkip > 20)
			systemFrameSkip = 20;
		else if(systemFrameSkip < 0)
			systemFrameSkip = 0;
	}

	lastTime = SDL_UXTimerRead();
}

bool systemPauseOnFrame()
{
	return false;
}

void systemInitialisePalette()
{
	int i;
	// Build GBPalette
	for( i = 0; i < 24; )
	{
		systemGbPalette[i++] = (0x1f) | (0x1f << 5) | (0x1f << 10);
		systemGbPalette[i++] = (0x15) | (0x15 << 5) | (0x15 << 10);
		systemGbPalette[i++] = (0x0c) | (0x0c << 5) | (0x0c << 10);
		systemGbPalette[i++] = 0;
	}
	// Set palette etc - Fixed to RGB565
#ifdef _VIDOD32_
	systemColorDepth = 32;
	systemRedShift = 19;
	systemGreenShift = 11;
	systemBlueShift = 3;
	for(i = 0; i < 0x10000; i++)
	{
		systemColorMap32[i] =
			((i & 0x1f) << systemRedShift) |
			(((i & 0x3e0) >> 5) << systemGreenShift) |
			(((i & 0x7c00) >> 10) << systemBlueShift);
	}
#else
	systemColorDepth = 16;
	systemRedShift = 11;
	systemGreenShift = 6;
	systemBlueShift = 0;
	for(i = 0; i < 0x10000; i++)
	{
		systemColorMap16[i] =
			((i & 0x1f) << systemRedShift) |
			(((i & 0x3e0) >> 5) << systemGreenShift) |
			(((i & 0x7c00) >> 10) << systemBlueShift);
	}
#endif
}

/****************************************************************************
* sound
****************************************************************************/
SoundDriver * systemSoundInit()
{
	soundShutdown();

	return new SoundSDL();
}

void systemOnSoundShutdown()
{
}

void systemOnWriteDataToSoundBuffer(const u16 * finalWave, int length)
{
}

bool systemCanChangeSoundQuality()
{
  return false;
}

/****************************************************************************
* joysticks
****************************************************************************/
#define VBA_BUTTON_A		1
#define VBA_BUTTON_B		2
#define VBA_BUTTON_SELECT	4
#define VBA_BUTTON_START	8
#define VBA_RIGHT			16
#define VBA_LEFT			32
#define VBA_UP				64
#define VBA_DOWN			128
#define VBA_BUTTON_R		256
#define VBA_BUTTON_L		512

bool systemReadJoypads()
{
	return true;
}

u32 systemReadJoypad(int which)
{
	u32 resJ = 0;
	if(which == -1) which = 0; // default joypad

	if (keys[SDLK_UP] == SDL_PRESSED)  { resJ |= VBA_UP; } // UP
	if (keys[SDLK_DOWN] == SDL_PRESSED) { resJ |= VBA_DOWN; } // DOWN
	if (keys[SDLK_LEFT] == SDL_PRESSED) { resJ |= VBA_LEFT; } // LEFT
	if (keys[SDLK_RIGHT] == SDL_PRESSED) { resJ |= VBA_RIGHT; } // RIGHT

	if (keys[SDLK_LCTRL] == SDL_PRESSED) { resJ |=  VBA_BUTTON_A;  }  // BUTTON A
	if (keys[SDLK_LALT] == SDL_PRESSED) { resJ |=  VBA_BUTTON_B; }  // BUTTON B

	if (keys[SDLK_SPACE] == SDL_PRESSED) { resJ |=  VBA_BUTTON_A;  }  // BUTTON X -> A
	if (keys[SDLK_LSHIFT] == SDL_PRESSED)  { resJ |=  VBA_BUTTON_B;  }   // BUTTON Y -> VBA_BUTTON_B

	if (keys[SDLK_BACKSPACE] == SDL_PRESSED)  { resJ |=  VBA_BUTTON_R;  }  // BUTTON R
	if (keys[SDLK_TAB] == SDL_PRESSED)  { resJ |=  VBA_BUTTON_L;  }  // BUTTON L

	if ((keys[SDLK_ESCAPE] == SDL_PRESSED) && (keys[SDLK_RETURN] == SDL_PRESSED )) { 
		m_Flag = GF_MAINUI;
	}
	else if ( (keys[SDLK_RETURN] == SDL_PRESSED) )  { resJ |=  VBA_BUTTON_START;  } // START
	else if ( (keys[SDLK_ESCAPE] == SDL_PRESSED) ) { resJ |=  VBA_BUTTON_SELECT; } // SELECT
		
	return resJ;
}

/****************************************************************************
* Motion/Tilt sensor
* Used for games like:
* - Yoshi's Universal Gravitation
* - Kirby's Tilt-N-Tumble
* - Wario Ware Twisted!
*
****************************************************************************/
static int sensorX = 2047;
static int sensorY = 2047;
static int sensorWario = 0x6C0;
static u8 sensorDarkness = 0xE8; // total darkness (including daylight on rainy days)
bool CalibrateWario = false;

int systemGetSensorX()
{
	return sensorX;
}

int systemGetSensorY()
{
	return sensorY;
}

int systemGetSensorZ()
{
	if (CalibrateWario) return 0x6C0;
	else return sensorWario;
}


void updateRumble() {}

void systemPossibleCartridgeRumble(bool RumbleOn) {
	possibleCartridgeRumble = RumbleOn;
	updateRumble();
}

void systemCartridgeRumble(bool RumbleOn) {
	cartridgeRumble = RumbleOn;
	possibleCartridgeRumble = false;
	updateRumble();
}

u8 systemGetSensorDarkness()
{
	return sensorDarkness;
}

void systemUpdateSolarSensor()
{
	u8 sun = 0x0; //sun = 0xE8 - 0xE8 (case 0 and default)

	switch (SunBars)
	{
		case 1:
			sun = 0xE8 -  0xE0;
			break;
		case 2:
			sun = 0xE8 -  0xDA;
			break;
		case 3:
			sun = 0xE8 -  0xD0;
			break;
		case 4:
			sun = 0xE8 -  0xC8;
			break;
		case 5:
			sun = 0xE8 -  0xC0;
			break;
		case 6:
			sun = 0xE8 -  0xB0;
			break;
		case 7:
			sun = 0xE8 -  0xA0;
			break;
		case 8:
			sun = 0xE8 -  0x88;
			break;
		case 9:
			sun = 0xE8 -  0x70;
			break;
		case 10:
			sun = 0xE8 -  0x50;
			break;
		default:
			break;
	}

	struct tm *newtime;
	time_t long_time;

	// regardless of the weather, there should be no sun at night time!
	time(&long_time); // Get time as long integer.
	newtime = localtime(&long_time); // Convert to local time.
	if (newtime->tm_hour > 21 || newtime->tm_hour < 5)
	{
		sun = 0; // total darkness, 9pm - 5am
	}
	else if (newtime->tm_hour > 20 || newtime->tm_hour < 6)
	{
		sun /= 9; // almost total darkness 8pm-9pm, 5am-6am
	}
	else if (newtime->tm_hour > 18 || newtime->tm_hour < 7)
	{
		sun >>= 1;
	}

	sensorDarkness = 0xE8 - sun;
}

void systemUpdateMotionSensor()
{
	systemUpdateSolarSensor();
}

/****************************************************************************
* memory management
****************************************************************************/
void vba_memclose(void) {
	if (vram != NULL) { free(vram); vram = NULL; }
	if (paletteRAM != NULL) { free(paletteRAM); paletteRAM = NULL; }
	if (internalRAM != NULL) { free(internalRAM); internalRAM = NULL; }
	if (workRAM != NULL) { free(workRAM); workRAM = NULL; }
	if (gbRom != NULL) { free(gbRom); gbRom = NULL; }
	if (bios != NULL) { free(bios); bios = NULL; }
	if (pix != NULL) { free(pix); pix = NULL; }
	if (oam != NULL) { free(oam); oam = NULL; }
	if (ioMem != NULL) { free(ioMem); ioMem = NULL; }
	if (rom != NULL) { free(rom); rom = NULL; }
}

bool vba_memopen( void )
{
	ioMem = (u8 *)calloc(1, 0x400);
	oam = (u8 *)calloc(1, 0x400);
	pix = (u8 *)calloc(1, 4 * 241 * 162);
	bios = (u8 *)calloc(1,0x4000);
	workRAM = (u8 *)calloc(1, 0x40000);
	internalRAM = (u8 *)calloc(1,0x8000);
	paletteRAM = (u8 *)calloc(1,0x400);
	vram = (u8 *)calloc(1, 0x20000);
	
	if(workRAM == NULL || bios == NULL || internalRAM == NULL ||
		paletteRAM == NULL || vram == NULL || oam == NULL ||
		pix == NULL || ioMem == NULL )
	{
		vba_memclose();
		return false;
	}
	
	return true;
}

extern bool gbUpdateSizes();

bool vba_loadgbcard(char *szFileCard) 
{
	FILE *romfile;

	gbRom = (u8 *)malloc(1024*1024*4); // allocate 4 MB to GB ROM
	bios = (u8 *)calloc(1,0x100);
	
	gbRomSize = 0;
	
	romfile = fopen(szFileCard, "rb");
	if (romfile == NULL)
	{
		return false;
	}
	
	 // Seek to end of file, and get size
    fseek(romfile, 0, SEEK_END);
    gbRomSize = ftell(romfile);
    fseek(romfile, 0, SEEK_SET);

	fread(gbRom, 1, gbRomSize, romfile);
	fclose(romfile);
	
	gameCRC = crc32(0, gbRom, gbRomSize);
	
	if(gbRomSize <= 0)
		return false;

	return gbUpdateSizes();
}

bool vba_loadgbacard(char *szFileCard) 
{
	FILE *romfile;
	int GBAROMSize;
	
	vba_memclose();
	vba_memopen();
	GBAROMSize = 0;
	
	romfile = fopen(szFileCard, "rb");
	if (romfile == NULL)
	{
		return false;
	}
	
	rom = (u8 *)malloc(0x2000000);
	if (rom == NULL) 
	{
		fclose(romfile);
		return false;
	}
	 // Seek to end of file, and get size
    fseek(romfile, 0, SEEK_END);
    GBAROMSize = ftell(romfile);
    fseek(romfile, 0, SEEK_SET);

	fread(rom, 1, GBAROMSize, romfile);
	fclose(romfile);
	
	gameCRC = crc32(0, rom, GBAROMSize);

	flashInit();
	eepromInit();
	CPUUpdateRenderBuffers( true );
	
	return true;
}

static bool ValidGameId(u32 id)
{
	if (id == 0)
		return false;
	for (unsigned i = 1u; i <= 4u; ++i)
	{
		u8 b = id & 0xFF;
		id >>= 8;
		if (!(b >= 'A' && b <= 'Z') && !(b >= '0' && b <= '9'))
			return false;
	}
	return true;
}

static void gbApplyPerImagePreferences()
{
	// Only works for some GB Colour roms
	u8 Colour = gbRom[0x143];
	if (Colour == 0x80 || Colour == 0xC0)
	{
		RomIdCode = gbRom[0x13f] | (gbRom[0x140] << 8) | (gbRom[0x141] << 16)
				| (gbRom[0x142] << 24);
		if (!ValidGameId(RomIdCode))
			RomIdCode = 0;
	}
	else
		RomIdCode = 0;
	// Otherwise we need to make up our own code
	RomTitle[15] = '\0';
	RomTitle[16] = '\0';
	if (gbRom[0x143] < 0x7F && gbRom[0x143] > 0x20)
		strncpy(RomTitle, (const char *) &gbRom[0x134], 16);
	else
		strncpy(RomTitle, (const char *) &gbRom[0x134], 15);

#if 0
	if (RomIdCode == 0)
	{
		if (strcmp(RomTitle, "ZELDA") == 0)
			RomIdCode = LINKSAWAKENING;
		else if (strcmp(RomTitle, "MORTAL KOMBAT") == 0)
			RomIdCode = MK1;
		else if (strcmp(RomTitle, "MORTALKOMBATI&II") == 0)
			RomIdCode = MK12;
		else if (strcmp(RomTitle, "MORTAL KOMBAT II") == 0)
			RomIdCode = MK2;
		else if (strcmp(RomTitle, "MORTAL KOMBAT 3") == 0)
			RomIdCode = MK3;
		else if (strcmp(RomTitle, "MORTAL KOMBAT 4") == 0)
			RomIdCode = MK4;
		else if (strcmp(RomTitle, "SUPER MARIOLAND") == 0)
			RomIdCode = MARIOLAND1;
		else if (strcmp(RomTitle, "MARIOLAND2") == 0)
			RomIdCode = MARIOLAND2;
		else if (strcmp(RomTitle, "METROID2") == 0)
			RomIdCode = METROID2;
		else if (strcmp(RomTitle, "MARBLE MADNESS") == 0)
			RomIdCode = MARBLEMADNESS;
		else if (strcmp(RomTitle, "TMNT FOOT CLAN") == 0)
			RomIdCode = TMNT1;
		else if (strcmp(RomTitle, "TMNT BACK FROM") == 0 || strcmp(RomTitle, "TMNT 2") == 0)
			RomIdCode = TMNT2;
		else if (strcmp(RomTitle, "TMNT3") == 0)
			RomIdCode = TMNT3;
		else if (strcmp(RomTitle, "CASTLEVANIA ADVE") == 0)
			RomIdCode = CVADVENTURE;
		else if (strcmp(RomTitle, "CASTLEVANIA2 BEL") == 0)
			RomIdCode = CVBELMONT;
		else if (strcmp(RomTitle, "CASTLEVANIA") == 0 || strcmp(RomTitle, "CV3 GER") == 0)
			RomIdCode = CVLEGENDS;
		else if (strcmp(RomTitle, "STAR WARS") == 0)
			RomIdCode = SWEP4;
		else if (strcmp(RomTitle, "EMPIRE STRIKES") == 0)
			RomIdCode = SWEP5;
		else if (strcmp(RomTitle, "SRJ DMG") == 0)
			RomIdCode = SWEP6;
	}
	// look for matching palettes if a monochrome gameboy game
	// (or if a Super Gameboy game, but the palette will be ignored later in that case)
	if ((Colour != 0x80) && (Colour != 0xC0))
	{
		if (GCSettings.colorize && strcmp(RomTitle, "MEGAMAN") != 0)
			SetPalette(RomTitle);
		else
			StopColorizing();
	}
#endif

}

static void gbaApplyPerImagePreferences()
{
	// look for matching game setting
	int snum = -1;
	RomIdCode = rom[0xac] | (rom[0xad] << 8) | (rom[0xae] << 16) | (rom[0xaf] << 24);
	RomTitle[0] = '\0';

	for(int i=0; i < gameSettingsCount; ++i)
	{
		if(gameSettings[i].gameID[0] == rom[0xac] &&
			gameSettings[i].gameID[1] == rom[0xad] &&
			gameSettings[i].gameID[2] == rom[0xae] &&
			gameSettings[i].gameID[3] == rom[0xaf])
		{
			snum = i;
			break;
		}
	}

	// match found!
	if(snum >= 0)
	{
		if(gameSettings[snum].rtcEnabled >= 0)
			rtcEnable(gameSettings[snum].rtcEnabled);
		if(gameSettings[snum].flashSize > 0)
			flashSetSize(gameSettings[snum].flashSize);
		if(gameSettings[snum].saveType >= 0)
			cpuSaveType = gameSettings[snum].saveType;
		if(gameSettings[snum].mirroringEnabled >= 0)
			mirroringEnable = gameSettings[snum].mirroringEnabled;
	}
	// In most cases this is already handled in GameSettings, but just to make sure:
	switch (rom[0xac])
	{
		case 'F': // Classic NES
			cpuSaveType = 1; // EEPROM
			mirroringEnable = 1;
			break;
		case 'K': // Accelerometers
			cpuSaveType = 4; // EEPROM + sensor
			break;
		case 'R': // WarioWare Twisted style sensors
		case 'V': // Drill Dozer
			rtcEnableWarioRumble(true);
			break;
		case 'U': // Boktai solar sensor and clock
			rtcEnable(true);
			break;
	}
}

void gbgbaApplyScreenPreferences(int type, bool sgb) {
	if (type ==2) { // it's gba
		srcWidth = 240;
		srcHeight = 160;
	}
	else { // it's gb or gbc
		if(sgb || gbBorderOn)
		{
			srcWidth = 256;
			srcHeight = 224;
			gbBorderLineSkip = 256;
			gbBorderColumnSkip = 48;
			gbBorderRowSkip = 40;
			gbBorderOn = 1;
		}
		else
		{
			srcWidth = 160;
			srcHeight = 144;
			gbBorderLineSkip = 160;
			gbBorderColumnSkip = 0;
			gbBorderRowSkip = 0;
			gbBorderOn = 0;
		}
	}
	if(systemColorDepth == 16) {
		srcPitch = srcWidth*2 + 4;
	} 
	else {
		srcPitch = srcWidth*4 + 4;
	}
}


/****************************************************************************
* loading rom
****************************************************************************/
bool vba_init(char * szFile) {
	cartridgeType = 0;
	bool loaded = false;

	// image type (checks file extension)
	if(utilIsGBAImage(szFile))
		cartridgeType = 2;
	else if(utilIsGBImage(szFile))
		cartridgeType = 1;

	// leave before we do anything
	if(cartridgeType != 1 && cartridgeType != 2)
	{
		// Not zip gba agb gbc cgb sgb gb mb bin elf or dmg!
		return false;
	}
	
	vba_memclose(); // cleanup GBA memory
	gbCleanUp(); // cleanup GB memory

	switch(cartridgeType)
	{
		case 2:
			emulator = GBASystem;
			loaded = vba_loadgbacard(szFile);
			soundInit();
			soundSetSampleRate(44100); 
			cpuSaveType = 0;
			gbgbaApplyScreenPreferences(2,false);
			break;

		case 1:
			emulator = GBSystem;
			loaded = vba_loadgbcard(szFile);
			soundInit();
			soundSetSampleRate(44100);
			gbgbaApplyScreenPreferences(1,gbBorderOn ? true : false);
			break;
	}

	if(!loaded)
	{
		return false;
	}
	else
	{
		if (cartridgeType == 1)
		{
			gbGetHardwareType();

			// used for the handling of the gb Boot Rom
			//if (gbHardware & 5)
			//gbCPUInit(gbBiosFileName, useBios);

			//ALEK LoadPatch();

			// Apply preferences specific to this game
			gbApplyPerImagePreferences();

			gbSoundReset();
			gbSoundSetDeclicking(true);
			gbReset();
		}
		else
		{
			// Set defaults
			cpuSaveType = 0; // automatic
			flashSetSize(0x10000); // 64K saves
			rtcEnable(false);
			agbPrintEnable(false);
			mirroringEnable = false;

			// Apply preferences specific to this game
			gbaApplyPerImagePreferences();
			doMirroring(mirroringEnable);

			soundReset();
			CPUInit(NULL, false);
			//ALEK LoadPatch();
			CPUReset();
		}

		emulating = 1;
	}
	
	// reset frameskip variables
	lastTime = systemFrameSkip = 0;

	// Start system clock
	start = SDL_UXTimerRead();

	return true;
}
