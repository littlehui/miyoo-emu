// Ambush FB Alpha driver module
// Based on MAME driver by Zsolt Vasvari

#include "burnint.h"
#include "driver.h"
extern "C" {
#include "ay8910.h"
}

static unsigned char *Mem, *Rom, *Gfx, *Prom;
static unsigned char DrvJoy1[8], DrvJoy2[8], DrvReset, DrvDips;
static int *Palette;
static short* pFMBuffer;
static short* pAY8910Buffer[6];

static unsigned char ambush_colorbank, flipscreen;

//----------------------------------------------------------------------------------
// Inputs

static struct BurnInputInfo DrvInputList[] = {
	{"Coin 1",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 coin"},
	{"Coin 2",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 coin"},
	{"Start 1",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 start"},
	{"Start 2",		BIT_DIGITAL,	DrvJoy2 + 1,    "p2 start"},

	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 left"},
	{"P1 Right",	  	BIT_DIGITAL,	DrvJoy1 + 5,	"p1 right"},
	{"P1 Button 1", 	BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 7,	"p1 fire 2"},

	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 4,	"p2 left"},
	{"P2 Right",	  	BIT_DIGITAL,	DrvJoy2 + 5,	"p2 right"},
	{"P2 Button 1", 	BIT_DIGITAL,	DrvJoy2 + 6,	"p2 fire 1"},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 7,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Dip Switches",	BIT_DIPSWITCH,	&DrvDips,	"dip"},
};

STDINPUTINFO(Drv);

static struct BurnDIPInfo DrvDIPList[]=
{
	// Default Values
	{0x11, 0xff, 0xff, 0xc0, NULL                     },

	{0   , 0xfe, 0   , 4   , "Lives"           	  },
	{0x11, 0x01, 0x03, 0x00, "3"     		  },
	{0x11, 0x01, 0x03, 0x01, "4"    		  },
	{0x11, 0x01, 0x03, 0x02, "5"     		  },
	{0x11, 0x01, 0x03, 0x03, "6"    		  },

	{0   , 0xfe, 0   , 8   , "Coinage"	          },
	{0x11, 0x01, 0x1c, 0x14, "3 Coins 1 Credit"	  },
	{0x11, 0x01, 0x1c, 0x10, "2 Coins 1 Credit"	  },
	{0x11, 0x01, 0x1c, 0x00, "1 Coins 1 Credit"	  },
	{0x11, 0x01, 0x1c, 0x04, "1 Coin 2 Credits"	  },
	{0x11, 0x01, 0x1c, 0x18, "2 Coins 5 Credits"	  },
	{0x11, 0x01, 0x1c, 0x08, "1 Coin 3 Credits"	  },
	{0x11, 0x01, 0x1c, 0x0c, "1 Coin 4 Credits"	  },
	{0x11, 0x01, 0x1c, 0x1c, "Free Play / Service"	  },

	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x11, 0x01, 0x20, 0x00, "Easy"       		  },
	{0x11, 0x01, 0x20, 0x20, "Hard"       		  },

	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x11, 0x01, 0x40, 0x40, "80000"     		  },
	{0x11, 0x01, 0x40, 0x00, "120000"    		  },

	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x11, 0x01, 0x80, 0x80, "Upright"     		  },
	{0x11, 0x01, 0x80, 0x00, "Cocktail"    		  },
};

STDDIPINFO(Drv);

//----------------------------------------------------------------------------------
// Memory / Port handlers

unsigned char __fastcall ambush_read_byte(unsigned short a)
{
	switch (a)
	{
		case 0xa000: // watchdog reset
		break;

		case 0xc800:
			return DrvDips; // dips
	}

	return 0;
}

void __fastcall ambush_write_byte(unsigned short a, unsigned char d)
{
	switch (a)
	{
		case 0xcc00:	// nop
		case 0xcc01:
		case 0xcc02:
		case 0xcc03:
		case 0xcc07:	// coin counter

		break;

		case 0xcc04:	// flip screen
			flipscreen = d;	
		break;

		case 0xcc05:	// color bank select
			ambush_colorbank = d & 3;
		break;
	}
}
		

unsigned char __fastcall ambush_in_port(unsigned short a)
{
	switch (a & 0xff)
	{
		case 0x00: // read_0
		case 0x80: // read 1
			return AY8910Read((a >> 7) & 1); 
	}

	return 0;
}

