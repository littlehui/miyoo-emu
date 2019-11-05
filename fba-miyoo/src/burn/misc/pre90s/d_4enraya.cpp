/***************************************************************************

Based on MAME driver by Tomasz Slanina  dox@space.pl

***************************************************************************

RAM :
    1 x GM76c28-10 (6116) RAM
    3 x 2114  - VRAM (only 10 bits are used )

ROM:
  27256 + 27128 for code/data
  3x2764 for gfx

PROM:
 82S123 32x8
 Used for system control
    (d0 - connected to ROM5 /CS , d1 - ROM4 /CS, d2 - RAM /CS , d3 - to some logic(gfx control), and Z80 WAIT )

Memory Map :
  0x0000 - 0xbfff - ROM
  0xc000 - 0xcfff - RAM
  0xd000 - 0xdfff - VRAM mirrored write,
        tilemap offset = address & 0x3ff
        tile number =  bits 0-7 = data, bits 8,9  = address bits 10,11

Video :
    No scrolling , no sprites.
    32x32 Tilemap stored in VRAM (10 bits/tile (tile numebr 0-1023))

    3 gfx ROMS
    ROM1 - R component (ROM ->(parallel in) shift register 74166 (serial out) -> jamma output
    ROM2 - B component
    ROM3 - G component

    Default MAME color palette is used

Sound :
 AY 3 8910

 sound_control :

  bit 0 - BC1
  bit 1 - BC2
  bit 2 - BDIR

  bits 3-7 - not connected

***************************************************************************/

#include "burnint.h"
#include "driver.h"
extern "C" {
#include "ay8910.h"
}

static unsigned char *Mem, *Rom, *Gfx;
static unsigned char DrvJoy1[6], DrvJoy2[6], DrvReset, DrvDips;
static short *pAY8910Buffer[3], *pFMBuffer = NULL;
static unsigned int Palette[8];
static unsigned char soundlatch;

static struct BurnInputInfo DrvInputList[] = {
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy1 + 0,	"p1 coin"  },
	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy1 + 1,	"p1 start" },
	{"P1 Down",	  BIT_DIGITAL,   DrvJoy1 + 2,   "p1 down", },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy1 + 3, 	"p1 left"  },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy1 + 4, 	"p1 right" },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 5,	"p1 fire 1"},

	{"P2 Coin"      , BIT_DIGITAL  , DrvJoy2 + 0,	"p2 coin"  },
	{"P2 start"  ,    BIT_DIGITAL  , DrvJoy2 + 1,	"p2 start" },
	{"P2 Down",	  BIT_DIGITAL,   DrvJoy2 + 2,   "p2 down", },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy2 + 3, 	"p2 left"  },
	{"P2 Right"     , BIT_DIGITAL  , DrvJoy2 + 4, 	"p2 right" },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy2 + 5,	"p2 fire 1"},

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, &DrvDips,	"dip"	   },
};

STDINPUTINFO(Drv);


static struct BurnDIPInfo DrvDIPList[]=
{
	// Default Values
	{0x0d, 0xff, 0xff, 0xff, NULL                     },

	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x0d, 0x01, 0x01, 0x01, "Easy"       		  },
	{0x0d, 0x01, 0x01, 0x00, "Hard"       		  },

	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x0d, 0x01, 0x02, 0x02, "Off"     		  },
	{0x0d, 0x01, 0x02, 0x00, "On"    		  },

	{0   , 0xfe, 0   , 2   , "Pieces"	          },
	{0x0d, 0x01, 0x04, 0x04, "30"     		  },
	{0x0d, 0x01, 0x04, 0x00, "16"			  },

	{0   , 0xfe, 0   , 2   , "Speed"                  },
	{0x0d, 0x01, 0x08, 0x08, "Slow"     		  },
	{0x0d, 0x01, 0x08, 0x00, "Fast"    		  },

	{0   , 0xfe, 0   , 4   , "Coin B"                 },
	{0x0d, 0x01, 0x30, 0x30, "1C 1C"     		  },
	{0x0d, 0x01, 0x30, 0x00, "2C 3C"    		  },
	{0x0d, 0x01, 0x30, 0x10, "1C 3C"     		  },
	{0x0d, 0x01, 0x30, 0x20, "1C 4C"    		  },

	{0   , 0xfe, 0   , 4   , "Coin A"                 },
	{0x0d, 0x01, 0xc0, 0x40, "2C 1C"     		  },
	{0x0d, 0x01, 0xc0, 0xc0, "1C 1C"    		  },
	{0x0d, 0x01, 0xc0, 0x00, "2C 3C"     		  },
	{0x0d, 0x01, 0xc0, 0x80, "1C 2C"    		  },
};

