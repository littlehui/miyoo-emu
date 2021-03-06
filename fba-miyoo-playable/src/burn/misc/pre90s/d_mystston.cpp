// FB Alpha Mysterious Stones Driver Module
// Based on MAME driver by Nicola Salmoria

#include "tiles_generic.h"
#include "m6502.h"
#include "driver.h"
extern "C" {
#include "ay8910.h"
}


static unsigned char *Mem, *Rom, *Gfx0, *Gfx1, *Gfx2, *Prom;
static unsigned char DrvJoy1[8], DrvJoy2[8], DrvReset, DrvDips[2];
static short *pAY8910Buffer[6], *pFMBuffer = NULL;
static int *Palette;

static int VBLK = 0x80;
static int soundlatch;
static unsigned char mystston_scroll_x = 0;
static int mystston_fgcolor, mystston_flipscreen;


//----------------------------------------------------------------------------------------------
// Input Handlers


static struct BurnInputInfo DrvInputList[] = {
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy1 + 6,	"p1 coin"  },
	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy2 + 6,	"p1 start" },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy1 + 0, 	"p1 right" },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy1 + 1, 	"p1 left"  },
	{"P1 Up",	  BIT_DIGITAL,   DrvJoy1 + 2,   "p1 up",   },
	{"P1 Down",	  BIT_DIGITAL,   DrvJoy1 + 3,   "p1 down", },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2"  , BIT_DIGITAL  , DrvJoy1 + 5,	"p1 fire 2"},

	{"P2 Coin"      , BIT_DIGITAL  , DrvJoy1 + 7,	"p2 coin"  },
	{"P2 start"  ,    BIT_DIGITAL  , DrvJoy2 + 7,	"p2 start" },
	{"P2 Right"     , BIT_DIGITAL  , DrvJoy2 + 0, 	"p2 right" },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy2 + 1, 	"p2 left"  },
	{"P2 Up",	  BIT_DIGITAL,   DrvJoy2 + 2,   "p2 up",   },
	{"P2 Down",	  BIT_DIGITAL,   DrvJoy2 + 3,   "p2 down", },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2"  , BIT_DIGITAL  , DrvJoy2 + 5,	"p2 fire 2"},

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip 1"	   },
	{"Dip 2",	  BIT_DIPSWITCH, DrvDips + 1,	"dip 2"	   },
};

STDINPUTINFO(Drv);

static struct BurnDIPInfo DrvDIPList[]=
{
	// Default Values
	{0x11, 0xff, 0xff, 0xfb, NULL                     },

	{0   , 0xfe, 0   , 2   , "Lives"	          },
	{0x11, 0x01, 0x01, 0x01, "3"     		  },
	{0x11, 0x01, 0x01, 0x00, "5"			  },

	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x11, 0x01, 0x02, 0x02, "Easy"       		  },
	{0x11, 0x01, 0x02, 0x00, "Hard"       		  },

	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x11, 0x01, 0x04, 0x04, "Off"     		  },
	{0x11, 0x01, 0x04, 0x00, "On"    		  },

	// Default Values
	{0x12, 0xff, 0xff, 0x1f, NULL                     },

	{0   , 0xfe, 0   , 4   , "Coin A"                 },
	{0x12, 0x01, 0x03, 0x00, "2C 1C"     		  },
	{0x12, 0x01, 0x03, 0x03, "1C 1C"    		  },
	{0x12, 0x01, 0x03, 0x02, "1C 2C"     		  },
	{0x12, 0x01, 0x03, 0x01, "1C 3C"    		  },

	{0   , 0xfe, 0   , 4   , "Coin B"                 },
	{0x12, 0x01, 0x0c, 0x00, "2C 1C"     		  },
	{0x12, 0x01, 0x0c, 0x0c, "1C 1C"    		  },
	{0x12, 0x01, 0x0c, 0x08, "1C 2C"     		  },
	{0x12, 0x01, 0x0c, 0x04, "1C 3C"    		  },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x12, 0x01, 0x20, 0x00, "Off"       		  },
	{0x12, 0x01, 0x20, 0x20, "On"       		  },

	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x12, 0x01, 0x40, 0x00, "Upright"     		  },
	{0x12, 0x01, 0x40, 0x40, "Cocktail"    		  },
};

STDDIPINFO(Drv);


//----------------------------------------------------------------------------------------------
// Memory Read/Write Handlers


