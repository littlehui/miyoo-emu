#include "toaplan.h"
// Batrider

static unsigned char drvButton[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char drvJoy1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char drvJoy2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char drvInput[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char drvRegion = 0;
static unsigned char drvReset = 0;
static unsigned char bDrawScreen;

static unsigned char nIRQPending;
static bool bVBlank;

static int nData;

static int nCurrentBank;
static int nTextROMStatus;

static void Map68KTextROM(bool bMapTextROM);

static struct BurnInputInfo batriderInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	drvButton + 3,	"p1 coin"},
	{"P1 Start",	BIT_DIGITAL,	drvButton + 5,	"p1 start"},

	{"P1 Up",		BIT_DIGITAL,	drvJoy1 + 0,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	drvJoy1 + 1,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	drvJoy1 + 2,	"p1 left"},
	{"P1 Right",	BIT_DIGITAL,	drvJoy1 + 3,	"p1 right"},
	{"P1 Shoot 1",	BIT_DIGITAL,	drvJoy1 + 4,	"p1 fire 1"},
	{"P1 Shoot 2",	BIT_DIGITAL,	drvJoy1 + 5,	"p1 fire 2"},
	{"P1 Shoot 3",	BIT_DIGITAL,	drvJoy1 + 6,	"p1 fire 3"},

	{"P2 Coin",		BIT_DIGITAL,	drvButton + 4,	"p2 coin"},
	{"P2 Start",	BIT_DIGITAL,	drvButton + 6,	"p2 start"},

	{"P2 Up",		BIT_DIGITAL,	drvJoy2 + 0,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	drvJoy2 + 1,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	drvJoy2 + 2,	"p2 left"},
	{"P2 Right",	BIT_DIGITAL,	drvJoy2 + 3,	"p2 right"},
	{"P2 Shoot 1",	BIT_DIGITAL,	drvJoy2 + 4,	"p2 fire 1"},
	{"P2 Shoot 2",	BIT_DIGITAL,	drvJoy2 + 5,	"p2 fire 2"},
	{"P2 Shoot 3",	BIT_DIGITAL,	drvJoy2 + 6,	"p2 fire 3"},

	{"Reset",		BIT_DIGITAL,	  &drvReset,		"reset"},
	{"Test",	  BIT_DIGITAL,	 drvButton + 2,	"diag"},
	{"Service",	BIT_DIGITAL,	 drvButton + 0,	"service"},
	{"Dip 1",		BIT_DIPSWITCH,	drvInput + 3,	"dip"},
	{"Dip 2",		BIT_DIPSWITCH,	drvInput + 4,	"dip"},
	{"Dip 3",		BIT_DIPSWITCH,	drvInput + 5,	"dip"},
	{"Region",  BIT_DIPSWITCH,	&drvRegion  ,	"dip"},
};

STDINPUTINFO(batrider);


static struct BurnDIPInfo batriderDIPList[] = {
	// Defaults
	{0x15,	0xFF,  0xFF,	0x00,	  NULL},
	{0x16,	0xFF,  0xFF,	0x00,	  NULL},
	{0x17,	0xFF,  0xFF,	0x00,	  NULL},