STDDIPINFO(Drv);


static void sound_control_w(unsigned short, unsigned char data)
{
	static int last;

	if ((last & 0x04) == 0x04 && (data & 0x4) == 0x00)
	{
		if (last & 0x01)
			AY8910Write(0, 0, soundlatch);
		else
			AY8910Write(0, 1, soundlatch);
	}

	last = data;
}


void __fastcall enraya4_out_port(unsigned short a, unsigned char d)
{
	switch (a & 0xff)
	{
		case 0x23:
			soundlatch = d;
		break;

		case 0x33:
			sound_control_w(a, d);
		break;
	}
}


unsigned char __fastcall enraya4_in_port(unsigned short a)
{
	static unsigned char nRet = 0; 

	switch (a & 0xff)
	{
		case 0x00:
			nRet = DrvDips;
		break;

		case 0x01:
			nRet  = DrvJoy2[3];
			nRet |= DrvJoy1[3] << 1;
			nRet |= DrvJoy1[2] << 2;
			nRet |= DrvJoy2[4] << 3;
			nRet |= DrvJoy2[2] << 4;
			nRet |= DrvJoy1[4] << 5;
			nRet |= DrvJoy1[5] << 6;
			nRet |= DrvJoy2[5] << 7;
			nRet ^= 0xff;
		break;
	
		case 0x02:
			nRet  = DrvJoy1[0] << 1;
			nRet |= DrvJoy2[0] << 7;
			nRet |= DrvJoy1[1] << 6;
			nRet |= DrvJoy2[1] << 5;
			nRet ^= 0xff;
		break;
	}

	return nRet;
}


void __fastcall enraya4_write(unsigned short a, unsigned char d)
{
	if (a >= 0xd000 && a <= 0xdfff)
	{
		Rom[0xd000 | ((a&0x3ff) << 1) | 0] = d;
		Rom[0xd000 | ((a&0x3ff) << 1) | 1] = (a & 0xc00) >> 10;
	}
}


static int DrvDoReset()
{
	DrvReset = 0;
	memset (Rom + 0xc000, 0, 0x4000);

	ZetOpen(0);
	ZetReset();
	ZetClose();

	soundlatch = 0;
	AY8910Reset(0);

	return 0;
}


static int DrvInit()
{
	Mem = (unsigned char *)malloc( 0x16000 );
	if (Mem == NULL) {
		return 1;
	}

	pFMBuffer = (short *)malloc (nBurnSoundLen * 3 * sizeof(short));
	if (pFMBuffer == NULL) {
		return 1;
	}

	Rom   = Mem + 0x00000;
	Gfx   = Mem + 0x10000;

	pAY8910Buffer[0] = pFMBuffer + nBurnSoundLen * 0;
	pAY8910Buffer[1] = pFMBuffer + nBurnSoundLen * 1;
	pAY8910Buffer[2] = pFMBuffer + nBurnSoundLen * 2;

	// Load Roms
	{
		BurnLoadRom(Rom + 0x00000, 0, 1);
		BurnLoadRom(Rom + 0x08000, 1, 1);

		BurnLoadRom(Gfx + 0x00000, 2, 1);
		BurnLoadRom(Gfx + 0x02000, 3, 1);
		BurnLoadRom(Gfx + 0x04000, 4, 1);

		// BurnLoadRom(Prom, 5, 1);	// Prom not used
	}

	ZetInit(1);
	ZetOpen(0);
	ZetSetInHandler(enraya4_in_port);
	ZetSetOutHandler(enraya4_out_port);
	ZetSetWriteHandler(enraya4_write);
	ZetMapArea(0x0000, 0xffff, 0, Rom);
	ZetMapArea(0x0000, 0xbfff, 2, Rom);
	ZetMapArea(0xc000, 0xcfff, 1, Rom + 0xc000);
	ZetMemEnd();
	ZetClose();

	// Initilize palette
	{
		for (int i = 0; i < 8; i++) {
			Palette[i]  = (i & 1) ? 0xff0000 : 0;
			Palette[i] |= (i & 2) ? 0x00ff00 : 0;
			Palette[i] |= (i & 4) ? 0x0000ff : 0;
		}
	}

	AY8910Init(0, 2000000, nBurnSoundRate, NULL, NULL, NULL, NULL);

	DrvDoReset();

	return 0;
}


