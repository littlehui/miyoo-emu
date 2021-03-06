// FB Alpha Legend of Silkroad driver
// Based on MAME driver by Haze, R.Belmont, and Stephh

#include "tiles_generic.h"
#include "burn_ym2151.h"
#include "msm6295.h"

static unsigned char *Mem, *MemEnd, *AllRam, *RamEnd;
static unsigned char *DrvRom, *DrvSprRam, *DrvPalRam, *DrvVidRam, *DrvRam;
static unsigned char *DrvGfx, *DrvTransTab;
static unsigned char *DrvSnd0, *DrvSnd1;
static unsigned short *DrvSysRegs;

static unsigned int *Palette, *DrvPalette;
static unsigned char DrvRecalc;

static unsigned char DrvJoy1[16], DrvJoy2[16], DrvDips[2], DrvReset;
static unsigned short DrvInputs[2];

static struct BurnInputInfo DrvInputList[] = {
	{"Coin 1"       , BIT_DIGITAL  , DrvJoy2 + 0,	 "p1 coin"  },
	{"Coin 2"       , BIT_DIGITAL  , DrvJoy2 + 1,	 "p2 coin"  },

	{"P1 Start"     , BIT_DIGITAL  , DrvJoy2 + 2,	 "p1 start" },
	{"P1 Up"        ,	BIT_DIGITAL  , DrvJoy1 + 15, "p1 up"    },
	{"P1 Down"      ,	BIT_DIGITAL  , DrvJoy1 + 14, "p1 down", },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy1 + 12, "p1 left"  },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy1 + 13, "p1 right" },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 11, "p1 fire 1"},
	{"P1 Button 2"  , BIT_DIGITAL  , DrvJoy1 + 10, "p1 fire 2"},
	{"P1 Button 3"  , BIT_DIGITAL  , DrvJoy1 + 9,	 "p1 fire 3"},

	{"P2 Start"     , BIT_DIGITAL  , DrvJoy2 + 3,	 "p2 start" },
	{"P2 Up"        ,	BIT_DIGITAL  , DrvJoy1 + 7,  "p2 up"    },
	{"P2 Down"      ,	BIT_DIGITAL  , DrvJoy1 + 6,  "p2 down"  },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy1 + 4,  "p2 left"  },
	{"P2 Right"     , BIT_DIGITAL  , DrvJoy1 + 5,  "p2 right" },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 3,	 "p2 fire 1"},
	{"P2 Button 2"  , BIT_DIGITAL  , DrvJoy1 + 2,	 "p2 fire 2"},
	{"P2 Button 3"  , BIT_DIGITAL  , DrvJoy1 + 1,	 "p2 fire 3"},

	{"Test"         ,	BIT_DIGITAL  , DrvJoy2 + 5,  "diag"     },
	{"Service"      ,	BIT_DIGITAL  , DrvJoy2 + 4,  "service"  },

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip"	   },
	{"Dip 2",	  BIT_DIPSWITCH, DrvDips + 1,	"dip"	   },
};

STDINPUTINFO(Drv);

static struct BurnDIPInfo DrvDIPList[]=
{
	{0x15, 0xff, 0xff, 0x1c, NULL},
	{0x16, 0xff, 0xff, 0xf7, NULL},

	{0,    0xfe, 0,    2,    "Lives"		},
	{0x15, 0x01, 0x01, 0x01, "1"			},
	{0x15, 0x01, 0x01, 0x00, "2"			},

	{0,    0xfe, 0,    2,    "Blood Effect"		},
	{0x15, 0x01, 0x02, 0x02, "Off"			},
	{0x15, 0x01, 0x02, 0x00, "On"			},
 
	// the number at the bottom of the "Warning" screen
	// is actually the difficulty level.  0 is easiest
	// and 7 is hardest.
	{0,    0xfe, 0,    8,    "Difficulty"		},
	{0x15, 0x01, 0xe0, 0x60, "Easiest"		},
	{0x15, 0x01, 0xe0, 0x40, "Easier"		},
	{0x15, 0x01, 0xe0, 0x20, "Easy"			},
	{0x15, 0x01, 0xe0, 0x00, "Normal"		},
	{0x15, 0x01, 0xe0, 0xe0, "Medium"		},
	{0x15, 0x01, 0xe0, 0xc0, "Hard"			},
	{0x15, 0x01, 0xe0, 0xa0, "Harder"		},
	{0x15, 0x01, 0xe0, 0x80, "Hardest"		},