void __fastcall ambush_out_port(unsigned short a, unsigned char d)
{
	switch (a & 0xff)
	{
		case 0x00: // control 0
		case 0x01: // write 0
		case 0x80: // control 1
		case 0x81: // write 1
			AY8910Write((a >> 7) & 1, a & 1, d);
		break;
	}
}

unsigned char AY8910_0_port0(unsigned int)
{
	unsigned char ret = 0xff;

	ret ^= DrvJoy1[7] << 0;
	ret ^= DrvJoy1[6] << 1;
	ret ^= DrvJoy2[7] << 2;
	ret ^= DrvJoy2[6] << 3;
	ret ^= DrvJoy2[1] << 4;
	ret ^= DrvJoy1[1] << 5;
	ret ^= DrvJoy2[0] << 6;
	ret ^= DrvJoy1[0] << 7;

	return ret;
}

unsigned char AY8910_1_port0(unsigned int)
{
	unsigned char ret = 0xff;

	ret ^= DrvJoy1[2] << 0;
	ret ^= DrvJoy1[3] << 1;
	ret ^= DrvJoy1[4] << 2;
	ret ^= DrvJoy1[5] << 3;
	ret ^= DrvJoy2[2] << 4;
	ret ^= DrvJoy2[3] << 5;
	ret ^= DrvJoy2[4] << 6;
	ret ^= DrvJoy2[5] << 7;

	return ret;
}

//----------------------------------------------------------------------------------
// Initilizing functions

static int DrvDoReset()
{
	ambush_colorbank = 0;

	memset(Rom + 0x8000, 0, 0x0400);
	memset(Rom + 0xc000, 0, 0x1000);

	ZetOpen(0);
	ZetReset();
	ZetClose();

	AY8910Reset(0);
	AY8910Reset(1);

	return 0;
}

static void ambush_palette_init()
{
	int i;
	unsigned char *color_prom = Prom;

	for (i = 0;i < 0x100; i++)
	{
		int bit0,bit1,bit2,r,g,b;

		/* red component */
		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* green component */
		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i] >> 4) & 0x01;
		bit2 = (color_prom[i] >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* blue component */
		bit0 = 0;
		bit1 = (color_prom[i] >> 6) & 0x01;
		bit2 = (color_prom[i] >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		Palette[i] = (r << 16) | (g << 8) | b;
	}
}

static void ambush_gfx_convert()
{
	unsigned char *tmp = (unsigned char *)malloc(0x4000);

	memcpy (tmp, Gfx, 0x4000);

	for (int i = 0; i < 0x10000; i++)
	{
		Gfx[i]  = ((tmp[0x0000 + (i >> 3)] >> (i & 7)) & 1) << 1;
		Gfx[i] |= ((tmp[0x2000 + (i >> 3)] >> (i & 7)) & 1);
	}

	free (tmp);
}

