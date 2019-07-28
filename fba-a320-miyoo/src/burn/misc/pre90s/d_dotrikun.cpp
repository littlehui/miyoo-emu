
/*
Dottori Kun (Head On's mini game)
(c)1990 SEGA

CPU   : Z-80 (4MHz)
SOUND : (none)

14479.MPR  ; PRG (FIRST VER)
14479A.MPR ; PRG (NEW VER)

* This game is only for the test of cabinet
* BackRaster = WHITE on the FIRST version.
* BackRaster = BLACK on the NEW version.
* On the NEW version, push COIN-SW as TEST MODE.
* 0000-3FFF:ROM 8000-85FF:VRAM(128x96) 8600-87FF:WORK-RAM

Based on MAME driver by Takahiro Nogi
*/

#include "burnint.h"


static unsigned char DrvJoy[8], DrvReset, *Mem, nColor;

static struct BurnInputInfo DrvInputList[] = {
	{"P1 Up",	  BIT_DIGITAL,   DrvJoy + 0,    "p1 up"    },
	{"P1 Down",	  BIT_DIGITAL,   DrvJoy + 1,    "p1 down", },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy + 2, 	"p1 left"  },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy + 3, 	"p1 right" },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy + 4,	"p1 fire 1"},
	{"P1 Button 2"  , BIT_DIGITAL  , DrvJoy + 5,	"p1 fire 2"},
	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy + 6,	"p1 start" },
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy + 7,	"p1 coin"  },

	{"Reset"        , BIT_DIGITAL  , &DrvReset  ,	"reset"    },
};

STDINPUTINFO(Drv);


unsigned char __fastcall dotrikun_in_port(unsigned short a)
{
	a &= 0xff;

	if (a == 0) {
		unsigned char ret = 0;
		for (int i = 0; i < 8; i++) ret |= DrvJoy[i] << i;

		return ~ret;
	}

	return 0;
}


void __fastcall dotrikun_out_port(unsigned short a, unsigned char d)
{
	a &= 0xff;

	if (a == 0) {
		nColor = d;
	}
}


static int DrvScan(int nAction,int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {
		*pnMin = 0x029521;
	}

	if (nAction & ACB_VOLATILE) {	
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = Mem + 0x4000;
		ba.nLen	  = 0x0800;
		ba.szName = "All Ram";
		BurnAcb(&ba);

		ZetScan(nAction);

		SCAN_VAR(nColor);
	}

	return 0;
}


static int DrvDoReset()
{
	DrvReset = 0;
	memset (Mem + 0x4000, 0, 0x0800);

	ZetOpen(0);
	ZetReset();
	ZetClose();

	return 0;
}


static int DrvDraw()
{
#define sw(n)	((nColor >> n) & 1 ? 0xff : 0)
#define color()	BurnHighCol(pen >> 16, pen >> 8, pen, 0)

	unsigned int back_pen = (sw(3) << 16) | (sw(4) << 8) | sw(5);
	unsigned int fore_pen = (sw(0) << 16) | (sw(1) << 8) | sw(2);

	for (int offs = 0; offs < 0x0600; offs++)
	{
		unsigned char x = offs << 4;
		unsigned char y = offs >> 4 << 1;
		unsigned char data = Mem[0x4000 + offs];

		for (int i = 0; i < 8; i++)
		{
			unsigned int pen = (data & 0x80) ? fore_pen : back_pen;

			PutPix(pBurnDraw + (x + 0 + (y + 0) * 256) * nBurnBpp, color());
			PutPix(pBurnDraw + (x + 1 + (y + 0) * 256) * nBurnBpp, color());
			PutPix(pBurnDraw + (x + 0 + (y + 1) * 256) * nBurnBpp, color());
			PutPix(pBurnDraw + (x + 1 + (y + 1) * 256) * nBurnBpp, color());

			x = x + 2;
			data = data << 1;
		}
	}

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

	if (pBurnDraw) {
		DrvDraw();
	}

	return 0;
}


static int DrvInit()
{
	Mem = (unsigned char*)malloc ( 0x4800 );
	if (Mem == NULL) {
		return 1;
	}

	if (BurnLoadRom(Mem, 0, 1)) return 1;

	ZetInit(1);
	ZetOpen(0);
	ZetMapArea (0x0000, 0x3fff, 0, Mem + 0x0000); // Read ROM
	ZetMapArea (0x0000, 0x3fff, 2, Mem + 0x0000); // Fetch ROM
	ZetMapArea (0x8000, 0x87ff, 0, Mem + 0x4000); // Read RAM
	ZetMapArea (0x8000, 0x87ff, 1, Mem + 0x4000); // Write RAM
	ZetMemEnd();
	ZetSetInHandler(dotrikun_in_port);
	ZetSetOutHandler(dotrikun_out_port);
	ZetClose();

	DrvDoReset();
	return 0;
}


static int DrvExit()
{
	ZetExit();

	DrvReset = 0;
	free (Mem);
	Mem = NULL;

	return 0;
}


// Dottori Kun (new version)

static struct BurnRomInfo dotrikunRomDesc[] = {
	{ "14479a.mpr",	0x4000, 0xb77a50db, BRF_ESS | BRF_PRG }, //  Z80 code
};

STD_ROM_PICK(dotrikun);
STD_ROM_FN(dotrikun);

struct BurnDriver BurnDrvdotrikun = {
	"dotrikun", NULL, NULL, "1990",
	"Dottori Kun (new version)\0", NULL, "Sega", "Test Hardware",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 1, HARDWARE_MISC_PRE90S,
	NULL, dotrikunRomInfo, dotrikunRomName, DrvInputInfo, NULL,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 192, 4, 3
};


// Dottori Kun (old version)

static struct BurnRomInfo dotriku2RomDesc[] = {
	{ "14479.mpr",	0x4000, 0xa6aa7fa5, BRF_ESS | BRF_PRG }, //  Z80 code
};

STD_ROM_PICK(dotriku2);
STD_ROM_FN(dotriku2);

struct BurnDriver BurnDrvdotriku2 = {
	"dotriku2", "dotrikun", NULL, "1990",
	"Dottori Kun (old version)\0", NULL, "Sega", "Test Hardware",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 1, HARDWARE_MISC_PRE90S,
	NULL, dotriku2RomInfo, dotriku2RomName, DrvInputInfo, NULL,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 192, 4, 3
};