	// DIP 1
	{0,		0xFE, 0,	2,	  "Test mode"},
	{0x15,	0x01, 0x01,	0x00, "Normal"},
	{0x15,	0x01, 0x01,	0x01, "Test"},
	// Normal
	{0,		0xFE, 0,	2,	  "Starting coin"},
	{0x15,	0x82, 0x02,	0x00, "1 credit"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x82, 0x02,	0x02, "2 credits"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	// Free play
	{0,		0xFE, 0,	2,	  "Stick mode"},
	{0x15,	0x02, 0x02,	0x00, "Normal"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x02, 0x02,	0x02, "Special"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	// Both
	{0,		0xFE, 0,	8,	  "Coin 1"},
	{0x15,	0x01, 0x1C, 0x00, "1 coin 1 credit"},
	{0x15,	0x01, 0x1C, 0x04, "1 coin 2 credits"},
	{0x15,	0x01, 0x1C, 0x08, "1 coin 3 credits"},
	{0x15,	0x01, 0x1C, 0x0C, "1 coin 4 credits"},
	{0x15,	0x01, 0x1C, 0x10, "2 coins 1 credit"},
	{0x15,	0x01, 0x1C, 0x14, "3 coins 1 credit"},
	{0x15,	0x01, 0x1C, 0x18, "4 coins 1 credit"},
	{0x15,	0x01, 0x1C, 0x1C, "Free Play"},
	// 0x1C: Free play settings active
	// Normal
	{0,		0xFE, 0,	7,	  "Coin 2"},
	{0x15,	0x82, 0xE0,	0x00, "1 coin 1 credit"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x82, 0xE0,	0x20, "1 coin 2 credits"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x82, 0xE0,	0x40, "1 coin 3 credits"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x82, 0xE0,	0x60, "1 coin 4 credits"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x82, 0xE0,	0x80, "2 coins 1 credit"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x82, 0xE0,	0xA0, "3 coins 1 credit"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x82, 0xE0,	0xC0, "4 coins 1 credit"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x82, 0xE0,	0xE0, "1 coin 1 credit"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	// Free play
	{0,		0xFE, 0,	2,	  "Hit score"},
	{0x15,	0x02, 0x20, 0x00, "Off"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x02, 0x20, 0x20, "On"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0,		0xFE, 0,	2,	  "Sound effect"},
	{0x15,	0x02, 0x40,	0x00, "Off"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x02, 0x40,	0x40, "On"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0,		0xFE, 0,	2,	  "Music"},
	{0x15,	0x02, 0x80,	0x00, "Off"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x15,	0x02, 0x80,	0x80, "On"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},

	// DIP 2
	{0,		0xFE, 0,	4,	  "Start rank"},
	{0x16,	0x01, 0x03, 0x00, "Normal"},
	{0x16,	0x01, 0x03, 0x01, "Easy"},
	{0x16,	0x01, 0x03, 0x02, "Hard"},
	{0x16,	0x01, 0x03, 0x03, "Very hard"},
	{0,		0xFE, 0,	4,	  "Timer rank"},
	{0x16,	0x01, 0x0C, 0x00, "Normal"},
	{0x16,	0x01, 0x0C, 0x04, "Low"},
	{0x16,	0x01, 0x0C, 0x08, "High"},
	{0x16,	0x01, 0x0C, 0x0C, "Highest"},
	{0,		0xFE, 0,	4,	  "Player counts"},
	{0x16,	0x01, 0x30, 0x00, "3"},
	{0x16,	0x01, 0x30, 0x10, "4"},
	{0x16,	0x01, 0x30, 0x20, "2"},
	{0x16,	0x01, 0x30, 0x30, "1"},
	{0,		0xFE, 0,	4,	  "Extra player"},
  {0x16,	0x01, 0xC0, 0x00, "1500000 each"},
  {0x16,	0x01, 0xC0, 0x40, "1000000 each"},
  {0x16,	0x01, 0xC0, 0x80, "2000000 each"},
  {0x16,	0x01, 0xC0, 0xC0, "No extra player"},

	// DIP 3
	{0,		0xFE, 0,	2,	  "Screen flip"},
	{0x17,	0x01, 0x01, 0x00, "Off"},
	{0x17,	0x01, 0x01, 0x01, "On"},
	{0,		0xFE, 0,	2,	  "Demo sound"},
	{0x17,	0x01, 0x02, 0x00, "On"},
	{0x17,	0x01, 0x02, 0x02, "Off"},
	{0,		0xFE, 0,	2,	  "Stage edit"},
	{0x17,	0x01, 0x04, 0x00, "Disable"},
	{0x17,	0x01, 0x04, 0x04, "Enable"},
	{0,		0xFE, 0,	2,	  "Continue play"},
	{0x17,	0x01, 0x08, 0x00, "Enable"},
	{0x17,	0x01, 0x08, 0x08, "Disable"},
	{0,		0xFE, 0,	2,	  "Invincible"},
	{0x17,	0x01, 0x10, 0x00, "Off"},
	{0x17,	0x01, 0x10, 0x10, "On"},
	// Free play
	{0,		0xFE, 0,	2,	  "Guest Players"},
	{0x17,	0x02, 0x20, 0x00, "Disable"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x17,	0x02, 0x20, 0x20, "Enable"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0,		0xFE, 0,	2,	  "Player select"},
	{0x17,	0x02, 0x40, 0x00, "Disable"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x17,	0x02, 0x40, 0x40, "Enable"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0,		0xFE, 0,	2,	  "Special Course"},
	{0x17,	0x02, 0x80, 0x00, "Disable"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},
	{0x17,	0x02, 0x80, 0x80, "Enable"},
	{0x15,	0x00, 0x1C, 0x1C, NULL},

	// Region
	{0,		0xFD, 0,	26,	  "Region"},
	{0x18,	0x01, 0x1F, 0x00, "Nippon"},
	{0x18,	0x01, 0x1F, 0x01, "U.S.A."},
	{0x18,	0x01, 0x1F, 0x02, "Europe"},
	{0x18,	0x01, 0x1F, 0x03, "Asia"},
	{0x18,	0x01, 0x1F, 0x04, "German"},
	{0x18,	0x01, 0x1F, 0x05, "Austria"},
	{0x18,	0x01, 0x1F, 0x06, "Belgium"},
	{0x18,	0x01, 0x1F, 0x07, "Denmark"},
	{0x18,	0x01, 0x1F, 0x08, "Finland"},
	{0x18,	0x01, 0x1F, 0x09, "France"},
	{0x18,	0x01, 0x1F, 0x0A, "Great Britain"},
	{0x18,	0x01, 0x1F, 0x0B, "Greece"},
	{0x18,	0x01, 0x1F, 0x0C, "Holland"},
	{0x18,	0x01, 0x1F, 0x0D, "Italy"},
	{0x18,	0x01, 0x1F, 0x0E, "Norway"},
	{0x18,	0x01, 0x1F, 0x0F, "Portugal"},
	{0x18,	0x01, 0x1F, 0x10, "Spain"},
	{0x18,	0x01, 0x1F, 0x11, "Sweden"},
	{0x18,	0x01, 0x1F, 0x12, "Switzerland"},
	{0x18,	0x01, 0x1F, 0x13, "Australia"},
	{0x18,	0x01, 0x1F, 0x14, "New Zealand"},
	{0x18,	0x01, 0x1F, 0x15, "Taiwan"},
	{0x18,	0x01, 0x1F, 0x16, "HongKong"},
	{0x18,	0x01, 0x1F, 0x17, "Korea"},
	{0x18,	0x01, 0x1F, 0x18, "China"},
	{0x18,	0x01, 0x1F, 0x19, "World"},
};

static struct BurnDIPInfo batriderRegionDIPList[] = {
	// Defaults
	{0x18,	0xFF,  0xFF,	0x00,   NULL},
};

static struct BurnDIPInfo batridrkRegionDIPList[] = {
	// Defaults
	{0x18,	0xFF,  0xFF,	0x17,   NULL},
};

STDDIPINFOEXT(batrider, batriderRegion, batrider);
STDDIPINFOEXT(batridrk, batridrkRegion, batrider);

static unsigned char *Mem = NULL, *MemEnd = NULL;
static unsigned char *RamStart, *RamEnd;
static unsigned char *Rom01;
static unsigned char *Ram01, *Ram02, *RamPal;
unsigned char *RamShared;
static int nColCount = 0x0800;

static int MemIndex()
{
	unsigned char *Next; Next = Mem;
	Rom01		= Next; Next += 0x200000;			// 68000 ROM
	RomZ80		= Next; Next += 0x040000;			// Z80 ROM
	GP9001ROM[0]= Next; Next += nGP9001ROMSize[0];	// Tile data
	MSM6295ROM	= Next; Next += 0x200000;			// MSM6295 ADPCM data
	RamStart	= Next;
	ExtraTRAM	= Next; Next += 0x002000;			// Extra text layer
	Ram01		= Next; Next += 0x005000;			// RAM + Extra text layer scroll/offset
	ExtraTROM	= Next; Next += 0x008000;			// Extra text layer tile data
	Ram02		= Next; Next += 0x008000;			// CPU #0 work RAM
	RamPal		= Next; Next += 0x001000;			// Palette
	RamZ80		= Next; Next += 0x004000;			// Z80 RAM
	GP9001RAM[0]= Next; Next += 0x004000;
	GP9001Reg[0]= (unsigned short*)Next; Next += 0x0100 * sizeof(short);
	RamShared	= Next; Next += 0x000008;			// Shared data
	RamEnd		= Next;
	ToaPalette	= (unsigned int*)Next; Next += nColCount * sizeof(unsigned int);
	MemEnd		= Next;

 	ExtraTSelect= Ram01;							// Extra text layer scroll
	ExtraTScroll= Ram01 + 0x000200;					// Extra text layer offset

	return 0;
}

static void drvZ80Bankswitch(int nBank)
{
	nBank &= 0x0F;
	if (nBank != nCurrentBank) {
		unsigned char* nStartAddress = RomZ80 + (nBank << 14);
		ZetMapArea(0x8000, 0xBFFF, 0, nStartAddress);
		ZetMapArea(0x8000, 0xBFFF, 2, nStartAddress);

		nCurrentBank = nBank;
	}
}

// Scan ram
static int drvScan(int nAction, int* pnMin)
{
	struct BurnArea ba;

	if (pnMin) {						// Return minimum compatible version
		*pnMin = 0x029496;
	}
	if (nAction & ACB_VOLATILE) {		// Scan volatile ram

		memset(&ba, 0, sizeof(ba));
    ba.Data		= RamStart;
		ba.nLen		= RamEnd - RamStart;
		ba.szName	= "RAM";
		BurnAcb(&ba);

		SekScan(nAction);				// Scan 68000
		ZetScan(nAction);				// Scan Z80
		SCAN_VAR(nCurrentBank);

		MSM6295Scan(0, nAction);
		MSM6295Scan(1, nAction);
		BurnYM2151Scan(nAction);

		ToaScanGP9001(nAction, pnMin);

		SCAN_VAR(nIRQPending);
		SCAN_VAR(nTextROMStatus);

		SCAN_VAR(drvInput);

		if (nAction & ACB_WRITE) {
			int n = nTextROMStatus;
			nTextROMStatus = -1;
			Map68KTextROM(n);

			n = nCurrentBank;
			nCurrentBank = -1;
			drvZ80Bankswitch(n);
		}
	}

	return 0;
}

static int LoadRoms()
{
	// Load 68000 ROMs
	if (ToaLoadCode(Rom01, 0, 4)) {
		return 1;
	}

	// Load GP9001 tile data
	ToaLoadGP9001Tiles(GP9001ROM[0], 4, 4, nGP9001ROMSize[0]);

	// Load the Z80 ROM
	if (BurnLoadRom(RomZ80, 8, 1)) {
		return 1;
	}

	// Load MSM6295 ADPCM data
	BurnLoadRom(MSM6295ROM + 0x000000, 9, 1);
	BurnLoadRom(MSM6295ROM + 0x100000, 10, 1);

	return 0;
}

unsigned char __fastcall batriderZIn(unsigned short nAddress)
{
	nAddress &= 0xFF;

	switch (nAddress) {

		// The sound commands sent by the 68000 are read from these locations
		case 0x48:
			return RamShared[0];
		case 0x4A:
			return RamShared[1];

		case 0x81:
			return BurnYM2151ReadStatus();
		case 0x82:
			return MSM6295ReadStatus(0);
		case 0x84:
			return MSM6295ReadStatus(1);
	}

	return 0;
}

void __fastcall batriderZOut(unsigned short nAddress, unsigned char nValue)
{
	nAddress &= 0xFF;

	switch (nAddress) {

		// The 68000 expects to read back the sound command it sent
		case 0x40:
			RamShared[4] = nValue;
			break;
		case 0x42:
			RamShared[5] = nValue;
			break;

		case 0x44:								// ???
		case 0x46:								// Acknowledge interrupt
			break;

		case 0x80:
			BurnYM2151SelectRegister(nValue);
			break;
		case 0x81:
			BurnYM2151WriteRegister(nValue);
			break;

		case 0x82:
			MSM6295Command(0, nValue);
			break;
		case 0x84:
			MSM6295Command(1, nValue);
			break;

		case 0x88: {
			drvZ80Bankswitch(nValue);
			break;

		case 0xC0:
			MSM6295SampleInfo[0][0] = MSM6295ROM + ((nValue & 0x0F) << 16);
			MSM6295SampleData[0][0] = MSM6295ROM + ((nValue & 0x0F) << 16);
			MSM6295SampleInfo[0][1] = MSM6295ROM + ((nValue & 0xF0) << 12) + 0x0100;
			MSM6295SampleData[0][1] = MSM6295ROM + ((nValue & 0xF0) << 12);
			break;
		case 0xC2:
			MSM6295SampleInfo[0][2] = MSM6295ROM + ((nValue & 0x0F) << 16) + 0x0200;
			MSM6295SampleData[0][2] = MSM6295ROM + ((nValue & 0x0F) << 16);
			MSM6295SampleInfo[0][3] = MSM6295ROM + ((nValue & 0xF0) << 12) + 0x0300;
			MSM6295SampleData[0][3] = MSM6295ROM + ((nValue & 0xF0) << 12);
			break;
		case 0xC4:
			MSM6295SampleInfo[1][0] = MSM6295ROM + 0x0100000 + ((nValue & 0x0F) << 16);
			MSM6295SampleData[1][0] = MSM6295ROM + 0x0100000 + ((nValue & 0x0F) << 16);
			MSM6295SampleInfo[1][1] = MSM6295ROM + 0x0100000 + ((nValue & 0xF0) << 12) + 0x0100;
			MSM6295SampleData[1][1] = MSM6295ROM + 0x0100000 + ((nValue & 0xF0) << 12);
			break;
		case 0xC6:
			MSM6295SampleInfo[1][2] = MSM6295ROM + 0x0100000 + ((nValue & 0x0F) << 16) + 0x0200;
			MSM6295SampleData[1][2] = MSM6295ROM + 0x0100000 + ((nValue & 0x0F) << 16);
			MSM6295SampleInfo[1][3] = MSM6295ROM + 0x0100000 + ((nValue & 0xF0) << 12) + 0x0300;
			MSM6295SampleData[1][3] = MSM6295ROM + 0x0100000 + ((nValue & 0xF0) << 12);
			break;
		}
	}
}

static int drvZInit()
{
	// Init the Z80
	ZetInit(1);

	ZetSetInHandler(batriderZIn);
	ZetSetOutHandler(batriderZOut);

	// ROM bank 1
	ZetMapArea(0x0000, 0x7FFF, 0, RomZ80 + 0x0000); // Direct Read from ROM
	ZetMapArea(0x0000, 0x7FFF, 2, RomZ80 + 0x0000); // Direct Fetch from ROM
	// ROM bank 2
	ZetMapArea(0x8000, 0xBFFF, 0, RomZ80 + 0x8000); // Direct Read from ROM
	ZetMapArea(0x8000, 0xBFFF, 2, RomZ80 + 0x8000); //
	// RAM
	ZetMapArea(0xC000, 0xDFFF, 0, RamZ80);			// Direct Read from RAM
	ZetMapArea(0xC000, 0xDFFF, 1, RamZ80);			// Direct Write to RAM
	ZetMapArea(0xC000, 0xDFFF, 2, RamZ80);			//

	ZetMemEnd();

	nCurrentBank = 2;

	return 0;
}

unsigned char __fastcall batriderReadByte(unsigned int sekAddress)
{
	switch (sekAddress) {

		case 0x500000:								// Player 2 inputs
			return drvInput[1];
		case 0x500001:								// Player 1 inputs
			return drvInput[0];
		case 0x500002:								// Dipswitch 3
			return drvInput[5];
		case 0x500003:								// Other inputs
			return drvInput[2];
        case 0x500004:								// Dipswitch 2
			return drvInput[4];
		case 0x500005:								// Dipswitch 1
			return drvInput[3];

		default: {
//			printf("Attempt to read %06X (byte).\n", sekAddress);

			if ((sekAddress & 0x00F80000) == 0x00300000) {
				return RomZ80[(sekAddress & 0x7FFFF) >> 1];
			}
		}
	}

	return 0;
}

unsigned short __fastcall batriderReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x500006:
			return ToaScanlineRegister();

		// These addresses contain the response of the Z80 to the sound commands
		case 0x500008:
			return RamShared[4];
		case 0x50000A:
			return RamShared[5];

		case 0x50000C:
			// This address echos the value written to 0x500060
			return nData;

		default: {
//			printf("Attempt to read %06X (word).\n", sekAddress);
		}
	}

	return 0;
}

void __fastcall batriderWriteByte(unsigned int sekAddress, unsigned char)	// unsigned char byteValue
{
	switch (sekAddress) {

		case 0x500011:								// Coin control
			break;

//		default:
//			printf("Attempt to write %06X (byte) -> %04X.\n", sekAddress, byteValue);
	}
}

void __fastcall batriderWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {
		case 0x500020: {
			RamShared[0] = wordValue;

			// The 68K program normally writes 0x500020/0x500022 as a single longword,
			// except during the communications test.
			if (wordValue == 0x55) {
				ZetNmi();
				nCyclesDone[1] += ZetRun(0x1800);
			}
			break;
		}
		case 0x500022:
			RamShared[1] = wordValue;

			// Sound commands are processed by the Z80 using an NMI
			// So, trigger a Z80 NMI and execute it
			ZetNmi();
			nCyclesDone[1] += ZetRun(0x1800);
			break;

		case 0x500024:
			// Writes to this address only occur in situations where the program sets
			// 0x20FA19 (Ram02[0x7A18]) to 0xFF, and then sits in a loop waiting for it to become 0x00
			// Interrupt 4 does this (the same code is also conditionally called from interrupt 2)

			nIRQPending = 1;
			SekSetIRQLine(4, SEK_IRQSTATUS_ACK);
			break;

		case 0x500060:
			// Bit 0 of the value written to this location must be echod at 0x50000C
			nData = wordValue;
			break;

		case 0x500080:
			Map68KTextROM(false);
			break;

		case 0x500082:								// Acknowledge interrupt
			SekSetIRQLine(0, SEK_IRQSTATUS_NONE);
			nIRQPending = 0;
			break;

		case 0x5000C0:
		case 0x5000C1:
		case 0x5000C2:
		case 0x5000C3:
		case 0x5000C4:
		case 0x5000C5:
		case 0x5000C6:
		case 0x5000C7:
		case 0x5000C8:
		case 0x5000C9:
		case 0x5000CA:
		case 0x5000CB:
		case 0x5000CC:
		case 0x5000CD:
		case 0x5000CE:
			GP9001TileBank[(sekAddress & 0x0F) >> 1] = ((wordValue & 0x0F) << 15);
			break;

//		default:
//			printf("Attempt to write %06X (word) -> %04X.\n", sekAddress, wordValue);
	}
}