static void mystston_soundcontrol_w(unsigned short, unsigned char data)
{
	static int last;

	if ((last & 0x20) == 0x20 && (data & 0x20) == 0x00)
	{
		if (last & 0x10)
			AY8910Write(0, 0, soundlatch);
		else
			AY8910Write(0, 1, soundlatch);
	}

	if ((last & 0x80) == 0x80 && (data & 0x80) == 0x00)
	{
		if (last & 0x40)
			AY8910Write(1, 0, soundlatch);
		else
			AY8910Write(1, 1, soundlatch);
	}

	last = data;
}

unsigned char mystston_read_byte(unsigned short address)
{
	unsigned char ret = 0;

	switch (address)
	{
		case 0x2000:
		{
			for (int i = 0; i < 8; i++) 
				ret |= DrvJoy1[i] << i;

			return ~ret;
		}

		case 0x2010:
		{
			for (int i = 0; i < 8; i++) 
				ret |= DrvJoy2[i] << i;

			return ~ret;
		}

		case 0x2020:
			return DrvDips[0];

		case 0x2030:
			return DrvDips[1] | VBLK;
	}

	return 0;
}

void mystston_write_byte(unsigned short address, unsigned char data)
{
#define pal2bit(bits) (((bits & 3) << 6) | ((bits & 3) << 4) | ((bits & 3) << 2) | (bits & 3))
#define pal3bit(bits) (((bits & 7) << 5) | ((bits & 7) << 2) | ((bits & 7) >> 1))

	switch (address)
	{
		case 0x2000:
		{
			mystston_fgcolor = ((data & 0x01) << 1) + ((data & 0x02) >> 1);
			mystston_flipscreen = (data & 0x80) ^ ((DrvDips[1] & 0x20) ? 0x80 : 0);
		}
		break;

		case 0x2010:
			m6502SetIRQ(M6502_CLEAR);
		break;

		case 0x2020:
			mystston_scroll_x = data;
		break;

		case 0x2030:
			soundlatch = data;
		break;

		case 0x2040:
			mystston_soundcontrol_w(0, data);
		break;
	}

	if (address >= 0x2060 && address <= 0x2077)
	{
		Palette[address & 0x1f] = (pal3bit(data) << 16) | (pal3bit(data >> 3) << 8) | pal2bit(data >> 6);
	}

#undef pal2bit
#undef pal3bit
}


//----------------------------------------------------------------------------------------------
// Initilization Routines


static int DrvDoReset()
{
	DrvReset = 0;

	memset (Rom, 0, 0x2000);

	VBLK = 0x80;
	soundlatch = 0;
	mystston_flipscreen = 0;
	mystston_scroll_x = 0;
	mystston_fgcolor = 0;

	AY8910Reset(0);
	AY8910Reset(1);

	m6502Open(0);
	m6502Reset();
	m6502Close();

	return 0;
}