static int DrvInit()
{
	Mem = (unsigned char*)malloc(0x10000 + 0x10000 + 0x100 + 0x400);
	if (Mem == NULL) {
		return 1;
	}

	pFMBuffer = (short*)malloc(nBurnSoundLen * 6 * sizeof(short));
	if (pFMBuffer == NULL) {
		return 1;
	}

	Rom  = Mem + 0x00000;
	Gfx  = Mem + 0x10000;
	Prom = Mem + 0x20000;
	Palette = (int*)(Mem + 0x20100);

	{
		if (BurnLoadRom(Rom  + 0x0000, 0, 1)) return 1;
		if (BurnLoadRom(Rom  + 0x2000, 1, 1)) return 1;
		if (BurnLoadRom(Rom  + 0x4000, 2, 1)) return 1;
		if (BurnLoadRom(Rom  + 0x6000, 3, 1)) return 1;

		if (BurnLoadRom(Gfx  + 0x0000, 4, 1)) return 1;
		if (BurnLoadRom(Gfx  + 0x2000, 5, 1)) return 1;

		if (BurnLoadRom(Prom + 0x0000, 6, 1)) return 1;

		ambush_palette_init();
		ambush_gfx_convert();
	}

	ZetInit(1);
	ZetOpen(0);
	ZetSetInHandler(ambush_in_port);
	ZetSetOutHandler(ambush_out_port);
	ZetSetReadHandler(ambush_read_byte);
	ZetSetWriteHandler(ambush_write_byte);

	ZetMapArea(0x0000, 0x7fff, 0, Rom + 0x0000);
	ZetMapArea(0x0000, 0x7fff, 2, Rom + 0x0000);

	ZetMapArea(0x8000, 0x87ff, 0, Rom + 0x8000); // main ram
	ZetMapArea(0x8000, 0x87ff, 1, Rom + 0x8000);
	ZetMapArea(0x8000, 0x87ff, 2, Rom + 0x8000);

	ZetMapArea(0xc000, 0xc7ff, 0, Rom + 0xc000); // video ram
	ZetMapArea(0xc000, 0xc7ff, 1, Rom + 0xc000);
	ZetMemEnd();
	ZetClose();

	pAY8910Buffer[0] = pFMBuffer + nBurnSoundLen * 0;
	pAY8910Buffer[1] = pFMBuffer + nBurnSoundLen * 1;
	pAY8910Buffer[2] = pFMBuffer + nBurnSoundLen * 2;
	pAY8910Buffer[3] = pFMBuffer + nBurnSoundLen * 3;
	pAY8910Buffer[4] = pFMBuffer + nBurnSoundLen * 4;
	pAY8910Buffer[5] = pFMBuffer + nBurnSoundLen * 5;

	AY8910Init(0, 1500000, nBurnSoundRate, &AY8910_0_port0, NULL, NULL, NULL);
	AY8910Init(1, 1500000, nBurnSoundRate, &AY8910_1_port0, NULL, NULL, NULL);

	DrvDoReset();

	return 0;
}

static int DrvExit()
{
	ZetExit();
	AY8910Exit(0);
	AY8910Exit(1);

	free (pFMBuffer);
	free (Mem);

	return 0;
}

//----------------------------------------------------------------------------------
// Drawing functions

static void draw_tile(int code, int sx, int sy, int color, int flipx, int flipy, int transp, int spr)
{
	unsigned char *src = Gfx + code;

	for (int y = 7; y >= 0; y--)
	{
		for (int x = 7; x >= 0; x--, src++)
		{
			if (*src == 0 && transp) continue;
			int pxl = Palette[color | *src];

			int pos;
			if (flipy)
				pos = ((sy + 7 - y) & 0xff) * 256;
			else
				pos = ((sy + y) & 0xff) * 256;

			if (flipx)
				pos += ((sx + 7 - x) & 0xff);
			else
				pos += ((sx + x) & 0xff);

			if (spr) pos ^= 0xff00; // vertical flip for sprites

			if (pos < 0x1000 || pos > 0xefff) continue; // trim garbage
			pos -= 0x1000;

			PutPix(pBurnDraw + pos * nBurnBpp, BurnHighCol(pxl >> 16, pxl >> 8, pxl, 0));
		}
	}
}

static void draw_chars(int priority)
{
	for (int offs = 0; offs < 0x400; offs++)
	{
		int code,sx,sy,color;
		unsigned char scroll;

		sy = (offs >> 5);
		sx = (offs & 0x1f);

		color = Rom[0xc100 + ((sy & 0x1c) << 3) + sx];

		if ((color & 0x10) != priority)  continue;

		scroll = ~Rom[0xc080 + sx];

		sy = (sy << 3) + scroll;
		sx = (sx << 3);

		code = (Rom[0xc400 + offs] | ((color & 0x60) << 3)) << 6;

		color = ((ambush_colorbank << 4) | (color & 0x0f)) << 2;

		draw_tile(code, sx, sy, color, 0, 1, priority >> 4, 0);
	}
}