unsigned short __fastcall batriderReadWordGP9001(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x400008:
			return ToaGP9001ReadRAM_Hi(0);
		case 0x40000A:
			return ToaGP9001ReadRAM_Lo(0);

	}

	return 0;
}

void __fastcall batriderWriteWordGP9001(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {

		case 0x400000:
			ToaGP9001WriteRegister(wordValue);
			break;

		case 0x400004:
			ToaGP9001SelectRegister(wordValue);
			break;

		case 0x400008:
		case 0x40000A:
			ToaGP9001WriteRAM(wordValue, 0);
			break;

		case 0x40000C:								// Set GP9001 VRAM address-pointer
			ToaGP9001SetRAMPointer(wordValue);
			break;
	}
}

unsigned char __fastcall batriderReadByteZ80ROM(unsigned int sekAddress)
{
	return RomZ80[(sekAddress & 0x7FFFF) >> 1];
}

unsigned short __fastcall batriderReadWordZ80ROM(unsigned int sekAddress)
{
	return RomZ80[(sekAddress & 0x7FFFF) >> 1];
}

static void Map68KTextROM(bool bMapTextROM)
{
	if (bMapTextROM) {
		if (nTextROMStatus != 1) {
			SekMapMemory(ExtraTROM,	0x200000, 0x207FFF, SM_RAM);	// Extra text tile memory

			nTextROMStatus = 1;
		}
	} else {
		if (nTextROMStatus != 0) {
			SekMapMemory(ExtraTRAM,	0x200000, 0x201FFF, SM_RAM);	// Extra text tilemap RAM
			SekMapMemory(RamPal,	0x202000, 0x202FFF, SM_RAM);	// Palette RAM
			SekMapMemory(Ram01,		0x203000, 0x207FFF, SM_RAM);	// Extra text Scroll & offset; RAM

			nTextROMStatus = 0;
		}
	}
}

