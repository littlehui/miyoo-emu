
#include "burnint.h"
#include "driver.h"
extern "C" {
#include "ay8910.h"
}
#include "bitswap.h"

static unsigned int pens[0x20];
static unsigned char *eposMem = NULL;
static unsigned char *eposRom, *eposRam;
static unsigned char DrvReset, DrvDips, palette;
static unsigned char DrvJoy[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static short* pAY8910Buffer[3];
static short *pFMBuffer = NULL;

//----------------------------------------------------------------------------------------------
// Game inputs

static struct BurnInputInfo DrvInputList[] = {
	{"Coin",		    BIT_DIGITAL,	DrvJoy + 0,	"p1 coin"},
	{"Start 1",		  BIT_DIGITAL,	DrvJoy + 1,	"p1 start"},
	{"Start 2",		  BIT_DIGITAL,	DrvJoy + 2, "p2 start"},

	{"P1 Up",		    BIT_DIGITAL,	DrvJoy + 7, "p1 up"},
	{"P1 Down",		  BIT_DIGITAL,	DrvJoy + 8, "p1 down"},
	{"P1 Left",		  BIT_DIGITAL,	DrvJoy + 4, "p1 left"},
	{"P1 Right",	  BIT_DIGITAL,	DrvJoy + 3, "p1 right"},
	{"P1 Button 1", BIT_DIGITAL,	DrvJoy + 5,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	DrvJoy + 6,	"p1 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Service Mode",	BIT_DIGITAL,	DrvJoy + 10,	"diag"},
	{"Dip Switches",	BIT_DIPSWITCH,	&DrvDips,	"dip"},
};

STDINPUTINFO(Drv);

static struct BurnDIPInfo megadonDIPList[] =
{
	// Defaults
	{0x0B, 0xFF, 0xFF, 0x00, NULL},

	{0,		0xFE, 0,	2,	  "Coinage"},
	{0x0B, 0x01, 0x01, 0x00, "1 Coin 1 Credit"},
	{0x0B, 0x01, 0x01, 0x01, "1 Coin 2 Credits"},

	{0,		0xFE, 0,	2,	  "Fuel Consumption"},
	{0x0B, 0x01, 0x02, 0x00, "Slow"},
	{0x0B, 0x01, 0x02, 0x02, "Fast"},

	{0,		0xFE, 0,	2,	  "Rotation"},
	{0x0B, 0x01, 0x04, 0x04, "Slow"},
	{0x0B, 0x01, 0x04, 0x00, "Fast"},

	{0,		0xFE, 0,	2,	  "ERG"},
	{0x0B, 0x01, 0x08, 0x08, "Easy"},
	{0x0B, 0x01, 0x08, 0x00, "Hard"},

	{0,		0xFE, 0,	2,	  "Enemy Fire Rate"},
	{0x0B, 0x01, 0x20, 0x20, "Slow"},
	{0x0B, 0x01, 0x20, 0x00, "Fast"},

	{0,		0xFE, 0,	4,	  "Lives"},
	{0x0B, 0x01, 0x50, 0x00, "3"},
	{0x0B, 0x01, 0x50, 0x10, "4"},
	{0x0B, 0x01, 0x50, 0x40, "5"},
	{0x0B, 0x01, 0x50, 0x50, "6"},

	{0,		0xFE, 0,	2,	  "Game Mode"},
	{0x0B, 0x01, 0x80, 0x00, "Arcade"},
	{0x0B, 0x01, 0x80, 0x80, "Contest"},
};

STDDIPINFO(megadon);

static struct BurnDIPInfo suprglobDIPList[] =
{
	// Defaults
	{0x0B, 0xFF, 0xFF, 0x00, NULL},

	{0,		0xFE, 0,	2,	  "Coinage"},
	{0x0B, 0x01, 0x01, 0x00, "1 Coin 1 Credit"},
	{0x0B, 0x01, 0x01, 0x01, "1 Coin 2 Credits"},

	{0,		0xFE, 0,	8,	  "Difficulty"},
	{0x0B, 0x01, 0x26, 0x00, "1"},
	{0x0B, 0x01, 0x26, 0x02, "2"},
	{0x0B, 0x01, 0x26, 0x20, "3"},
	{0x0B, 0x01, 0x26, 0x22, "4"},
	{0x0B, 0x01, 0x26, 0x04, "5"},
	{0x0B, 0x01, 0x26, 0x06, "6"},
	{0x0B, 0x01, 0x26, 0x24, "7"},
	{0x0B, 0x01, 0x26, 0x26, "8"},

	// Extra Glob condition
	{0,		0xFE, 0,	2,	  "Bonus Life"},
	{0x0B, 0x02, 0x08, 0x00, "20000 pts"},
	{0x0B, 0x00, 0x26, 0x00, NULL},
	{0x0B, 0x02, 0x08, 0x08, "100000 pts"},
	{0x0B, 0x00, 0x26, 0x00, NULL},
	{0,		0xFE, 0,	2,	  "Bonus Life"},
	{0x0B, 0x02, 0x08, 0x00, "30000 pts"},
	{0x0B, 0x00, 0x26, 0x02, NULL},
	{0x0B, 0x02, 0x08, 0x08, "110000 pts"},
	{0x0B, 0x00, 0x26, 0x02, NULL},
	{0,		0xFE, 0,	2,	  "Bonus Life"},
	{0x0B, 0x02, 0x08, 0x00, "40000 pts"},
	{0x0B, 0x00, 0x26, 0x20, NULL},
	{0x0B, 0x02, 0x08, 0x08, "120000 pts"},
	{0x0B, 0x00, 0x26, 0x20, NULL},
	{0,		0xFE, 0,	2,	  "Bonus Life"},
	{0x0B, 0x02, 0x08, 0x00, "50000 pts"},
	{0x0B, 0x00, 0x26, 0x22, NULL},
	{0x0B, 0x02, 0x08, 0x08, "130000 pts"},
	{0x0B, 0x00, 0x26, 0x22, NULL},
	{0,		0xFE, 0,	2,	  "Bonus Life"},
	{0x0B, 0x02, 0x08, 0x00, "60000 pts"},
	{0x0B, 0x00, 0x26, 0x04, NULL},
	{0x0B, 0x02, 0x08, 0x08, "140000 pts"},
	{0x0B, 0x00, 0x26, 0x04, NULL},
	{0,		0xFE, 0,	2,	  "Bonus Life"},
	{0x0B, 0x02, 0x08, 0x00, "70000 pts"},
	{0x0B, 0x00, 0x26, 0x06, NULL},
	{0x0B, 0x02, 0x08, 0x08, "150000 pts"},
	{0x0B, 0x00, 0x26, 0x06, NULL},
	{0,		0xFE, 0,	2,	  "Bonus Life"},
	{0x0B, 0x02, 0x08, 0x00, "80000 pts"},
	{0x0B, 0x00, 0x26, 0x24, NULL},
	{0x0B, 0x02, 0x08, 0x08, "160000 pts"},
	{0x0B, 0x00, 0x26, 0x24, NULL},
	{0,		0xFE, 0,	2,	  "Bonus Life"},
	{0x0B, 0x02, 0x08, 0x00, "90000 pts"},
	{0x0B, 0x00, 0x26, 0x26, NULL},
	{0x0B, 0x02, 0x08, 0x08, "170000 pts"},
	{0x0B, 0x00, 0x26, 0x26, NULL},

	{0,		0xFE, 0,	4,	  "Lives"},
	{0x0B, 0x01, 0x50, 0x00, "3"},
	{0x0B, 0x01, 0x50, 0x10, "4"},
	{0x0B, 0x01, 0x50, 0x40, "5"},
	{0x0B, 0x01, 0x50, 0x50, "6"},

	{0,		0xFE, 0,	2,	  "Demo Sounds"},
	{0x0B, 0x01, 0x80, 0x80, "Off"},
	{0x0B, 0x01, 0x80, 0x00, "On"},

};

STDDIPINFO(suprglob);

static struct BurnDIPInfo igmoDIPList[] =
{
	// Defaults
	{0x0B, 0xFF, 0xFF, 0x00, NULL},

	{0,		0xFE, 0,	2,	  "Coinage"},
	{0x0B, 0x01, 0x01, 0x00, "1 Coin 1 Credit"},
	{0x0B, 0x01, 0x01, 0x01, "1 Coin 2 Credits"},

	{0,		0xFE, 0,	4,	  "Bonus Life"},
	{0x0B, 0x01, 0x22, 0x00, "20000"},
	{0x0B, 0x01, 0x22, 0x02, "40000"},
	{0x0B, 0x01, 0x22, 0x20, "60000"},
	{0x0B, 0x01, 0x22, 0x22, "80000"},

	{0,		0xFE, 0,	8,	  "Difficulty"},
	{0x0B, 0x01, 0x8c, 0x00, "1"},
	{0x0B, 0x01, 0x8c, 0x04, "2"},
	{0x0B, 0x01, 0x8c, 0x08, "3"},
	{0x0B, 0x01, 0x8c, 0x0c, "4"},
	{0x0B, 0x01, 0x8c, 0x80, "5"},
	{0x0B, 0x01, 0x8c, 0x84, "6"},
	{0x0B, 0x01, 0x8c, 0x88, "7"},
	{0x0B, 0x01, 0x8c, 0x8c, "8"},

	{0,		0xFE, 0,	4,	  "Lives"},
	{0x0B, 0x01, 0x50, 0x00, "3"},
	{0x0B, 0x01, 0x50, 0x10, "4"},
	{0x0B, 0x01, 0x50, 0x40, "5"},
	{0x0B, 0x01, 0x50, 0x50, "6"},
};

STDDIPINFO(igmo);


//----------------------------------------------------------------------------------------------
// Hardware emulation

unsigned char __fastcall epos_read_port(unsigned short a)
{
	unsigned char ret;

	switch (a & 0xff)
	{
		case 0x00:
			return DrvDips;

		case 0x01:
			ret  = DrvJoy[9];	// highest 2 bits used as protection
			ret |= DrvJoy[10] << 4;	// service mode
			ret |= DrvJoy[0];
			ret |= DrvJoy[1] << 2;
			ret |= DrvJoy[2] << 3;

			return ret ^ 0x3e; 

		case 0x02:
			ret  = DrvJoy[3];
			ret |= DrvJoy[4] << 1;
			ret |= DrvJoy[5] << 2;
			ret |= DrvJoy[6] << 3;
			ret |= DrvJoy[7] << 4;
			ret |= DrvJoy[8] << 5;

			return ret ^ 0xff;

		case 0x03:
			return 0;	// input 3, not used
	}

	return a;
}


void __fastcall epos_write_port(unsigned short a, unsigned char d)
{
	switch (a & 0xff)
	{
		case 0x00: 	// watchdog
		break;

		case 0x01:	// choose palette
			palette = (d << 1) & 0x10;
		break;

		case 0x02: 	// AY8910 write port 0
			AY8910Write(0, 1, d);
		break;

		case 0x06: 	// AY8910 control port 0
			AY8910Write(0, 0, d);
		break;
	}
}


static int DrvDoReset()
{
	memset (eposRam, 0, 0x08800);
	palette = 0;
	DrvReset = 0;

	ZetOpen(0);
	ZetReset();
	ZetClose();

	AY8910Reset(0);

	return 0;
}


static int DrvInit()
{
	eposMem = (unsigned char *)calloc(1, 0x10000);
	if (eposMem == NULL) {
		return 1;
	}

	pFMBuffer = (short *)malloc (nBurnSoundLen * 3 * sizeof(short));
	if (pFMBuffer == NULL) {
		return 1;
	}

	eposRom = eposMem;
	eposRam = eposMem + 0x07800;

	// Load program Roms
	for (int i = 0; i < 8; i++) {
		if (BurnLoadRom(eposRom + i * 0x1000, i, 1)) return 1;
	}

	// Load palette Prom
	{
		unsigned char prom[32] = { // in case the set lacks a prom dump
			0x00, 0xE1, 0xC3, 0xFC, 0xEC, 0xF8, 0x34, 0xFF,
			0x17, 0xF0, 0xEE, 0xEF, 0xAC, 0xC2, 0x1C, 0x07,
			0x00, 0xE1, 0xC3, 0xFC, 0xEC, 0xF8, 0x34, 0xFF,
			0x17, 0xF0, 0xEE, 0xEF, 0xAC, 0xC2, 0x1C, 0x07
		};

		BurnLoadRom(prom, 8, 1);
	
		// Pre-calculate colors
		for (int i = 0; i < 0x20; i++) {
			pens[i] = BITSWAP24(prom[i], 7,6,5,7,6,6,7,5,4,3,2,4,3,3,4,2,1,0,1,0,1,1,0,1);
		}	
	}


	ZetInit(1);
	ZetOpen(0);

	ZetMapArea(0x0000, 0xffff, 0, eposRom); // Read entire memory space
	ZetMapArea(0x0000, 0xffff, 2, eposRom); // Fetch entire memory space
	ZetMapArea(0x7800, 0xffff, 1, eposRam); // Allow writing in 0x7800 - 0xffff

	ZetMemEnd();

	ZetSetInHandler(epos_read_port);
	ZetSetOutHandler(epos_write_port);

	ZetClose();

	// set protected bits
	DrvJoy[9] = 0xc0;

	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "suprglob") || 
	     !strncmp(BurnDrvGetTextA(DRV_NAME), "theglob", 7))
		DrvJoy[9] = 0x80;

	// Snd
	pAY8910Buffer[0] = pFMBuffer + nBurnSoundLen * 0;
	pAY8910Buffer[1] = pFMBuffer + nBurnSoundLen * 1;
	pAY8910Buffer[2] = pFMBuffer + nBurnSoundLen * 2;

	AY8910Init(0, 2750000, nBurnSoundRate, NULL, NULL, NULL, NULL);

	DrvDoReset();

	return 0;
}