static void mystston_palette_init()
{
	for (int i = 0; i < 32; i++)
	{
		int bit0, bit1, bit2, r, g, b;

		bit0 = (Prom[i] >> 0) & 0x01;
		bit1 = (Prom[i] >> 1) & 0x01;
		bit2 = (Prom[i] >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		bit0 = (Prom[i] >> 3) & 0x01;
		bit1 = (Prom[i] >> 4) & 0x01;
		bit2 = (Prom[i] >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		bit1 = (Prom[i] >> 6) & 0x01;
		bit2 = (Prom[i] >> 7) & 0x01;
		b = 0x21 * 0 + 0x47 * bit1 + 0x97 * bit2;

		Palette[i + 24] = (r << 16) | (g << 8) | b;
	}
}

static int mystston_gfx_convert()
{
	static int PlaneOffsets[3]   = { 0x40000, 0x20000, 0 };
	static int XOffsets[16]      = { 128, 129, 130, 131, 132, 133, 134, 135, 0, 1, 2, 3, 4, 5, 6, 7 };
	static int YOffsets[16]      = { 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120 };

	unsigned char *tmp = (unsigned char*)malloc(0x10000);
	if (tmp == NULL) {
		return 1;
	}

	memcpy (tmp, Gfx0, 0x10000);

	GfxDecode(0x800, 3,  8,  8, PlaneOffsets, XOffsets + 8, YOffsets, 0x040, tmp, Gfx0);
	GfxDecode(0x200, 3, 16, 16, PlaneOffsets, XOffsets + 0, YOffsets, 0x100, tmp, Gfx2);

	memcpy (tmp, Gfx1, 0x10000);

	GfxDecode(0x200, 3, 16, 16, PlaneOffsets, XOffsets + 0, YOffsets, 0x100, tmp, Gfx1);

	free (tmp);

	return 0;
}

static int DrvInit()
{
	Mem = (unsigned char*)malloc(0x10000 + 0x20000 + 0x20000 + 0x20000 + 0x20 + 0x38 * sizeof(int));
	if (Mem == NULL) {
		return 1;
	}
	memset (Mem, 0, 0x70000 + 0x20);

	pFMBuffer = (short *)malloc (nBurnSoundLen * 6 * sizeof(short));
	if (pFMBuffer == NULL) {
		return 1;
	}

	Rom  = Mem + 0x00000;
	Gfx0 = Mem + 0x10000;
	Gfx1 = Mem + 0x30000;
	Gfx2 = Mem + 0x50000;
	Prom = Mem + 0x70000;
	Palette = (int*)(Mem + 0x70020);

	// Load Roms
	{
		for (int i = 0; i < 6; i++) {
			if (BurnLoadRom(Rom  + i * 0x2000 + 0x4000, i +  0, 1)) return 1;
			if (BurnLoadRom(Gfx0 + i * 0x2000 + 0x0000, i +  6, 1)) return 1;
			if (BurnLoadRom(Gfx1 + i * 0x2000 + 0x0000, i + 12, 1)) return 1;
		}

		if (BurnLoadRom(Prom, 18, 1)) return 1;

		if(mystston_gfx_convert()) return 1;
		mystston_palette_init();
	}

	m6502Init(1);
	m6502Open(0);
	m6502MapMemory(Rom + 0x0000, 0x0000, 0x1fff, M6502_RAM);
	m6502MapMemory(Rom + 0x4000, 0x4000, 0xffff, M6502_ROM);
	m6502SetReadHandler(mystston_read_byte);
	m6502SetWriteHandler(mystston_write_byte);
	m6502Close();

	pAY8910Buffer[0] = pFMBuffer + nBurnSoundLen * 0;
	pAY8910Buffer[1] = pFMBuffer + nBurnSoundLen * 1;
	pAY8910Buffer[2] = pFMBuffer + nBurnSoundLen * 2;
	pAY8910Buffer[3] = pFMBuffer + nBurnSoundLen * 3;
	pAY8910Buffer[4] = pFMBuffer + nBurnSoundLen * 4;
	pAY8910Buffer[5] = pFMBuffer + nBurnSoundLen * 5;

	AY8910Init(0, 1500000, nBurnSoundRate, NULL, NULL, NULL, NULL);
	AY8910Init(1, 1500000, nBurnSoundRate, NULL, NULL, NULL, NULL);

	DrvDoReset();

//	BurnSetRefreshRate(57.445);

	return 0;
}

static int DrvExit()
{
	m6502Exit();
	AY8910Exit(0);
	AY8910Exit(1);

	free (Mem);
	free (pFMBuffer);

	pFMBuffer = NULL;
	Palette = NULL;
	Rom = Gfx0 = Gfx1 = Gfx2 = Prom = NULL;

	VBLK = 0x00;
	soundlatch = 0;
	mystston_scroll_x = 0;
	mystston_fgcolor = mystston_flipscreen = 0;

	return 0;
}


//----------------------------------------------------------------------------------------------
// Drawing Routines


static inline void mystston_putpix(int x, int y, unsigned char src, int color, int transp)
{
	int pos, pxl;

	if (y > 255 || x > 239 || x < 0 || (!src && transp)) return;

	if (mystston_flipscreen)
		pos = ((255 - y) * 240) + (239 - x);
	else
		pos = (y * 240) + x;

	pxl = Palette[color | src];

	PutPix(pBurnDraw + pos * nBurnBpp, BurnHighCol(pxl >> 16, pxl >> 8, pxl, 0));
}

static void draw_16x16(int sx, int sy, unsigned char *gfx_base, int code, int color, int flipx, int flipy, int transp)
{
	unsigned char *src = gfx_base + code;

	if (flipx)
	{
		for (int x = sx + 15; x >= sx; x--)
		{
			if (flipy)
			{
				for (int y = sy; y < sy + 16; y++, src++) {
					mystston_putpix(x, y, *src, color, transp);
				}
			} else {
				for (int y = sy + 15; y >= sy; y--, src++) {
					mystston_putpix(x, y, *src, color, transp);
				}
			}
		}
	} else {
		for (int x = sx; x < sx + 16; x++)
		{
			if (flipy)
			{
				for (int y = sy; y < sy + 16; y++, src++) {
					mystston_putpix(x, y, *src, color, transp);
				}
			} else {
				for (int y = sy + 15; y >= sy; y--, src++) {
					mystston_putpix(x, y, *src, color, transp);
				}
			}
		}
	}
}

static int DrvDraw()
{
	for (int offs = 0; offs < 0x200; offs++)
	{
		int code = (Rom[0x1800 + offs] + ((Rom[0x1a00 + offs] & 0x01) << 8)) << 8;
		int flipx = offs & 0x10;
		int sx = ((offs & 0x1f) << 4) - (mystston_scroll_x + 8);
		int sy = ((offs >> 5) << 4);

		draw_16x16(sx, sy, Gfx1, code, 0x10, flipx, 0, 0);
	}

	for (int offs = 0; offs < 0x60; offs += 4)
	{
		int attr = Rom[0x0780 + offs];

		if (attr & 0x01)
		{
			int code = (Rom[0x0781 + offs] + ((attr & 0x10) << 4)) << 8;
			int color = attr & 0x08;
			int flipy = attr & 0x04;
			int flipx = attr & 0x02;
			int sy = Rom[0x0783 + offs];
			int sx = (240 - Rom[0x0782 + offs]) - 9;

			draw_16x16(sx, sy, Gfx2, code, color, flipx, flipy, 1);
		}
	}

	for (int offs = 0; offs < 0x400; offs++)
	{
		int code = (Rom[0x1000 | offs] + ((Rom[0x1400 + offs] & 0x07) << 8)) << 6;
		int color = (mystston_fgcolor << 3) + 0x18;
		int sx = (offs & 0x1f) << 3;
		int sy = (offs >> 2) & 0xf8;
		if (sx >= 248 || sx < 8) continue;
		sx -= 8;

		unsigned char *src = Gfx0 + code;

		for (int x = sx; x < sx + 8; x++)
		{
			for (int y = sy + 7; y >= sy; y--, src++)
			{
				if (!*src) continue;

				int pos;
				if (mystston_flipscreen)
					pos = ((255 - y) * 240) + (239 - x);
				else
					pos = (y * 240) + x;

				int pxl = Palette[color | *src];

				PutPix(pBurnDraw + pos * nBurnBpp, BurnHighCol(pxl >> 16, pxl >> 8, pxl, 0));
			}
		}
	}

	return 0;
}

static void mystston_interrupt_handler(int scanline)
{
	static int coin;
	int inp = (DrvJoy1[6] << 6) | (DrvJoy1[7] << 7);

	if ((~inp & 0xc0) != 0xc0)
	{
		if (coin == 0)
		{
			coin = 1;
			m6502SetIRQ(M6502_NMI);
			return;
		}
	}
	else coin = 0;

	if (scanline == 8)
		VBLK = 0;

	if (scanline == 248)
		VBLK = 0x80;

	if ((scanline & 0x0f) == 0)
		m6502SetIRQ(M6502_IRQ);
}

static int DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}

	int nTotalCycles = (int)((double)(1500000 / 57.45));
	int nCyclesRun = 0;

	m6502Open(0);

	for (int i = 0; i < 272; i++) {
		nCyclesRun += m6502Run(nTotalCycles / 272);
		mystston_interrupt_handler(i);
	}

	m6502Close();

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


//----------------------------------------------------------------------------------------------
// Save State


static int DrvScan(int nAction,int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {
		*pnMin = 0x029521;
	}

	if (nAction & ACB_VOLATILE) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = Rom + 0x0000;
		ba.nLen	  = 0x2000;
		ba.szName = "All Ram";
		BurnAcb(&ba);

		memset(&ba, 0, sizeof(ba));
		ba.Data	  = Mem + 0x70020;
		ba.nLen	  = 24 * sizeof(int);
		ba.szName = "Palette";
		BurnAcb(&ba);

		m6502Scan(nAction);
		AY8910Scan(nAction, pnMin);

		// Scan critical driver variables
		SCAN_VAR(mystston_flipscreen);
		SCAN_VAR(mystston_scroll_x);
		SCAN_VAR(mystston_fgcolor);
		SCAN_VAR(soundlatch);
		SCAN_VAR(VBLK);
	}

	return 0;
}