static int drvDoReset()
{
	// Insert region code into 68K ROM, code by BisonSAS
	unsigned char nRegion = drvRegion & 0x1F;
  if (nRegion<=25) {
  	Rom01[0x00000^1]=(unsigned char)(nRegion<<13) | (drvRegion & 0x1F);
  }

	SekOpen(0);

	nIRQPending = 0;
  SekSetIRQLine(0, SEK_IRQSTATUS_NONE);

	Map68KTextROM(true);

	SekReset();
	SekClose();

	ZetReset();

	MSM6295Reset(0);
	MSM6295Reset(1);
	BurnYM2151Reset();

	return 0;
}

static int drvInit()
{
	int nLen;

#ifdef DRIVER_ROTATION
	bToaRotateScreen = true;
#endif

	nGP9001ROMSize[0] = 0x01000000;

	// Find out how much memory is needed
	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (unsigned char*)0;
	if ((Mem = (unsigned char*)malloc(nLen)) == NULL) {
		return 1;
	}
	memset(Mem, 0, nLen);										// Zero memory
	MemIndex();													// Index the allocated memory

	if (LoadRoms()) {
		return 1;
	}

	{
		SekInit(0, 0x68000);									// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom01, 0x000000, 0x1FFFFF, SM_ROM);		// CPU 0 ROM
		SekMapMemory(Ram02, 0x208000, 0x20FFFF, SM_RAM);

		Map68KTextROM(true);

		SekSetReadWordHandler(0, batriderReadWord);
		SekSetReadByteHandler(0, batriderReadByte);
		SekSetWriteWordHandler(0, batriderWriteWord);
		SekSetWriteByteHandler(0, batriderWriteByte);

		SekMapHandler(1,	0x400000, 0x400400, SM_RAM);		// GP9001 addresses

		SekSetReadWordHandler(1, batriderReadWordGP9001);
		SekSetWriteWordHandler(1, batriderWriteWordGP9001);

		SekMapHandler(2,	0x300000, 0x37FFFF, SM_ROM);		// Z80 ROM

		SekSetReadByteHandler(2, batriderReadByteZ80ROM);
		SekSetReadWordHandler(2, batriderReadWordZ80ROM);

		SekClose();
	}

	nSpriteYOffset = 0x0001;

	nLayer0XOffset = -0x01D6;
	nLayer1XOffset = -0x01D8;
	nLayer2XOffset = -0x01DA;

	ToaInitGP9001();

	nExtraTXOffset = 0x2C;
	ToaExtraTextInit();

	drvZInit();				// Initialize Z80

	BurnYM2151Init(32000000 / 8, 50.0);
	MSM6295Init(0, 32000000 / 10 / 132, 50.0, 1);
	MSM6295Init(1, 32000000 / 10 / 165, 50.0, 1);

	nToaPalLen = nColCount;
	ToaPalSrc = RamPal;
	ToaPalInit();

	nTextROMStatus = -1;
	bDrawScreen = true;

#if defined FBA_DEBUG && defined USE_SPEEDHACKS
	bprintf(PRINT_IMPORTANT, _T("  * Using speed-hacks (detecting idle loops).\n"));
#endif

	drvDoReset(); // Reset machine
	return 0;
}