static int DrvExit()
{
	AY8910Exit(0);
	ZetExit();

	free (pFMBuffer);
	free (eposMem);

	pFMBuffer = NULL;
	eposMem = eposRom = eposRam = NULL;

	return 0;
}

static int DrvDraw()
{
#define color(x) BurnHighCol((pens[palette | x] >> 16), (pens[palette | x] >> 8), pens[palette | x], 0)

	for (int i = 0; i < 0x8000; i++)
	{
		int x = (i % 136) * 2;
		int y = (i / 136);
		if (y > 235) continue; // screen is actually 240 pixels wide
		    y *= 272;

		PutPix(pBurnDraw + (x + y + 0) * nBurnBpp, color(eposRam[i + 0x800]  & 0x0f));
		PutPix(pBurnDraw + (x + y + 1) * nBurnBpp, color(eposRam[i + 0x800] >> 0x04));
	}

	return 0;
}


static int DrvFrame()
{
	if (DrvReset) DrvDoReset();

	int nSoundBufferPos = 0;

	ZetOpen(0);
	ZetRun(2750000 / 60);
	ZetRaiseIrq(0xff);
	ZetClose();

	if (pBurnSoundOut) {
		int nSample;
		int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
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

	if (pBurnDraw) DrvDraw();

	return 0;
}


// Savestates
static int DrvScan(int nAction,int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {						// Return minimum compatible version
		*pnMin = 0x029521;
	}

	if (nAction & ACB_VOLATILE) {		// Scan volatile ram		
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = eposRam;
		ba.nLen	  = 0x08800;
		ba.szName = "All Ram";
		BurnAcb(&ba);

		ZetScan(nAction);			// Scan Z80

		AY8910Scan(nAction, pnMin);		// Scan AY8910

		// Scan critical driver variables
		SCAN_VAR(palette);
	}

	return 0;
}



//----------------------------------------------------------------------------------------------
// Drivers


// Megadon

static struct BurnRomInfo megadonRomDesc[] = {
	{ "2732u10b.bin",   0x1000, 0xaf8fbe80, BRF_ESS | BRF_PRG }, //  0 Z80 code
	{ "2732u09b.bin",   0x1000, 0x097d1e73, BRF_ESS | BRF_PRG }, //  1
	{ "2732u08b.bin",   0x1000, 0x526784da, BRF_ESS | BRF_PRG }, //  2
	{ "2732u07b.bin",   0x1000, 0x5b060910, BRF_ESS | BRF_PRG }, //  3
	{ "2732u06b.bin",   0x1000, 0x8ac8af6d, BRF_ESS | BRF_PRG }, //  4
	{ "2732u05b.bin",   0x1000, 0x052bb603, BRF_ESS | BRF_PRG }, //  5
	{ "2732u04b.bin",   0x1000, 0x9b8b7e92, BRF_ESS | BRF_PRG }, //  6
	{ "2716u11b.bin",   0x0800, 0x599b8b61, BRF_ESS | BRF_PRG }, //  7

	{ "74s288.bin",     0x0020, 0xc779ea99, BRF_GRA },	     //  8 Color PROM
};

STD_ROM_PICK(megadon);
STD_ROM_FN(megadon);

struct BurnDriver BurnDrvMegadon = {
	"megadon", NULL, NULL, "1982",
	"Megadon\0", NULL, "Epos Corporation (Photar Industries License)", "EPOS Tristar",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, megadonRomInfo, megadonRomName, DrvInputInfo, megadonDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	236, 272, 3, 4
};


// Catapult

static struct BurnRomInfo catapultRomDesc[] = {
	{ "co3223.u10",     0x1000, 0x50abcfd2, BRF_ESS | BRF_PRG }, //  0 Z80 code
	{ "co3223.u09",     0x1000, 0xfd5a9a1c, BRF_ESS | BRF_PRG }, //  1
	{ "co3223.u08",     0x1000, 0x4bfc36f3, BRF_ESS | BRF_PRG }, //  2  BADADDR xxxx-xxxxxxx
	{ "co3223.u07",     0x1000, 0x4113bb99, BRF_ESS | BRF_PRG }, //  3
	{ "co3223.u06",     0x1000, 0x966bb9f5, BRF_ESS | BRF_PRG }, //  4
	{ "co3223.u05",     0x1000, 0x65f9fb9a, BRF_ESS | BRF_PRG }, //  5
	{ "co3223.u04",     0x1000, 0x648453bc, BRF_ESS | BRF_PRG }, //  6
	{ "co3223.u11",     0x0800, 0x08fb8c28, BRF_ESS | BRF_PRG }, //  7

	{ "co3223.u66",     0x0020, 0xe7de76a7, BRF_GRA },	     //  8 Color PROM
};

STD_ROM_PICK(catapult);
STD_ROM_FN(catapult);

struct BurnDriverD BurnDrvCatapult = {
	"catapult", NULL, NULL, "1982",
	"Catapult\0", "Bad dump", "Epos Corporation", "EPOS Tristar",
	NULL, NULL, NULL, NULL,
	BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, catapultRomInfo, catapultRomName, DrvInputInfo, igmoDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	236, 272, 6, 9
};


// Super Glob

static struct BurnRomInfo suprglobRomDesc[] = {
	{ "u10",	    0x1000, 0xc0141324, BRF_ESS | BRF_PRG }, //  0 Z80 code
	{ "u9",		    0x1000, 0x58be8128, BRF_ESS | BRF_PRG }, //  1
	{ "u8",		    0x1000, 0x6d088c16, BRF_ESS | BRF_PRG }, //  2
	{ "u7",		    0x1000, 0xb2768203, BRF_ESS | BRF_PRG }, //  3
	{ "u6",		    0x1000, 0x976c8f46, BRF_ESS | BRF_PRG }, //  4
	{ "u5",		    0x1000, 0x340f5290, BRF_ESS | BRF_PRG }, //  5
	{ "u4",		    0x1000, 0x173bd589, BRF_ESS | BRF_PRG }, //  6
	{ "u11",	    0x0800, 0xd45b740d, BRF_ESS | BRF_PRG }, //  7

	{ "82s123.u66",     0x0020, 0xf4f6ddc5, BRF_GRA },	     //  8 Color PROM
};

STD_ROM_PICK(suprglob);
STD_ROM_FN(suprglob);

struct BurnDriver BurnDrvSuprglob = {
	"suprglob", NULL, NULL, "1983",
	"Super Glob\0", NULL, "Epos Corporation", "EPOS Tristar",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, suprglobRomInfo, suprglobRomName, DrvInputInfo, suprglobDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	236, 272, 3, 4
};


// The Glob

static struct BurnRomInfo theglobRomDesc[] = {
	{ "globu10.bin",    0x1000, 0x08fdb495, BRF_ESS | BRF_PRG }, //  0 Z80 code
	{ "globu9.bin",	    0x1000, 0x827cd56c, BRF_ESS | BRF_PRG }, //  1
	{ "globu8.bin",	    0x1000, 0xd1219966, BRF_ESS | BRF_PRG }, //  2
	{ "globu7.bin",	    0x1000, 0xb1649da7, BRF_ESS | BRF_PRG }, //  3
	{ "globu6.bin",	    0x1000, 0xb3457e67, BRF_ESS | BRF_PRG }, //  4
	{ "globu5.bin",	    0x1000, 0x89d582cd, BRF_ESS | BRF_PRG }, //  5
	{ "globu4.bin",	    0x1000, 0x7ee9fdeb, BRF_ESS | BRF_PRG }, //  6
	{ "globu11.bin",    0x0800, 0x9e05dee3, BRF_ESS | BRF_PRG }, //  7

	{ "82s123.u66",	    0x0020, 0xf4f6ddc5, BRF_GRA },	     //  8 Color PROM
};

STD_ROM_PICK(theglob);
STD_ROM_FN(theglob);

struct BurnDriver BurnDrvTheglob = {
	"theglob", "suprglob", NULL, "1983",
	"The Glob\0", NULL, "Epos Corporation", "EPOS Tristar",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, theglobRomInfo, theglobRomName, DrvInputInfo, suprglobDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	236, 272, 3, 4
};


// The Glob (earlier)

static struct BurnRomInfo theglob2RomDesc[] = {
	{ "611293.u10",	    0x1000, 0x870af7ce, BRF_ESS | BRF_PRG }, //  0 Z80 code
	{ "611293.u9",	    0x1000, 0xa3679782, BRF_ESS | BRF_PRG }, //  1
	{ "611293.u8",	    0x1000, 0x67499d1a, BRF_ESS | BRF_PRG }, //  2
	{ "611293.u7",	    0x1000, 0x55e53aac, BRF_ESS | BRF_PRG }, //  3
	{ "611293.u6",	    0x1000, 0xc64ad743, BRF_ESS | BRF_PRG }, //  4
	{ "611293.u5",	    0x1000, 0xf93c3203, BRF_ESS | BRF_PRG }, //  5
	{ "611293.u4",	    0x1000, 0xceea0018, BRF_ESS | BRF_PRG }, //  6
	{ "611293.u11",	    0x0800, 0x6ac83f9b, BRF_ESS | BRF_PRG }, //  7

	{ "82s123.u66",	    0x0020, 0xf4f6ddc5, BRF_GRA },	     //  8 Color PROM
};

STD_ROM_PICK(theglob2);
STD_ROM_FN(theglob2);

struct BurnDriver BurnDrvTheglob2 = {
	"theglob2", "suprglob", NULL, "1983",
	"The Glob (earlier)\0", NULL, "Epos Corporation", "EPOS Tristar",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, theglob2RomInfo, theglob2RomName, DrvInputInfo, suprglobDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	236, 272, 3, 4
};


// The Glob (set 3)

static struct BurnRomInfo theglob3RomDesc[] = {
	{ "theglob3.u10",   0x1000, 0x969cfaf6, BRF_ESS | BRF_PRG }, //  0 Z80 code
	{ "theglob3.u9",    0x1000, 0x8e6c010a, BRF_ESS | BRF_PRG }, //  1
	{ "theglob3.u8",    0x1000, 0x1c1ca5c8, BRF_ESS | BRF_PRG }, //  2
	{ "theglob3.u7",    0x1000, 0xa54b9d22, BRF_ESS | BRF_PRG }, //  3
	{ "theglob3.u6",    0x1000, 0x5a6f82a9, BRF_ESS | BRF_PRG }, //  4
	{ "theglob3.u5",    0x1000, 0x72f935db, BRF_ESS | BRF_PRG }, //  5
	{ "theglob3.u4",    0x1000, 0x81db53ad, BRF_ESS | BRF_PRG }, //  6
	{ "theglob3.u11",   0x0800, 0x0e2e6359, BRF_ESS | BRF_PRG }, //  7

	{ "82s123.u66",	    0x0020, 0xf4f6ddc5, BRF_GRA },	     //  8 Color PROM
};

STD_ROM_PICK(theglob3);
STD_ROM_FN(theglob3);

struct BurnDriver BurnDrvTheglob3 = {
	"theglob3", "suprglob", NULL, "1983",
	"The Glob (set 3)\0", NULL, "Epos Corporation", "EPOS Tristar",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, theglob3RomInfo, theglob3RomName, DrvInputInfo, suprglobDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	236, 272, 3, 4
};


// IGMO

static struct BurnRomInfo igmoRomDesc[] = {
	{ "igmo-u10.732",	0x1000, 0xa9f691a4, BRF_ESS | BRF_PRG }, //  0 Z80 code
	{ "igmo-u9.732",	0x1000, 0x3c133c97, BRF_ESS | BRF_PRG }, //  1
	{ "igmo-u8.732",	0x1000, 0x5692f8d8, BRF_ESS | BRF_PRG }, //  2
	{ "igmo-u7.732",	0x1000, 0x630ae2ed, BRF_ESS | BRF_PRG }, //  3
	{ "igmo-u6.732",	0x1000, 0xd3f20e1d, BRF_ESS | BRF_PRG }, //  4
	{ "igmo-u5.732",	0x1000, 0xe26bb391, BRF_ESS | BRF_PRG }, //  5
	{ "igmo-u4.732",	0x1000, 0x762a4417, BRF_ESS | BRF_PRG }, //  6
	{ "igmo-u11.716",	0x0800, 0x8c675837, BRF_ESS | BRF_PRG }, //  7

	{ "82s123.u66",		0x0020, 0x00000000, BRF_GRA | BRF_NODUMP },	//  8 missing
};

STD_ROM_PICK(igmo);
STD_ROM_FN(igmo);

struct BurnDriver BurnDrvIgmo = {
	"igmo", NULL, NULL, "1984",
	"IGMO\0", "Missing color Prom (Incorrect Color)", "Epos Corporation", "EPOS Tristar",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, igmoRomInfo, igmoRomName, DrvInputInfo, igmoDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	236, 272, 3, 4
};


// The Dealer

static struct BurnRomInfo dealerRomDesc[] = {
	{ "u1.bin",	0x2000, 0xe06f3563, BRF_ESS | BRF_PRG }, //  0 Z80 code
	{ "u2.bin",	0x2000, 0x726bbbd6, BRF_ESS | BRF_PRG }, //  1
	{ "u3.bin",	0x2000, 0xab721455, BRF_ESS | BRF_PRG }, //  2
	{ "u4.bin",	0x2000, 0xddb903e4, BRF_ESS | BRF_PRG }, //  3

	{ "82s123.u66",	0x0020, 0x00000000, BRF_GRA | BRF_NODUMP },   //  4 missing
};

STD_ROM_PICK(dealer);
STD_ROM_FN(dealer);

int dealerInit()
{
	return 1;
}

struct BurnDriverD BurnDrvDealer = {
	"dealer", NULL, NULL, "198?",
	"The Dealer\0", "Bad dump", "Epos Corporation", "EPOS Tristar",
	NULL, NULL, NULL, NULL,
	BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, dealerRomInfo, dealerRomName, DrvInputInfo, igmoDIPInfo,
	dealerInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	236, 272, 3, 4
};


// Revenger

static struct BurnRomInfo revengerRomDesc[] = {
	{ "r06124.u1",	0x2000, 0xfad1a2a5, BRF_ESS | BRF_PRG }, //  0 Z80 code
	{ "r06124.u2",	0x2000, 0xa8e0ee7b, BRF_ESS | BRF_PRG }, //  1
	{ "r06124.u3",	0x2000, 0xcca414a5, BRF_ESS | BRF_PRG }, //  2
	{ "r06124.u4",	0x2000, 0x0b81c303, BRF_ESS | BRF_PRG }, //  3

	{ "82s123.u66",	0x0020, 0x00000000, BRF_GRA | BRF_NODUMP },	      //  4 missing
};

STD_ROM_PICK(revenger);
STD_ROM_FN(revenger);

struct BurnDriverD BurnDrvRevenger = {
	"revenger", NULL, NULL, "1984",
	"Revenger\0", "Bad dump", "Epos Corporation", "EPOS Tristar",
	NULL, NULL, NULL, NULL,
	BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, revengerRomInfo, revengerRomName, DrvInputInfo, igmoDIPInfo,
	dealerInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	236, 272, 3, 4
};