	{0,    0xfe, 0,    2,    "Free Play"		},
	{0x16, 0x01, 0x02, 0x02, "Off"			},
	{0x16, 0x01, 0x02, 0x00, "On"			},

	{0,    0xfe, 0,    2,    "Demo Sounds"		},
	{0x16, 0x01, 0x08, 0x08, "Off"			},
	{0x16, 0x01, 0x08, 0x00, "On"			},

	{0,    0xfe, 0,    2,    "Chute Type"		},
	{0x16, 0x01, 0x10, 0x10, "Single"		},
	{0x16, 0x01, 0x10, 0x00, "Multi"		},

	{0,    0xfe, 0,    8,    "Coinage"		},
	{0x16, 0x01, 0xe0, 0x00, "5 Coins 1 Credit"	},
	{0x16, 0x01, 0xe0, 0x20, "4 Coins 1 Credit"	},
	{0x16, 0x01, 0xe0, 0x40, "3 Coins 1 Credit"	},
	{0x16, 0x01, 0xe0, 0x60, "2 Coins 1 Credit"	},
	{0x16, 0x01, 0xe0, 0xe0, "1 Coin  1 Credit"	},
	{0x16, 0x01, 0xe0, 0xc0, "1 Coin  2 Credits"	},
	{0x16, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"	},
	{0x16, 0x01, 0xe0, 0x80, "1 Coin  4 Credits"	},
};

STDDIPINFO(Drv);

static void palette_write(int offset, unsigned short pal)
{
	unsigned char r = (pal >> 10) & 0x1f;
	unsigned char g = (pal >>  5) & 0x1f;
	unsigned char b = (pal >>  0) & 0x1f;

	r = (r << 3) | (r >> 2);
	g = (g << 3) | (g >> 2);
	b = (b << 3) | (b >> 2);

	Palette[offset] = (r << 16) | (g << 8) | b;
	DrvPalette[offset] = BurnHighCol(r, g, b, 0);
}

unsigned char __fastcall silkroad_read_byte(unsigned int address)
{
	switch (address)
	{
		case 0xc00000:
			return DrvInputs[0] >> 8;

		case 0xc00001:
			return DrvInputs[0] & 0xff;

		case 0xc00002:
			return 0xff;

		case 0xc00003:
			return DrvInputs[1] & 0xff;

		case 0xc00004:
			return DrvDips[1];

		case 0xc00005:
			return DrvDips[0];

		case 0xc00006:
		case 0xc00007:
			return 0;

		case 0xc00025:
			return MSM6295ReadStatus(0);

		case 0xc0002d:
			return BurnYM2151ReadStatus();

		case 0xc00031:
			return MSM6295ReadStatus(1);
	}

	return 0;
}

unsigned short __fastcall silkroad_read_word(unsigned int)
{
	return 0;
}

unsigned int __fastcall silkroad_read_long(unsigned int)
{
	return 0;
}

void __fastcall silkroad_write_byte(unsigned int address, unsigned char data)
{
	switch (address)
	{
		case 0xc00025:
			MSM6295Command(0, data);
		return;

		case 0xc00029:
			BurnYM2151SelectRegister(data);
		return;


		case 0xc0002d:
			BurnYM2151WriteRegister(data);
		return;

		case 0xc00031:
			MSM6295Command(1, data);
		return;

		case 0xc00034:
		case 0xc00039:
		return;
	}
}

void __fastcall silkroad_write_word(unsigned int address, unsigned short data)
{
	if ((address & 0xffffc000) == 0x600000) {
		*((unsigned short *)(DrvPalRam + (address & 0x3ffe))) = data;

		palette_write((address >> 2) & 0x0fff, data);

		return;
	}

	if (address >= 0xc0010c || address <= 0xc00123) {
		DrvSysRegs[(address - 0xc0010c) >> 1] = data;

		return;
	}
}

void __fastcall silkroad_write_long(unsigned int address, unsigned int data)
{
	if ((address & 0xffffc000) == 0x600000) {
		*((unsigned int *)(DrvPalRam + (address & 0x3ffc))) = data;

		palette_write((address >> 2) & 0x0fff, data >> 16);

		return;
	}
}

static int DrvGfxDecode()
{
	static int Planes[6] = { 0x0000008, 0x0000000, 0x1000008, 0x1000000, 0x2000008, 0x2000000 };
	static int XOffs[16] = { 0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007,
				 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x017 };
	static int YOffs[16] = { 0x000, 0x020, 0x040, 0x060, 0x080, 0x0a0, 0x0c0, 0x0e0,
				 0x100, 0x120, 0x140, 0x160, 0x180, 0x1a0, 0x1c0, 0x1e0 };

	unsigned char *tmp = (unsigned char*)malloc(0x600000);
	if (tmp == NULL) {
		return 1;
	}

	for (int i = 0; i < 4; i++ ){
		if (BurnLoadRom(tmp + 0x000000, 2 + i * 3, 1)) return 1;
		if (BurnLoadRom(tmp + 0x200000, 3 + i * 3, 1)) return 1;
		if (BurnLoadRom(tmp + 0x400000, 4 + i * 3, 1)) return 1;

		// fix rom04
		if (i == 0) {
			for (int j = 0x1b3fff; j >= 0; j--) {
				tmp[0x44c000 + j] = tmp[0x44c000 + (j - 1)];
			}
		}

		// Invert bits
		for (int j = 0; j < 0x600000; j++) {
			tmp[j] ^= 0xff;
		}

		GfxDecode(0x08000, 6, 16, 16, Planes, XOffs, YOffs, 0x200, tmp, DrvGfx + 0x0800000 * i);
	}

	free (tmp);

	DrvTransTab = (unsigned char*)malloc(0x20000);
	if (DrvTransTab == NULL) {
		return 1;
	}	

	// Make a table that holds info on if a tile is completely transparent, opaque
	// or only semi-transparent. This gives a massive speedup when drawing tiles.
	{
		memset (DrvTransTab, 0, 0x20000);
		for (int i = 0, c = 0; i < 0x2000000; i++) {
			if (DrvGfx[i]) {
				DrvTransTab[i>>8] |= 1;
				c++;
			}

			if ((i & 0xff) == 0xff) {
				if (c == 0x100) DrvTransTab[i>>8] |= 2;
				c = 0;
			}
		} 
	}

	return 0;
}

static int DrvDoReset()
{
	DrvReset = 0;

	memset (AllRam, 0, RamEnd - AllRam);

	SekOpen(0);
	SekReset();
	SekClose();

	BurnYM2151Reset();

	MSM6295Reset(0);
	MSM6295Reset(1);

	for (int nChannel = 0; nChannel < 4; nChannel++) {
		MSM6295SampleInfo[0][nChannel] = DrvSnd0 + (nChannel << 8);
		MSM6295SampleData[0][nChannel] = DrvSnd0 + (nChannel << 16);

		MSM6295SampleInfo[1][nChannel] = DrvSnd1 + (nChannel << 8);
		MSM6295SampleData[1][nChannel] = DrvSnd1 + (nChannel << 16);
	}

	DrvRecalc = 1;

	return 0;
}

static int MemIndex()
{
	unsigned char *Next; Next = Mem;

	DrvRom		= Next; Next += 0x0200000;

	MSM6295ROM	= Next;
	DrvSnd0		= Next; Next += 0x0080000;
	DrvSnd1		= Next; Next += 0x0040000;

	DrvPalette	= (unsigned int*)Next; Next += 0x01001 * sizeof(int);
	
	AllRam		= Next;

	DrvSprRam	= Next; Next += 0x0001000;
	DrvPalRam	= Next; Next += 0x0004000;
	DrvVidRam	= Next; Next += 0x000c000;
	DrvRam		= Next; Next += 0x0020000;

	DrvSysRegs	= (unsigned short*)Next; Next += 0x0000020 * sizeof (short);

	Palette		= (unsigned int*)Next; Next += 0x01000 * sizeof(int);

	RamEnd		= Next;

	MemEnd		= Next;

	return 0;
}

static int DrvInit()
{
	int nLen;

	DrvGfx = (unsigned char*)malloc(0x2000000);
	if (DrvGfx == NULL) {
		return 1;
	}

	if (DrvGfxDecode()) return 1;

	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	MemIndex();

	{
		if (BurnLoadRom(DrvRom + 0, 0, 2)) return 1;
		if (BurnLoadRom(DrvRom + 1, 1, 2)) return 1;

		for (int i = 0; i < 0x200000; i+= 4) {
			int t = DrvRom[i + 1];
			DrvRom[i + 1] = DrvRom[i + 2];
			DrvRom[i + 2] = t;
		}

		if (BurnLoadRom(DrvSnd0, 14, 1)) return 1;
		if (BurnLoadRom(DrvSnd1, 15, 1)) return 1;
	}

	SekInit(0, 0x68EC020);
	SekOpen(0);
	SekMapMemory (DrvRom,		0x000000, 0x1fffff, SM_ROM);
	SekMapMemory (DrvSprRam,	0x40c000, 0x40cfff, SM_RAM);
	SekMapMemory (DrvPalRam,	0x600000, 0x603fff, SM_ROM);
	SekMapMemory (DrvVidRam,	0x800000, 0x80bfff, SM_RAM);
	SekMapMemory (DrvRam,		0xfe0000, 0xffffff, SM_RAM);
	SekSetWriteByteHandler(0,	silkroad_write_byte);
	SekSetWriteWordHandler(0,	silkroad_write_word);
	SekSetWriteLongHandler(0,	silkroad_write_long);
	SekSetReadByteHandler(0,	silkroad_read_byte);
	SekSetReadWordHandler(0,	silkroad_read_word);
	SekSetReadLongHandler(0,	silkroad_read_long);
	SekClose();

	BurnYM2151Init(3579545, 100.0);
	MSM6295Init(0, 1056000 / 132, 45.0, 1);
	MSM6295Init(1, 2112000 / 132, 45.0, 1);

	DrvDoReset();

	GenericTilesInit();

	return 0;
}

static int DrvExit()
{
	SekExit();

	BurnYM2151Exit();
	MSM6295Exit(0);
	MSM6295Exit(1);

	GenericTilesExit();

	free (Mem);
	Mem = NULL;

	free (DrvGfx);
	DrvGfx = NULL;

	free (DrvTransTab);
	DrvTransTab = NULL;

	return 0;
}


static void silkroad_draw_tile(int code, int sx, int sy, int color, int flipx)
{
	if (!DrvTransTab[code]) return;

	if (flipx) {
		if (sx >= 0 && sx <= 352 && sy >= 0 && sy <= 208) {
			if (DrvTransTab[code] & 2) {
				Render16x16Tile_FlipX(pTransDraw, code, sx, sy, color, 6, 0, DrvGfx);
			} else {
				Render16x16Tile_Mask_FlipX(pTransDraw, code, sx, sy, color, 6, 0, 0, DrvGfx);
			}
		} else {
			if (DrvTransTab[code] & 2) {
				Render16x16Tile_FlipX_Clip(pTransDraw, code, sx, sy, color, 6, 0, DrvGfx);
			} else {
				Render16x16Tile_Mask_FlipX_Clip(pTransDraw, code, sx, sy, color, 6, 0, 0, DrvGfx);
			}
		}
	} else {
		if (sx >= 0 && sx <= 352 && sy >= 0 && sy <= 208) {
			if (DrvTransTab[code] & 2) {
				Render16x16Tile(pTransDraw, code, sx, sy, color, 6, 0, DrvGfx);
			} else {
				Render16x16Tile_Mask(pTransDraw, code, sx, sy, color, 6, 0, 0, DrvGfx);
			}
		} else {
			if (DrvTransTab[code] & 2) {
				Render16x16Tile_Clip(pTransDraw, code, sx, sy, color, 6, 0, DrvGfx);
			} else {
				Render16x16Tile_Mask_Clip(pTransDraw, code, sx, sy, color, 6, 0, 0, DrvGfx);
			}
		}
	}
}

static void draw_sprites(int pri)
{
	unsigned int *source = (unsigned int*)DrvSprRam;
	unsigned int *finish = source + 0x1000/4;
	unsigned int *maxspr = source;

	while (maxspr < finish)
	{
		int attr = maxspr[1] >> 16;

		if ((attr & 0xff00) == 0xff00) break;
		maxspr += 2;
	}

	finish = maxspr - 2;

	while (finish >= source)
	{
		int xpos  = finish[0] & 0x01ff;
		int ypos  = finish[0] >> 16;
		int tileno = finish[1] & 0xffff;
		int attr  = finish[1] >> 16;

		xpos -= 50;
		ypos -= 16;

		finish -= 2;

		if (ypos == -16 || ypos > 223) continue;

		int flipx = (attr & 0x0080);

		int width = ((attr >> 8) & 0x0f) + 1;
		int color = (attr & 0x003f);

		int priority = attr & 0x1000;

		if (attr & 0x8000) tileno += 0x10000;

		if (priority == pri) {
			if (!flipx) {
				for (int wcount = 0; wcount < width; wcount++) {
					int sx = xpos + ((wcount << 4) | 8);

					if (sx < -15 || sx > 379) continue;

					silkroad_draw_tile(tileno + wcount, sx, ypos, color, 0);
				}
			} else {
				for (int wcount = width; wcount > 0; wcount--) {
					int sx = xpos + (((wcount << 4) - 0x10) | 8);

					if (sx < -15 || sx > 379) continue;

					silkroad_draw_tile(tileno+(width-wcount), sx, ypos, color, 1);
				}
			}
		}
	}
}

static void draw_fg(int offset, int scrollx, int scrolly)
{
	unsigned int *vidram = (unsigned int*)(DrvVidRam + offset);

	for (int offs = 0; offs < 0x4000 / 4; offs++)
	{
		int sx = (offs << 4) & 0x3f0;
		int sy = (offs >> 2) & 0x3f0;

		sx -= scrollx & 0x3ff;
		sy -= scrolly & 0x3ff;

		sx -= 50;
		sy -= 16;

		if (sy < -15) sy += 0x400;
		if (sx < -15) sx += 0x400;

		if (sx >= 380) continue;

		int code  = vidram[offs] & 0xffff;
		int color = (vidram[offs] >> 16) & 0x1f;
		int flipx = (vidram[offs] >> 16) & 0x80;

		code += 0x18000;

		silkroad_draw_tile(code, sx, sy, color, flipx);
	}
}

static int DrvDraw()
{
	if (DrvRecalc) {
		for (int i = 0; i < 0x1000; i++) {
			int rgb = Palette[i];
			DrvPalette[i] = BurnHighCol(rgb >> 16, rgb >> 8, rgb, 0);
		}

		// Set magenta for layer disabling
		DrvPalette[0x1000] = BurnHighCol(0xff, 0x00, 0xff, 0);
	}

	// Set background color
	if (nBurnLayer & 1) {
		for (int i = 0; i < nScreenWidth * nScreenHeight; i++) {
			pTransDraw[i] = 0x7c0;
		}
	} else {
		for (int i = 0; i < nScreenWidth * nScreenHeight; i++) {
			pTransDraw[i] = 0x1000;
		}
	}

	if (nBurnLayer    & 2) draw_fg(0x0000, DrvSysRegs[ 0], DrvSysRegs[ 1]);
	if (nSpriteEnable & 1) draw_sprites(0x0000);
	if (nBurnLayer    & 4) draw_fg(0x4000, DrvSysRegs[ 5], DrvSysRegs[10]);
	if (nSpriteEnable & 2) draw_sprites(0x1000);
	if (nBurnLayer    & 8) draw_fg(0x8000, DrvSysRegs[ 4], DrvSysRegs[ 2]);

	BurnTransferCopy(DrvPalette);

	return 0;
}


static int DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}

	{
		DrvInputs[0] = DrvInputs[1] = 0xffff;
		for (int i = 0; i < 16; i++) {
			DrvInputs[0] ^= DrvJoy1[i] << i;
			DrvInputs[1] ^= DrvJoy2[i] << i;
		}

		DrvInputs[1] ^= DrvJoy2[4] << 6;

		// Clear opposites
		if (!(DrvInputs[0] & 0x00c0)) DrvInputs[0] |= 0x00c0;
		if (!(DrvInputs[0] & 0x0030)) DrvInputs[0] |= 0x0030;
		if (!(DrvInputs[0] & 0xc000)) DrvInputs[0] |= 0xc000;
		if (!(DrvInputs[0] & 0x3000)) DrvInputs[0] |= 0x3000;
	}

	// Allow CPU overclock
	int nTotalCycles = (int)((long long)16000000 * nBurnCPUSpeedAdjust / (0x0100 * 60));

	SekOpen(0);
	SekRun(nTotalCycles);
	SekSetIRQLine(4, SEK_IRQSTATUS_AUTO);
	SekClose();

	if (pBurnSoundOut) {
		BurnYM2151Render(pBurnSoundOut, nBurnSoundLen);
		MSM6295Render(0, pBurnSoundOut, nBurnSoundLen);
		MSM6295Render(1, pBurnSoundOut, nBurnSoundLen);
	}

	if (pBurnDraw) {
		DrvDraw();
	}

	return 0;
}