static int drvExit()
{
	MSM6295Exit(1);
	MSM6295Exit(0);
	BurnYM2151Exit();

	ToaPalExit();
	ToaExitGP9001();
	ToaExtraTextExit();
	ToaZExit();				// Z80 exit
	SekExit();				// Deallocate 68000

	free(Mem);
	Mem = NULL;

	return 0;
}

inline static int CheckSleep(int)
{
#if 1 && defined USE_SPEEDHACKS
	int nCurrentPC = SekGetPC(-1);

	if (!nIRQPending &&
		((nCurrentPC >= 0x0009F4 && nCurrentPC <= 0x0009FA) ||
		 (nCurrentPC >= 0x001FF6 && nCurrentPC <= 0x001FFC) ||
		 (nCurrentPC >= 0x003C7C && nCurrentPC <= 0x003C82)))
	{
		return 1;
	}
#endif

	return 0;
}

static int drvDraw()
{
	ToaClearScreen();

	if (bDrawScreen) {
		ToaGetBitmap();
		ToaRenderGP9001();					// Render GP9001 graphics
		ToaExtraTextLayer();				// Render extra text layer
	}

	ToaPalUpdate();							// Update the palette

	return 0;
}

static int drvFrame()
{
	int nInterleave = 8;

	if (drvReset) {														// Reset machine
		drvDoReset();
	}

	// Compile digital inputs
	drvInput[0] = 0x00;													// Buttons
	drvInput[1] = 0x00;													// Player 1
	drvInput[2] = 0x00;													// Player 2
	for (int i = 0; i < 8; i++) {
		drvInput[0] |= (drvJoy1[i] & 1) << i;
		drvInput[1] |= (drvJoy2[i] & 1) << i;
		drvInput[2] |= (drvButton[i] & 1) << i;
	}
	ToaClearOpposites(&drvInput[0]);
	ToaClearOpposites(&drvInput[1]);

	SekNewFrame();

	nCyclesTotal[0] = (int)((long long)16000000 * nBurnCPUSpeedAdjust / (0x0100 * 60));
	nCyclesTotal[1] = TOA_Z80_SPEED / 60;
	nCyclesDone[0] = nCyclesDone[1] = 0;

	SekSetCyclesScanline(nCyclesTotal[0] / 262);
	nToaCyclesDisplayStart = nCyclesTotal[0] - ((nCyclesTotal[0] * (TOA_VBLANK_LINES + 240)) / 262);
	nToaCyclesVBlankStart = nCyclesTotal[0] - ((nCyclesTotal[0] * TOA_VBLANK_LINES) / 262);
	bVBlank = false;

	int nSoundBufferPos = 0;

	SekOpen(0);
	for (int i = 1; i <= nInterleave; i++) {
    	int nCurrentCPU;
		int nNext;

		// Run 68000

		nCurrentCPU = 0;
		nNext = i * nCyclesTotal[nCurrentCPU] / nInterleave;

		// Trigger VBlank interrupt
		if (!bVBlank && nNext > nToaCyclesVBlankStart) {
			if (nCyclesDone[nCurrentCPU] < nToaCyclesVBlankStart) {
				nCyclesSegment = nToaCyclesVBlankStart - nCyclesDone[nCurrentCPU];
				if (!CheckSleep(nCurrentCPU)) {
					nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
				} else {
					nCyclesDone[nCurrentCPU] += SekIdle(nCyclesSegment);
				}
			}

			ToaBufferGP9001Sprites();
			if (pBurnDraw) {											// Draw screen if needed
				drvDraw();
			}

			nIRQPending = 1;
			SekSetIRQLine(2, SEK_IRQSTATUS_ACK);

			bVBlank = true;
		}

		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		if (!CheckSleep(nCurrentCPU)) {									// See if this CPU is busywaiting
			nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
		} else {
			nCyclesDone[nCurrentCPU] += SekIdle(nCyclesSegment);
		}

		if ((i & 1) == 0) {
			// Run Z80
			nCurrentCPU = 1;
			nNext = i * nCyclesTotal[nCurrentCPU] / nInterleave;
			nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
			nCyclesDone[nCurrentCPU] += ZetRun(nCyclesSegment);

			// Render sound segment
			if (pBurnSoundOut) {
				int nSegmentLength = (nBurnSoundLen * i / nInterleave) - nSoundBufferPos;
				short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
				BurnYM2151Render(pSoundBuf, nSegmentLength);
				MSM6295Render(0, pSoundBuf, nSegmentLength);
				MSM6295Render(1, pSoundBuf, nSegmentLength);
				nSoundBufferPos += nSegmentLength;
			}
		}
	}

	SekClose();

	{
		// Make sure the buffer is entirely filled.
		if (pBurnSoundOut) {
			int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			if (nSegmentLength) {
				BurnYM2151Render(pSoundBuf, nSegmentLength);
				MSM6295Render(0, pSoundBuf, nSegmentLength);
				MSM6295Render(1, pSoundBuf, nSegmentLength);
			}
		}
	}

	return 0;
}