static void draw_sprites()
{
	for (int offs = 0x200 - 4;offs >= 0;offs -= 4)
	{
		int code,color,sx,sy,flipx,flipy;

		sy = Rom[0xc200 + offs + 0];
		sx = Rom[0xc200 + offs + 3];

		if ((sy == 0) || (sy == 0xff) || ((sx <  0x40) && (  Rom[0xc200 + offs + 2] & 0x10)) ||
		    ((sx >= 0xc0) && (!(Rom[0xc200 + offs + 2] & 0x10))))  continue;  // prevent wraparound

		code = ((Rom[0xc200 + offs + 1] & 0x3f) | ((Rom[0xc200 + offs + 2] & 0x60) << 1)) << 8;

		color = Rom[0xc200 + offs + 2] & 0x0f;
		flipx = Rom[0xc200 + offs + 1] & 0x40;
		flipy = Rom[0xc200 + offs + 1] & 0x80;

		color = (color | (ambush_colorbank << 4)) << 2;

		if (Rom[0xc200 + offs + 2] & 0x80)
		{
			// 16x16 sprites (four 8x8 tiles)
			draw_tile(code, sx + (flipx ? 8 : 0), sy + (flipy ? 0 : 8), color, flipx, flipy, 1, 1);
			code += 0x40;

			draw_tile(code, sx + (flipx ? 0 : 8), sy + (flipy ? 0 : 8), color, flipx, flipy, 1, 1);
			code += 0x40;

			draw_tile(code, sx + (flipx ? 8 : 0), sy + (flipy ? 8 : 0), color, flipx, flipy, 1, 1);
			code += 0x40;

			draw_tile(code, sx + (flipx ? 0 : 8), sy + (flipy ? 8 : 0), color, flipx, flipy, 1, 1);
			code += 0x40;
		}
		else
		{
			// 8x8 sprites
			draw_tile(code, sx + (flipx ? 8 : 0), sy + (flipy ? 8 : 0), color, flipx, flipy, 1, 1);
		}
	}
}

static int DrvDraw()
{
	// set background color
	{
		int px = BurnHighCol(Palette[0] >> 16, Palette[0] >> 8, Palette[0], 0);

		for (int i = 0; i < 0xE000; i++)
			PutPix(pBurnDraw + i * nBurnBpp, px);
	}

	// draw background
	draw_chars(0);

	// sprites
	draw_sprites();

	// draw foreground
	draw_chars(0x10);

	return 0;
}

static int DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}

	ZetOpen(0);
	ZetRun(4000000 / 60);
	ZetRaiseIrq(0);
	ZetClose();

	if (pBurnSoundOut) {
		int nSample;
		int nSegmentLength = nBurnSoundLen;
		short* pSoundBuf = pBurnSoundOut;
		if (nSegmentLength) {
			AY8910Update(0, &pAY8910Buffer[0], nSegmentLength);
			AY8910Update(1, &pAY8910Buffer[3], nSegmentLength);
			for (int n = 0; n < nSegmentLength; n++) {
				nSample  = pAY8910Buffer[0][n] >> 2;
				nSample += pAY8910Buffer[1][n] >> 2;
				nSample += pAY8910Buffer[2][n] >> 2;
				nSample += pAY8910Buffer[3][n] >> 2;
				nSample += pAY8910Buffer[4][n] >> 2;
				nSample += pAY8910Buffer[5][n] >> 2;

				if (nSample < -32768) {
					nSample = -32768;
				} else {
					if (nSample > 32767) {
						nSample = 32767;
					}
				}

				pSoundBuf[(n << 1) + 0] = nSample;
				pSoundBuf[(n << 1) + 1] = nSample;
			}
		}
	}

	if (pBurnDraw) {
		DrvDraw();
	}

	return 0;
}

//----------------------------------------------------------------------------------
// Savestates

static int DrvScan(int nAction,int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {						// Return minimum compatible version
		*pnMin = 0x029521;
	}

	if (nAction & ACB_VOLATILE) {		// Scan volatile ram		
		memset(&ba, 0, sizeof(ba));

		ba.Data	  = Rom + 0x8000;
		ba.nLen	  = 0x0800;
		ba.szName = "Work Ram";
		BurnAcb(&ba);

		memset(&ba, 0, sizeof(ba));
		ba.Data	  = Rom + 0xc000;
		ba.nLen	  = 0x1000;
		ba.szName = "Video Ram";
		BurnAcb(&ba);

		ZetScan(nAction);			// Scan Z80

		AY8910Scan(nAction, pnMin);		// Scan AY8910

		// Scan critical driver variables
		SCAN_VAR(ambush_colorbank);
		SCAN_VAR(flipscreen);
	}

	return 0;
}

//----------------------------------------------------------------------------------
// Game drivers

// Ambush

static struct BurnRomInfo ambushRomDesc[] = {
	{ "ambush.h7",    0x2000, 0xce306563, BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "ambush.g7",    0x2000, 0x90291409, BRF_ESS | BRF_PRG }, //  1
	{ "ambush.f7",    0x2000, 0xd023ca29, BRF_ESS | BRF_PRG }, //  2
	{ "ambush.e7",    0x2000, 0x6cc2d3ee, BRF_ESS | BRF_PRG }, //  3