static int DrvScan(int nAction, int *pnMin)
{
	struct BurnArea ba;
	
	if (pnMin != NULL) {
		*pnMin = 0x029682;
	}

	if (nAction & ACB_MEMORY_RAM) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = AllRam;
		ba.nLen	  = RamEnd-AllRam;
		ba.szName = "All Ram";
		BurnAcb(&ba);
	}

	if (nAction & ACB_DRIVER_DATA) {
		SekScan(nAction);

		BurnYM2151Scan(nAction);
		MSM6295Scan(0, nAction);
		MSM6295Scan(1, nAction);

		SCAN_VAR(DrvInputs[0]);
		SCAN_VAR(DrvInputs[1]);
	}

	return 0;
}

// The Legend of Silkroad

static struct BurnRomInfo silkroadRomDesc[] = {
	{ "rom02.bin",	0x100000, 0x4e5200fc, 1 | BRF_PRG }, //  0 Motorola '020 Code
	{ "rom03.bin",	0x100000, 0x73ccc78c, 1 | BRF_PRG }, //  1

	{ "rom12.bin",	0x200000, 0x96393d04, 2 | BRF_GRA }, //  2 Tiles
	{ "rom08.bin",	0x200000, 0x23f1d462, 2 | BRF_GRA }, //  3
	{ "rom04.bin",	0x200000, 0x2cf6ed30, 2 | BRF_GRA }, //  4