//----------------------------------------------------------------------------------------------
// Game Drivers


// Mysterious Stones - Dr. John's Adventure

static struct BurnRomInfo myststonRomDesc[] = {
	{ "rom6.bin",     0x2000, 0x7bd9c6cd, 1 | BRF_PRG | BRF_ESS }, //  0 M6205 Code
	{ "rom5.bin",     0x2000, 0xa83f04a6, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "rom4.bin",     0x2000, 0x46c73714, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "rom3.bin",     0x2000, 0x34f8b8a3, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "rom2.bin",     0x2000, 0xbfd22cfc, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "rom1.bin",     0x2000, 0xfb163e38, 1 | BRF_PRG | BRF_ESS }, //  5

	{ "ms6",          0x2000, 0x85c83806, 2 | BRF_GRA }, 	       //  6 Character + Sprite Tiles
	{ "ms9",          0x2000, 0xb146c6ab, 2 | BRF_GRA },  	       //  7
	{ "ms7",          0x2000, 0xd025f84d, 2 | BRF_GRA },  	       //  8
	{ "ms10",         0x2000, 0xd85015b5, 2 | BRF_GRA },  	       //  9
	{ "ms8",          0x2000, 0x53765d89, 2 | BRF_GRA },  	       // 10
	{ "ms11",         0x2000, 0x919ee527, 2 | BRF_GRA },  	       // 11

