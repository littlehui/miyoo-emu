// Wall Crash FBA Driver Module
// Based on MAME driver (wallc.c) by Jarek Burczynski

#include "burnint.h"
#include "bitswap.h"
#include "driver.h"
extern "C" {
#include "ay8910.h"
}


static unsigned char *Mem, *Rom, *Gfx, *Prom;
static int Palette[8];

static unsigned char DrvJoy1[8], DrvDips[2], DrvReset;
static unsigned short DrvAxis[1];
static unsigned int nAnalogAxis;

static short *pFMBuffer = NULL,  *pAY8910Buffer[3];

static int wallca;


#define A(a, b, c, d) { a, b, (unsigned char*)(c), d }

static struct BurnInputInfo DrvInputList[] = {
	{"Coin 1"    ,    BIT_DIGITAL  , DrvJoy1 + 0,	"p1 coin"   },
	{"Coin 2"  ,      BIT_DIGITAL  , DrvJoy1 + 1,	"p2 coin"   },
	{"Coin 3"     ,   BIT_DIGITAL  , DrvJoy1 + 2, 	"p3 coin"   },
	{"Start"      ,   BIT_DIGITAL  , DrvJoy1 + 3, 	"p1 start"  },
	{"P1 Button 1",	  BIT_DIGITAL,   DrvJoy1 + 4,   "p1 fire 1" },
	{"P1 Button 2",	  BIT_DIGITAL,   DrvJoy1 + 5,   "p1 fire 2" },

	A("P1 Right / left",	BIT_ANALOG_REL, DrvAxis + 0,	"mouse x-axis"),

	{"Service"  ,     BIT_DIGITAL  , DrvJoy1 + 6,	"diag"      },
	{"Reset"        , BIT_DIGITAL  , &DrvReset  ,	"reset"     },
	{"Dip 1"     ,    BIT_DIPSWITCH, DrvDips + 0  ,	"dip"	    },
	{"Dip 2"     ,    BIT_DIPSWITCH, DrvDips + 1  ,	"dip"	    },
};

STDINPUTINFO(Drv);


static struct BurnDIPInfo DrvDIPList[]=
{
	// Default Values
	{0x09, 0xff, 0xff, 0xff, NULL                     },

	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x09, 0x01, 0x03, 0x03, "5"       		  },
	{0x09, 0x01, 0x03, 0x02, "4"       		  },
	{0x09, 0x01, 0x03, 0x01, "3"       		  },
	{0x09, 0x01, 0x03, 0x00, "2"       		  },

	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x09, 0x01, 0x0c, 0x0c, "100K/200K/400K/800K"    },
	{0x09, 0x01, 0x0c, 0x08, "80K/160K/320K/640K"     },
	{0x09, 0x01, 0x0c, 0x04, "60K/120K/240K/480K"     },
	{0x09, 0x01, 0x0c, 0x00, "Off"    		  },

	{0   , 0xfe, 0   , 2   , "Curve Effect"           },
	{0x09, 0x01, 0x10, 0x10, "Normal"     		  },
	{0x09, 0x01, 0x10, 0x00, "More"		          },

	{0   , 0xfe, 0   , 4   , "Timer Speed"            },
	{0x09, 0x01, 0x60, 0x60, "Slow"     		  },
	{0x09, 0x01, 0x60, 0x40, "Normal"    		  },
	{0x09, 0x01, 0x60, 0x20, "Fast"     		  },
	{0x09, 0x01, 0x60, 0x00, "Super Fast" 		  },

	{0   , 0xfe, 0   , 2   , "Service" 	          },
	{0x09, 0x01, 0x80, 0x80, "Free Play and Level Select" },
	{0x09, 0x01, 0x80, 0x00, "Normal"    		  },

	// Default Values
	{0x0a, 0xff, 0xff, 0xff, NULL                     },

	{0   , 0xfe, 0   , 4   , "Coin A" 	          },
	{0x0a, 0x01, 0x03, 0x03, "2C 1C"       		  },
	{0x0a, 0x01, 0x03, 0x00, "1C 1C"       		  },
	{0x0a, 0x01, 0x03, 0x01, "1C 2C"       		  },
	{0x0a, 0x01, 0x03, 0x02, "1C 5C"       		  },

	{0   , 0xfe, 0   , 4   , "Coin B" 	          },
	{0x0a, 0x01, 0x0c, 0x0c, "2C 1C"       		  },
	{0x0a, 0x01, 0x0c, 0x00, "1C 1C"       		  },
	{0x0a, 0x01, 0x0c, 0x04, "1C 2C"       		  },
	{0x0a, 0x01, 0x0c, 0x08, "1C 5C"       		  },

	{0   , 0xfe, 0   , 4   , "Coin C" 	          },
	{0x0a, 0x01, 0x30, 0x30, "2C 1C"       		  },
	{0x0a, 0x01, 0x30, 0x00, "1C 1C"       		  },
	{0x0a, 0x01, 0x30, 0x10, "1C 2C"       		  },
	{0x0a, 0x01, 0x30, 0x20, "1C 5C"       		  },
};