// Rom information
static struct BurnRomInfo batriderRomDesc[] = {
	{ "prg0b.u22",    0x080000, 0x4F3FC729, BRF_ESS | BRF_PRG }, //  0 CPU #0 code	(even)
	{ "prg2.u21",     0x080000, 0xBDAA5FBF, BRF_ESS | BRF_PRG }, //  1
	{ "prg1b.u23",    0x080000, 0x8E70B492, BRF_ESS | BRF_PRG }, //  2				(odd)
	{ "prg3.u24",     0x080000, 0x7AA9F941, BRF_ESS | BRF_PRG }, //  3

	{ "rom-1.bin",    0x400000, 0x0DF69CA2, BRF_GRA },			 //  4 GP9001 Tile data
	{ "rom-3.bin",    0x400000, 0x60167D38, BRF_GRA },			 //  5
	{ "rom-2.bin",    0x400000, 0x1BFEA593, BRF_GRA },			 //  6
	{ "rom-4.bin",    0x400000, 0xBEE03c94, BRF_GRA },			 //  7

	{ "snd.u77",      0x040000, 0x56682696, BRF_ESS | BRF_PRG }, //  8 Z80 program

	{ "rom-5.bin",    0x100000, 0x4274DAf6, BRF_SND },			 //  9 MSM6295 #1 ADPCM data
	{ "rom-6.bin",    0x100000, 0x2A1C2426, BRF_SND },			 // 10 MSM6295 #2 ADPCM data
};