	{ "rom13.bin",	0x200000, 0x4ca1698e, 2 | BRF_GRA }, //  5
	{ "rom09.bin",	0x200000, 0xef0b5bf4, 2 | BRF_GRA }, //  6
	{ "rom05.bin",	0x200000, 0x512d6e25, 2 | BRF_GRA }, //  7

	{ "rom14.bin",	0x200000, 0xd00b19c4, 2 | BRF_GRA }, //  8
	{ "rom10.bin",	0x200000, 0x7d324280, 2 | BRF_GRA }, //  9
	{ "rom06.bin",	0x200000, 0x3ac26060, 2 | BRF_GRA }, // 10

	{ "rom07.bin",	0x200000, 0x9fc6ff9d, 2 | BRF_GRA }, // 11
	{ "rom11.bin",	0x200000, 0x11abaf1c, 2 | BRF_GRA }, // 12
	{ "rom15.bin",	0x200000, 0x26a3b168, 2 | BRF_GRA }, // 13

	{ "rom00.bin",  0x080000, 0xb10ba7ab, 3 | BRF_SND }, // 14 OKI Samples
	{ "rom01.bin",  0x040000, 0xdb8cb455, 4 | BRF_SND }, // 15
};

STD_ROM_PICK(silkroad);
STD_ROM_FN(silkroad);

struct BurnDriver BurnDrvSilkroad = {
	"silkroad", NULL, NULL, "1999",
	"The Legend of Silkroad\0", NULL, "Unico", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_POST90S,
	NULL, silkroadRomInfo, silkroadRomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvRecalc,
	380, 224, 4, 3
};
