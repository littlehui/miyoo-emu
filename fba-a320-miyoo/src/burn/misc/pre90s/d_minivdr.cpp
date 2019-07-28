
/*
Minivader (Space Invaders's mini game)
(c)1990 Taito Corporation

This is a test board sold together with the cabinet (as required by law in
Japan). It has no sound.

Based on MAME Driver by Takahiro Nogi
*/

#include "burnint.h"


static unsigned char DrvJoy[4], DrvReset, *Mem;

static struct BurnInputInfo DrvInputList[] = {
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy + 0, 	"p1 left"  },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy + 1, 	"p1 right" },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy + 2,	"p1 fire 1"},
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy + 3,	"p1 coin"  },

	{"Reset"        , BIT_DIGITAL  , &DrvReset  ,	"reset"    },
};

STDINPUTINFO(Drv);


unsigned char __fastcall MinivadrMemRead(unsigned short a)
{
	if (a == 0xe008) {
		return ~(DrvJoy[0] | (DrvJoy[1] << 1) | (DrvJoy[2] << 2) | (DrvJoy[3] << 3));
	}

	return 0;
}


static int DrvScan(int nAction,int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {
		*pnMin = 0x029521;
	}

	if (nAction & ACB_VOLATILE) {	
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = Mem + 0x2000;
		ba.nLen	  = 0x2000;
		ba.szName = "All Ram";
		BurnAcb(&ba);

		ZetScan(nAction);
	}

	return 0;
}


static int DrvDoReset()
{
	DrvReset = 0;
	memset (Mem + 0x2000, 0, 0x2000);

	ZetOpen(0);
	ZetReset();
	ZetClose();

	return 0;
}


static int DrvDraw()
{
	int a, i, y;
	unsigned char c, d, x;

	for (a = 0x200; a < 0x1e00; a++)
	{
		x = a << 3;
		y = ((a >> 5) - 0x10) << 8;
		d = Mem[0x2000 + a];

		for (i = 0; i < 8; i++)
		{
			c = (d & 0x80) ? 0xff : 0;
			PutPix(pBurnDraw + (x + y) * nBurnBpp, BurnHighCol(c, c, c, 0));

			d <<= 1;
			x += 1;
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
	ZetSetIRQLine(0xFF, ZET_IRQSTATUS_AUTO);
	ZetClose();

	if (pBurnDraw) {
		DrvDraw();
	}

	return 0;
}


static int DrvInit()
{
	Mem = (unsigned char*)malloc ( 0x4000 );
	if (Mem == NULL) {
		return 1;
	}

	if (BurnLoadRom(Mem, 0, 1)) return 1;

	ZetInit(1);
	ZetOpen(0);
	ZetMapArea (0x0000, 0x1fff, 0, Mem + 0x0000); // Read ROM
	ZetMapArea (0x0000, 0x1fff, 2, Mem + 0x0000); // Fetch ROM
	ZetMapArea (0xa000, 0xbfff, 0, Mem + 0x2000); // Read RAM
	ZetMapArea (0xa000, 0xbfff, 1, Mem + 0x2000); // Write RAM
	ZetSetReadHandler(MinivadrMemRead);
	ZetMemEnd();
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


// Minivader

static struct BurnRomInfo minivadrRomDesc[] = {
	{ "d26-01.bin",	0x2000, 0xa96c823d, BRF_ESS | BRF_PRG }, //  Z80 code
};

STD_ROM_PICK(minivadr);
STD_ROM_FN(minivadr);

struct BurnDriver BurnDrvminivadr = {
	"minivadr", NULL, NULL, "1990",
	"Minivader\0", NULL, "Taito Corporation", "Minivader",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 1, HARDWARE_MISC_PRE90S,
	NULL, minivadrRomInfo, minivadrRomName, DrvInputInfo, NULL,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 224, 4, 3
};