STDDIPINFO(Drv);


void __fastcall wallc_write_handler(unsigned short a, unsigned char d)
{
	switch (a)
	{
		case 0xb000: // ?
		case 0xb100: // coin counter
		case 0xb200: // ?
		break;

		case 0xb500:
			AY8910Write(0, 0, d);
		break;

		case 0xb600:
			AY8910Write(0, 1, d);
		break;
	}
}

unsigned char __fastcall wallc_read_handler(unsigned short a)
{
	unsigned char ret = 0;

	switch (a)
	{
		case 0xb000: 			// input port 0
			ret = DrvDips[0];
		break;

		case 0xb200:			// input port 1
			ret |= DrvJoy1[5];
			ret |= DrvJoy1[6] << 2;
			ret |= DrvJoy1[4] << 3;
			ret |= DrvJoy1[0] << 4;
			ret |= DrvJoy1[1] << 5;
			ret |= DrvJoy1[2] << 6;
			ret |= DrvJoy1[3] << 7;
			ret ^= 0xff;
		break;

		case 0xb400:			// input port 2
			nAnalogAxis -= DrvAxis[0];
			ret = (nAnalogAxis >> 8) & 0xFF;
		break;

		case 0xb600:			// input port 3
			ret = DrvDips[1];
		break; 
	}

	return ret;
}


static int DrvDoReset()
{
	DrvReset = 0;
	nAnalogAxis = 0;

	memset(Rom + 0x8000, 0, 0x0400);
	memset(Rom + 0xa000, 0, 0x0400);

	ZetOpen(0);
	ZetReset();
	ZetClose();

	AY8910Reset(0);

	return 0;
}

static void wallc_palette_init()
{
	int i;
	for (i = 8; i < 16; i++)
	{
		int bit0,bit1,bit7,r,g,b;

		// red component
		bit0 = (Prom[i] >> 5) & 0x01;
		bit1 = (Prom[i] >> 6) & 0x01;
		r = ((77 * bit1) + (115 * bit0)) + 1;

		// green component
		bit0 = (Prom[i] >> 2) & 0x01;
		bit1 = (Prom[i] >> 3) & 0x01;
		g = ((77 * bit1) + (115 * bit0)) + 1;

		// blue component
		bit0 = (Prom[i] >> 0) & 0x01;
		bit1 = (Prom[i] >> 1) & 0x01;
		bit7 = (Prom[i] >> 7) & 0x01;
		b = ((54 * bit7) + (84 * bit1) + (115 * bit0));

		Palette[i-8] = (r << 16) | (g << 8) | b;
	}
}