STD_ROM_PICK(batrider);
STD_ROM_FN(batrider);

static struct BurnRomInfo batridraRomDesc[] = {
	{ "prg0.bin",     0x080000, 0xF93EA27C, BRF_ESS | BRF_PRG }, //  0 CPU #0 code	(even)
	{ "prg2.u21",     0x080000, 0xBDAA5FBF, BRF_ESS | BRF_PRG }, //  1
	{ "prg1.bin",     0x080000, 0x8AE7F592, BRF_ESS | BRF_PRG }, //  2				(odd)
	{ "prg3.u24",     0x080000, 0x7AA9F941, BRF_ESS | BRF_PRG }, //  3

	{ "rom-1.bin",    0x400000, 0x0DF69CA2, BRF_GRA },			 //  4 GP9001 Tile data
	{ "rom-3.bin",    0x400000, 0x60167D38, BRF_GRA },			 //  5
	{ "rom-2.bin",    0x400000, 0x1BFEA593, BRF_GRA },			 //  6
	{ "rom-4.bin",    0x400000, 0xBEE03c94, BRF_GRA },			 //  7

	{ "snd.u77",      0x040000, 0x56682696, BRF_ESS | BRF_PRG }, //  8 Z80 program

	{ "rom-5.bin",    0x100000, 0x4274DAf6, BRF_SND },			 //  9 MSM6295 #1 ADPCM data
	{ "rom-6.bin",    0x100000, 0x2A1C2426, BRF_SND },			 // 10 MSM6295 #2 ADPCM data
};