	{ "ms12",         0x2000, 0x72d8331d, 3 | BRF_GRA },  	       // 12 Sprite Tiles
	{ "ms13",         0x2000, 0x845a1f9b, 3 | BRF_GRA },  	       // 13
	{ "ms14",         0x2000, 0x822874b0, 3 | BRF_GRA },  	       // 14 
	{ "ms15",         0x2000, 0x4594e53c, 3 | BRF_GRA },  	       // 15 
	{ "ms16",         0x2000, 0x2f470b0f, 3 | BRF_GRA },  	       // 16
	{ "ms17",         0x2000, 0x38966d1b, 3 | BRF_GRA },  	       // 17

	{ "ic61",         0x0020, 0xe802d6cf, 4 | BRF_GRA },  	       // 18 Color Prom
};

STD_ROM_PICK(mystston);
STD_ROM_FN(mystston);

struct BurnDriver BurnDrvmystston = {
	"mystston", NULL, NULL, "1984",
	"Mysterious Stones - Dr. John's Adventure\0", NULL, "Technos", "misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, myststonRomInfo, myststonRomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	240, 256, 3, 4
};


// Mysterious Stones - Dr. Kick in Adventure

static struct BurnRomInfo myststnoRomDesc[] = {
	{ "ms0",          0x2000, 0x6dacc05f, 1 | BRF_PRG | BRF_ESS }, //  0 M6205 Code
	{ "ms1",          0x2000, 0xa3546df7, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "ms2",          0x2000, 0x43bc6182, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "ms3",          0x2000, 0x9322222b, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "ms4",          0x2000, 0x47cefe9b, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "ms5",          0x2000, 0xb37ae12b, 1 | BRF_PRG | BRF_ESS }, //  5

	{ "ms6",          0x2000, 0x85c83806, 2 | BRF_GRA }, 	       //  6 Character + Sprite Tiles
	{ "ms9",          0x2000, 0xb146c6ab, 2 | BRF_GRA },  	       //  7
	{ "ms7",          0x2000, 0xd025f84d, 2 | BRF_GRA },  	       //  8
	{ "ms10",         0x2000, 0xd85015b5, 2 | BRF_GRA },  	       //  9
	{ "ms8",          0x2000, 0x53765d89, 2 | BRF_GRA },  	       // 10
	{ "ms11",         0x2000, 0x919ee527, 2 | BRF_GRA },  	       // 11

	{ "ms12",         0x2000, 0x72d8331d, 3 | BRF_GRA },  	       // 12 Sprite Tiles
	{ "ms13",         0x2000, 0x845a1f9b, 3 | BRF_GRA },  	       // 13
	{ "ms14",         0x2000, 0x822874b0, 3 | BRF_GRA },  	       // 14 
	{ "ms15",         0x2000, 0x4594e53c, 3 | BRF_GRA },  	       // 15 
	{ "ms16",         0x2000, 0x2f470b0f, 3 | BRF_GRA },  	       // 16
	{ "ms17",         0x2000, 0x38966d1b, 3 | BRF_GRA },  	       // 17

	{ "ic61",         0x0020, 0xe802d6cf, 4 | BRF_GRA },  	       // 18 Color Prom
};

STD_ROM_PICK(myststno);
STD_ROM_FN(myststno);

struct BurnDriver BurnDrvmyststno = {
	"myststno", "mystston", NULL, "1984",
	"Mysterious Stones - Dr. Kick in Adventure\0", NULL, "Technos", "misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S,
	NULL, myststnoRomInfo, myststnoRomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	240, 256, 3, 4
};