static int DrvInit()
{
	Mem = (unsigned char*)malloc(0x10000 + 0x04000 + 0x00020);
	if (Mem == NULL) {
		return 1;
	}

	pFMBuffer = (short *)malloc (nBurnSoundLen * 3 * sizeof(short));
	if (pFMBuffer == NULL) {
		return 1;
	}

	Rom  = Mem + 0x00000;
	Gfx  = Mem + 0x10000;
	Prom = Mem + 0x14000;

	pAY8910Buffer[0] = pFMBuffer + nBurnSoundLen * 0;
	pAY8910Buffer[1] = pFMBuffer + nBurnSoundLen * 1;
	pAY8910Buffer[2] = pFMBuffer + nBurnSoundLen * 2;

	{
		if (BurnLoadRom(Rom + 0x0000, 0, 1)) return 1;
		if (BurnLoadRom(Rom + 0x2000, 1, 1)) return 1;

		if (BurnLoadRom(Gfx + 0x0000 + 0x800 * wallca, 2, 1)) return 1;
		if (BurnLoadRom(Gfx + 0x1000 + 0x800 * wallca, 3, 1)) return 1;
		if (BurnLoadRom(Gfx + 0x2000 + 0x800 * wallca, 4, 1)) return 1;

		if (BurnLoadRom(Prom,       5, 1)) return 1;
	}

	// Convert gfx
	{
		int i;
		unsigned char a, b, c;
		unsigned char *tmp = (unsigned char*)malloc(0x8000);
		for (i = 0; i < 0x1000 * 8; i++)
		{
			a = (Gfx[(i >> 3) + 0x0000] >> (i & 7)) & 1;
			b = (Gfx[(i >> 3) + 0x1000] >> (i & 7)) & 1;
			c = (Gfx[(i >> 3) + 0x2000] >> (i & 7)) & 1;

			tmp[i] = (a << 2) | (b << 1) | c;
		}
		memcpy (Gfx, tmp + 0x4000, 0x4000);
		free (tmp);

		wallc_palette_init();
	}

	ZetInit(1);
	ZetOpen(0);
	ZetSetReadHandler(wallc_read_handler);
	ZetSetWriteHandler(wallc_write_handler);
	ZetMapArea(0x0000, 0x7fff, 0, Rom + 0x0000);
	ZetMapArea(0x0000, 0x7fff, 2, Rom + 0x0000);

	ZetMapArea(0x8000, 0x83ff, 0, Rom + 0x8000);
	ZetMapArea(0x8000, 0x83ff, 1, Rom + 0x8000);
	ZetMapArea(0x8000, 0x83ff, 2, Rom + 0x8000);

	ZetMapArea(0x8c00, 0x8fff, 0, Rom + 0x8000); // Mirror
	ZetMapArea(0x8c00, 0x8fff, 1, Rom + 0x8000);
	ZetMapArea(0x8c00, 0x8fff, 2, Rom + 0x8000);

	ZetMapArea(0xa000, 0xa3ff, 0, Rom + 0xa000);
	ZetMapArea(0xa000, 0xa3ff, 1, Rom + 0xa000);
	ZetMapArea(0xa000, 0xa3ff, 2, Rom + 0xa000);
	ZetMemEnd();
	ZetClose();

	AY8910Init(0, 1536000, nBurnSoundRate, NULL, NULL, NULL, NULL);

	DrvDoReset();

	return 0;
}

static int DrvExit()
{
	ZetExit();
	AY8910Exit(0);

	free (Mem);
	free (pFMBuffer);

	Mem = Rom = Gfx = Prom = NULL;
	pAY8910Buffer[0] = pAY8910Buffer[1] = pAY8910Buffer[2] = NULL;

	return 0;
}