STD_ROM_PICK(batridra);
STD_ROM_FN(batridra);

static struct BurnRomInfo batridrkRomDesc[] = {
	{ "prg0.u22",     0x080000, 0xD9D8C907, BRF_ESS | BRF_PRG }, //  0 CPU #0 code	(even)
	{ "prg2.u21",     0x080000, 0xBDAA5FBF, BRF_ESS | BRF_PRG }, //  1
	{ "prg1.u23",     0x080000, 0x8E70B492, BRF_ESS | BRF_PRG }, //  2				(odd)
	{ "prg3.u24",     0x080000, 0x7AA9F941, BRF_ESS | BRF_PRG }, //  3

	{ "rom-1.bin",    0x400000, 0x0DF69CA2, BRF_GRA },			 //  4 GP9001 Tile data
	{ "rom-3.bin",    0x400000, 0x60167D38, BRF_GRA },			 //  5
	{ "rom-2.bin",    0x400000, 0x1BFEA593, BRF_GRA },			 //  6
	{ "rom-4.bin",    0x400000, 0xBEE03c94, BRF_GRA },			 //  7

	{ "snd.u77",      0x040000, 0x56682696, BRF_ESS | BRF_PRG }, //  8 Z80 program

	{ "rom-5.bin",    0x100000, 0x4274DAf6, BRF_SND },			 //  9 MSM6295 #1 ADPCM data
	{ "rom-6.bin",    0x100000, 0x2A1C2426, BRF_SND },			 // 10 MSM6295 #2 ADPCM data
};


STD_ROM_PICK(batridrk);
STD_ROM_FN(batridrk);

struct BurnDriver BurnDrvBatrider = {
	"batrider", NULL, NULL, "1998",
	"Armed police Batrider (Batrider ver. Fri Feb 13 1998, Japan)\0", NULL, "Raizing / 8ing", "Toaplan GP9001 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | TOA_ROTATE_GRAPHICS_CCW, 2, HARDWARE_TOAPLAN_RAIZING,
	NULL, batriderRomInfo, batriderRomName, batriderInputInfo, batriderDIPInfo,
	drvInit, drvExit, drvFrame, drvDraw, drvScan, 0, NULL, NULL, NULL, &ToaRecalcPalette,
	240, 320, 3, 4
};

struct BurnDriver BurnDrvBatridra = {
	"batridra", "batrider", NULL, "1998",
	"Armed police Batrider (Batrider ver. Mon Dec 22 1997, Japan)\0", NULL, "Raizing / 8ing", "Toaplan GP9001 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | TOA_ROTATE_GRAPHICS_CCW, 2, HARDWARE_TOAPLAN_RAIZING,
	NULL, batridraRomInfo, batridraRomName, batriderInputInfo, batriderDIPInfo,
	drvInit, drvExit, drvFrame, drvDraw, drvScan, 0, NULL, NULL, NULL, &ToaRecalcPalette,
	240, 320, 3, 4
};

struct BurnDriver BurnDrvBatridrk = {
	"batridrk", "batrider", NULL, "1998",
	"Armed police Batrider (Batrider ver. Fri Feb 13 1998, Korea)\0", NULL, "Raizing / 8ing", "Toaplan GP9001 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | TOA_ROTATE_GRAPHICS_CCW, 2, HARDWARE_TOAPLAN_RAIZING,
	NULL, batridrkRomInfo, batridrkRomName, batriderInputInfo, batridrkDIPInfo,
	drvInit, drvExit, drvFrame, drvDraw, drvScan, 0, NULL, NULL, NULL, &ToaRecalcPalette,
	240, 320, 3, 4
};