	{ "ambush.n4",    0x2000, 0xecc0dc85, BRF_GRA },	   //  4 Graphics tiles
	{ "ambush.m4",    0x2000, 0xe86ca98a, BRF_GRA },	   //  5

	{ "a.bpr",        0x0100, 0x5f27f511, BRF_GRA },	   //  6 color PROMs

	{ "b.bpr",        0x0100, 0x1b03fd3b, BRF_GRA },	   //  7 Proms - Not used
	{ "13.bpr",	  0x0100, 0x547e970f, BRF_GRA },	   //  8
	{ "14.bpr",	  0x0100, 0x622a8ce7, BRF_GRA },	   //  9
};

STD_ROM_PICK(ambush);
STD_ROM_FN(ambush);

struct BurnDriver BurnDrvambush = {
	"ambush", NULL, NULL, "1983",
	"Ambush\0", NULL, "Nippon Amuse Co-Ltd", "Ambush",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, ambushRomInfo, ambushRomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 224, 4, 3
};


// Ambush (Tecfri)

static struct BurnRomInfo ambushtRomDesc[] = {
	{ "a1.i7",        0x2000, 0xa7cd149d, BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "a2.g7",        0x2000, 0x8328d88a, BRF_ESS | BRF_PRG }, //  1
	{ "a3.f7",        0x2000, 0x8db57ab5, BRF_ESS | BRF_PRG }, //  2
	{ "a4.e7",        0x2000, 0x4a34d2a4, BRF_ESS | BRF_PRG }, //  3

	{ "fa2.n4",       0x2000, 0xe7f134ba, BRF_GRA },	   //  4 Graphics tiles
	{ "fa1.m4",       0x2000, 0xad10969e, BRF_GRA },	   //  5

	{ "a.bpr",        0x0100, 0x5f27f511, BRF_GRA },	   //  6 color PROMs

	{ "b.bpr",        0x0100, 0x1b03fd3b, BRF_GRA },	   //  7 Proms - Not used
	{ "13.bpr",	  0x0100, 0x547e970f, BRF_GRA },	   //  8
	{ "14.bpr",	  0x0100, 0x622a8ce7, BRF_GRA },	   //  9
};

STD_ROM_PICK(ambusht);
STD_ROM_FN(ambusht);

struct BurnDriver BurnDrvambusht = {
	"ambusht", "ambush", NULL, "1983",
	"Ambush (Tecfri)\0", NULL, "Tecfri", "Ambush",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S,
	NULL, ambushtRomInfo, ambushtRomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 224, 4, 3
};


// Ambush (Volt Elec co-ltd)

static struct BurnRomInfo ambushvRomDesc[] = {
	{ "n1_h7.bin",    0x2000, 0x3c0833b4, BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "ambush.g7",    0x2000, 0x90291409, BRF_ESS | BRF_PRG }, //  1
	{ "ambush.f7",    0x2000, 0xd023ca29, BRF_ESS | BRF_PRG }, //  2
	{ "ambush.e7",    0x2000, 0x6cc2d3ee, BRF_ESS | BRF_PRG }, //  3

	{ "ambush.n4",    0x2000, 0xecc0dc85, BRF_GRA },	   //  4 Graphics tiles
	{ "ambush.m4",    0x2000, 0xe86ca98a, BRF_GRA },	   //  5

	{ "a.bpr",        0x0100, 0x5f27f511, BRF_GRA },	   //  6 color PROMs

	{ "b.bpr",        0x0100, 0x1b03fd3b, BRF_GRA },	   //  7 Proms - Not used
	{ "13.bpr",	  0x0100, 0x547e970f, BRF_GRA },	   //  8
	{ "14.bpr",	  0x0100, 0x622a8ce7, BRF_GRA },	   //  9
};

STD_ROM_PICK(ambushv);
STD_ROM_FN(ambushv);

struct BurnDriver BurnDrvambushv = {
	"ambushv", "ambush", NULL, "1983",
	"Ambush (Volt Elec co-ltd)\0", NULL, "Volt Elec co-ltd", "Ambush",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S,
	NULL, ambushvRomInfo, ambushvRomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 224, 4, 3
};