static int DrvExit()
{
	ZetExit();
	AY8910Exit(0);

	DrvReset = 0;
	free (Mem);
	free (pFMBuffer);

	pFMBuffer = NULL;
	Mem = Rom = Gfx = NULL;

	return 0;
}


static int DrvDraw()
{
#define cl(v)	(v & 0x80) ? 0xff : 0

	for (int i = 0x80; i < 0x800 - 0x80; i+=2)
	{
		int x1 = ((i >> 1) & 0x1f) << 3;
		int y1 = ((i >> 6) << 11) - 0x1000;

		int tile_no = (Rom[0xd000 | i] | (Rom[0xd000 | i | 1] << 8)) << 3;

		for (int y = y1; y < y1 + 256 * 8; y+=256, tile_no++)
		{
			unsigned char r0 = Gfx[0x0000 + tile_no];
			unsigned char g0 = Gfx[0x4000 + tile_no];
			unsigned char b0 = Gfx[0x2000 + tile_no];

			for (int x = x1; x < x1 + 8; x++)
			{
				PutPix(pBurnDraw + (y | x) * nBurnBpp, BurnHighCol(cl(r0), cl(g0), cl(b0), 0));

				r0 <<= 1;
				g0 <<= 1;
				b0 <<= 1;
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
	ZetRun(4000000 / 60);
	ZetSetIRQLine(0, 4);
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

	if (pnMin) {						// Return minimum compatible version
		*pnMin = 0x029521;
	}

	if (nAction & ACB_VOLATILE) {		// Scan volatile ram		
		memset(&ba, 0, sizeof(ba));

		ba.Data	  = Rom + 0xc000;
		ba.nLen	  = 0x4000;
		ba.szName = "All Ram";
		BurnAcb(&ba);

		ZetScan(nAction);			// Scan Z80
		AY8910Scan(nAction, pnMin);		// Scan AY8910

		// Scan critical driver variables
		SCAN_VAR(soundlatch);
	}

	return 0;
}


// 4 En Raya

static struct BurnRomInfo enraya4RomDesc[] = {
	{ "5.bin",   0x8000, 0xcf1cd151, BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "4.bin",   0x4000, 0xf9ec1be7, BRF_ESS | BRF_PRG }, //  1

	{ "1.bin",   0x2000, 0x87f92552, BRF_GRA },	      //  2 Graphics
	{ "2.bin",   0x2000, 0x2b0a3793, BRF_GRA },	      //  3
	{ "3.bin",   0x2000, 0xf6940836, BRF_GRA },	      //  4

	{ "1.bpr",   0x0020, 0xdcbd2352, BRF_GRA },	      //  5 - not used
};

STD_ROM_PICK(enraya4);
STD_ROM_FN(enraya4);

struct BurnDriver BurnDrvenraya4 = {
	"4enraya", NULL, NULL, "1990",
	"4 En Raya\0", NULL, "IDSA", "misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, enraya4RomInfo, enraya4RomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 224, 4, 3
};