static int DrvDraw()
{
	unsigned char *vram = Rom + 0x8000;

	int offs;
	for (offs = 0; offs < 0x400; offs ++)
	{
		int sy = (~offs & 0x1f) << 3;
		int sx = (offs >> 2) & 0xf8;
		unsigned char *src = Gfx + (vram[offs] << 6);

		for (int y = sy; y < sy + 8; y++)
		{
			for (int x = sx; x < sx + 8; x++, src++)
			{
				int pxl = Palette[src[0]];

				PutPix(pBurnDraw + ((y << 8) | x) * nBurnBpp, BurnHighCol(pxl >> 16, pxl >> 8, pxl, 0));
			}
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
	ZetRun(3072000 / 60);
	ZetSetIRQLine(0, ZET_IRQSTATUS_AUTO);
	ZetClose();

	if (pBurnSoundOut) {
		int nSample;
		int nSegmentLength = nBurnSoundLen;
		short* pSoundBuf = pBurnSoundOut;
		if (nSegmentLength) {
			AY8910Update(0, &pAY8910Buffer[0], nSegmentLength);
			for (int n = 0; n < nSegmentLength; n++) {
				nSample  = pAY8910Buffer[0][n];
				nSample += pAY8910Buffer[1][n];
				nSample += pAY8910Buffer[2][n];

				nSample /= 4;

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


static int DrvScan(int nAction,int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {					// Return minimum compatible version
		*pnMin = 0x029521;
	}

	if (nAction & ACB_VOLATILE) {			// Scan volatile ram		
		memset(&ba, 0, sizeof(ba));

		ba.Data	  = Rom + 0x8000;
		ba.nLen	  = 0x0400;
		ba.szName = "Video Ram";
		BurnAcb(&ba);

		ba.Data	  = Rom + 0xa000;
		ba.nLen	  = 0x0400;
		ba.szName = "Main Ram";
		BurnAcb(&ba);

		ZetScan(nAction);			// Scan Z80

		AY8910Scan(nAction, pnMin);		// Scan AY8910

		// Scan critical driver variables
		SCAN_VAR(nAnalogAxis);
	}

	return 0;
}



// Wall Crash (set 1)

static struct BurnRomInfo wallcRomDesc[] = {
	{ "wac05.h7",	0x2000, 0xab6e472e, 0 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "wac1-52.h6",	0x2000, 0x988eaa6d, 0 | BRF_ESS | BRF_PRG }, //  1

	{ "wc1.e3",	0x1000, 0xca5c4b53, 1 | BRF_GRA },	     //  2 Graphics
	{ "wc2.e2",	0x1000, 0xb7f52a59, 1 | BRF_GRA },	     //  3
	{ "wc3.e1",	0x1000, 0xf6854b3a, 1 | BRF_GRA },	     //  4

	{ "74s288.c2",  0x0020, 0x83e3e293, 2 | BRF_GRA },	     //  5 Color Prom
};

STD_ROM_PICK(wallc);
STD_ROM_FN(wallc);

static int wallcInit()
{
	wallca = 0;

	int nRet = DrvInit();

	if (nRet == 0) {
		for (int i = 0; i < 0x4000; i++) {
			Rom[i] = BITSWAP08(Rom[i] ^ 0xAA, 4,2,6,0,7,1,3,5);
		}
	}

	return nRet;
}

struct BurnDriver BurnDrvwallc = {
	"wallc", NULL, NULL, "1984",
	"Wall Crash (set 1)\0", NULL, "Midcoin", "Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 1, HARDWARE_MISC_PRE90S,
	NULL, wallcRomInfo, wallcRomName, DrvInputInfo, DrvDIPInfo,
	wallcInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Wall Crash (set 2)

static struct BurnRomInfo wallcaRomDesc[] = {
	{ "rom4.rom",	0x2000, 0xce43af1b, 0 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "rom5.rom",	0x2000, 0xb789a705, 0 | BRF_ESS | BRF_PRG }, //  1

	{ "rom3.rom",	0x0800, 0x6634db73, 1 | BRF_GRA },	     //  2 Graphics
	{ "rom2.rom",	0x0800, 0x79f49c2c, 1 | BRF_GRA },	     //  3
	{ "rom1.rom",   0x0800, 0x3884fd4f, 1 | BRF_GRA },	     //  4

	{ "74s288.c2",	0x0020, 0x83e3e293, 2 | BRF_GRA },	     //  5 Color Prom
};

STD_ROM_PICK(wallca);
STD_ROM_FN(wallca);

static int wallcaInit()
{
	wallca = 1;

	int nRet = DrvInit();

	if (nRet == 0) {
		for (int i = 0; i < 0x4000; i++) {
			if(i & 0x100) {
				Rom[i] = BITSWAP08(Rom[i] ^ 0x4a, 4,7,1,3,2,0,5,6);
			} else {
				Rom[i] = BITSWAP08(Rom[i] ^ 0xa5, 0,2,3,6,1,5,7,4);
			}
		}
	}

	return nRet;
}

struct BurnDriver BurnDrvwallca = {
	"wallca", "wallc", NULL, "1984",
	"Wall Crash (set 2)\0", NULL, "Midcoin", "Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 1, HARDWARE_MISC_PRE90S,
	NULL, wallcaRomInfo, wallcaRomName, DrvInputInfo, DrvDIPInfo,
	wallcaInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


