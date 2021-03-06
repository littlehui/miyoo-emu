// DoDonpachi
#include "cave.h"
#include "ymz280b.h"

#define CAVE_VBLANK_LINES 12

static unsigned char DrvJoy1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned short DrvInput[2] = {0x0000, 0x0000};

static unsigned char *Mem = NULL, *MemEnd = NULL;
static unsigned char *RamStart, *RamEnd;
static unsigned char *Rom01;
static unsigned char *Ram01;

static unsigned char DrvReset = 0;
static unsigned char bDrawScreen;
static bool bVBlank;

static int nSpeedhack;

static char nVideoIRQ;
static char nSoundIRQ;
static char nUnknownIRQ;

static char nIRQPending;

static int nCurrentCPU;
static int nCyclesDone[2];
static int nCyclesTotal[2];
static int nCyclesSegment;

static struct BurnInputInfo ddonpachInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 8,	"p1 coin"},
	{"P1 Start",	BIT_DIGITAL,	DrvJoy1 + 7,	"p1 start"},

	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0, 	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1, 	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2, 	"p1 left"},
	{"P1 Right",	BIT_DIGITAL,	DrvJoy1 + 3, 	"p1 right"},
	{"P1 Button 1",	BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"},
	{"P1 Button 3",	BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 3"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy2 + 8,	"p2 coin"},
	{"P2 Start",	BIT_DIGITAL,	DrvJoy2 + 7,	"p2 start"},

	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 0, 	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 1, 	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 2, 	"p2 left"},
	{"P2 Right",	BIT_DIGITAL,	DrvJoy2 + 3, 	"p2 right"},
	{"P2 Button 1",	BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2",	BIT_DIGITAL,	DrvJoy2 + 5,	"p2 fire 2"},
	{"P2 Button 3",	BIT_DIGITAL,	DrvJoy2 + 6,	"p2 fire 3"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,		"reset"},
	{"Diagnostics",	BIT_DIGITAL,	DrvJoy1 + 9,	"diag"},
	{"Service",		BIT_DIGITAL,	DrvJoy2 + 9,	"service"},
};

STDINPUTINFO(ddonpach);

static void UpdateIRQStatus()
{
	nIRQPending = (nVideoIRQ == 0 || nSoundIRQ == 0 || nUnknownIRQ == 0);
	SekSetIRQLine(1, nIRQPending ? SEK_IRQSTATUS_ACK : SEK_IRQSTATUS_NONE);
}

unsigned char __fastcall ddonpachReadByte(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x300002:
		case 0x300003: {
			return YMZ280BReadStatus();
		}

		case 0x800000:
		case 0x800001: {
			unsigned char nRet = 6 | nVideoIRQ;
			nVideoIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}
		case 0x800002:
		case 0x800003:
		case 0x800004:
		case 0x800005:
		case 0x800006:
		case 0x800007: {
			unsigned char nRet = 6 | nVideoIRQ;
			return nRet;
		}

		case 0xD00000:
			return (DrvInput[0] >> 8) ^ 0xFF;
		case 0xD00001:
			return (DrvInput[0] & 0xFF) ^ 0xFF;
		case 0xD00002:
			return (DrvInput[1] >> 8) ^ 0xF7 | (EEPROMRead() << 3);
		case 0xD00003:
			return (DrvInput[1] & 0xFF) ^ 0xFF;

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to read byte value of location %x\n", sekAddress);
		}
	}
	return 0;
}

unsigned short __fastcall ddonpachReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x300002: {
			return YMZ280BReadStatus();
		}

		case 0x800000: {
			unsigned short nRet = 6 | nVideoIRQ;
			nVideoIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}

		case 0x800002:
		case 0x800004:
		case 0x800006: {
			unsigned short nRet = 6 | nVideoIRQ;
			return nRet;
		}

		case 0xD00000:
			return DrvInput[0] ^ 0xFFFF;
		case 0xD00002:
			return DrvInput[1] ^ 0xF7FF | (EEPROMRead() << 11);

		default: {
// 			bprintf(PRINT_NORMAL, "Attempt to read word value of location %x\n", sekAddress);
		}
	}
	return 0;
}

void __fastcall ddonpachWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	switch (sekAddress) {
		case 0x300001:
			YMZ280BSelectRegister(byteValue);
			return;
		case 0x300003:
			YMZ280BWriteRegister(byteValue);
			return;

		case 0xE00000:
			EEPROMWrite(byteValue & 0x04, byteValue & 0x02, byteValue & 0x08);
			return;

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to write byte value %x to location %x\n", byteValue, sekAddress);
		}
	}
}

void __fastcall ddonpachWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {
		case 0x300000:
			YMZ280BSelectRegister(wordValue);
			return;
		case 0x300002:
			YMZ280BWriteRegister(wordValue);
			return;

		case 0x800000:
			nCaveXOffset = wordValue;
			return;
		case 0x800002:
			nCaveYOffset = wordValue;
			return;
		case 0x800008:
			CaveSpriteBuffer();
			nCaveSpriteBank = wordValue;
			return;

		case 0x900000:
			CaveTileReg[0][0] = wordValue;
			return;
		case 0x900002:
			CaveTileReg[0][1] = wordValue;
			return;
		case 0x900004:
			CaveTileReg[0][2] = wordValue;
			return;

		case 0xA00000:
			CaveTileReg[1][0] = wordValue;
			return;
		case 0xA00002:
			CaveTileReg[1][1] = wordValue;
			return;
		case 0xA00004:
			CaveTileReg[1][2] = wordValue;
			return;

		case 0xB00000:
			CaveTileReg[2][0] = wordValue;
			return;
		case 0xB00002:
			CaveTileReg[2][1] = wordValue;
			return;
		case 0xB00004:
			CaveTileReg[2][2] = wordValue;
			return;

		case 0xE00000:
			wordValue >>= 8;
			EEPROMWrite(wordValue & 0x04, wordValue & 0x02, wordValue & 0x08);
			return;

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to write word value %x to location %x\n", wordValue, sekAddress);
		}
	}
}

void __fastcall ddonpachWriteBytePalette(unsigned int sekAddress, unsigned char byteValue)
{
	CavePalWriteByte(sekAddress & 0xFFFF, byteValue);
}

void __fastcall ddonpachWriteWordPalette(unsigned int sekAddress, unsigned short wordValue)
{
	CavePalWriteWord(sekAddress & 0xFFFF, wordValue);
}

static void TriggerSoundIRQ(int nStatus)
{
	nSoundIRQ = nStatus ^ 1;
	UpdateIRQStatus();

	if (nIRQPending && nCurrentCPU != 0) {
		nCyclesDone[0] += SekRun(0x0400);
	}
}

static int DrvExit()
{
	YMZ280BExit();

	EEPROMExit();

	CaveTileExit();
	CaveSpriteExit();
    CavePalExit();

	SekExit();				// Deallocate 68000s

	// Deallocate all used memory
	free(Mem);
	Mem = NULL;

	return 0;
}

static int DrvDoReset()
{
	SekOpen(0);
	SekReset();
	SekClose();

	EEPROMReset();

	YMZ280BReset();

	nVideoIRQ = 1;
	nSoundIRQ = 1;
	nUnknownIRQ = 1;

	nIRQPending = 0;

	YMZ280BReset();

	return 0;
}

static int DrvDraw()
{
	CavePalUpdate8Bit(0, 128);				// Update the palette
	CaveClearScreen(CavePalette[0x7F00]);

	if (bDrawScreen) {
//		CaveGetBitmap();

		CaveTileRender(1);					// Render tiles
	}

	return 0;
}

inline static int CheckSleep(int)
{
#if 1 && defined USE_SPEEDHACKS
	int nCurrentPC = SekGetPC(-1) - nSpeedhack;

	if (!nIRQPending && nCurrentPC >= 0 && nCurrentPC <= 12) {
		return 1;
	}
#endif

	return 0;
}

static int DrvFrame()
{
	int nCyclesVBlank;
	int nInterleave = 8;

	if (DrvReset) {														// Reset machine
		DrvDoReset();
	}

//	bprintf(PRINT_NORMAL, "\n");

	// Compile digital inputs
	DrvInput[0] = 0x0000;  												// Player 1
	DrvInput[1] = 0x0000;  												// Player 2
	for (int i = 0; i < 10; i++) {
		DrvInput[0] |= (DrvJoy1[i] & 1) << i;
		DrvInput[1] |= (DrvJoy2[i] & 1) << i;
	}
	CaveClearOpposites(&DrvInput[0]);
	CaveClearOpposites(&DrvInput[1]);

	SekNewFrame();

	nCyclesTotal[0] = (int)((long long)16000000 * nBurnCPUSpeedAdjust / (0x0100 * CAVE_REFRESHRATE));
	nCyclesDone[0] = 0;

	nCyclesVBlank = nCyclesTotal[0] - (int)((nCyclesTotal[0] * CAVE_VBLANK_LINES) / 271.5);
	bVBlank = false;

	int nSoundBufferPos = 0;

	SekOpen(0);

	for (int i = 1; i <= nInterleave; i++) {
		int nNext;

		// Render sound segment
		if ((i & 1) == 0) {
			if (pBurnSoundOut) {
				int nSegmentEnd = nBurnSoundLen * i / nInterleave;
				short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
				YMZ280BRender(pSoundBuf, nSegmentEnd - nSoundBufferPos);
				nSoundBufferPos = nSegmentEnd;
			}
		}

		// Run 68000
    	nCurrentCPU = 0;
		nNext = i * nCyclesTotal[nCurrentCPU] / nInterleave;

		// See if we need to trigger the VBlank interrupt
		if (!bVBlank && nNext > nCyclesVBlank) {
			if (nCyclesDone[nCurrentCPU] < nCyclesVBlank) {
				nCyclesSegment = nCyclesVBlank - nCyclesDone[nCurrentCPU];
				if (!CheckSleep(nCurrentCPU)) {							// See if this CPU is busywaiting
					nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
				} else {
					nCyclesDone[nCurrentCPU] += SekIdle(nCyclesSegment);
				}
			}

			if (pBurnDraw != NULL) {
				DrvDraw();												// Draw screen if needed
			}

			bVBlank = true;
			nVideoIRQ = 0;
			UpdateIRQStatus();
		}

		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		if (!CheckSleep(nCurrentCPU)) {									// See if this CPU is busywaiting
			nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
		} else {
			nCyclesDone[nCurrentCPU] += SekIdle(nCyclesSegment);
		}

		nCurrentCPU = -1;
	}

	// Make sure the buffer is entirely filled.
	{
		if (pBurnSoundOut) {
			int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			if (nSegmentLength) {
				YMZ280BRender(pSoundBuf, nSegmentLength);
			}
		}
	}

	SekClose();

	return 0;
}

// This routine is called first to determine how much memory is needed (MemEnd-(unsigned char *)0),
// and then afterwards to set up all the pointers
static int MemIndex()
{
	unsigned char* Next; Next = Mem;
	Rom01			= Next; Next += 0x100000;		// 68K program
	CaveSpriteROM	= Next; Next += 0x1000000;
	CaveTileROM[0]	= Next; Next += 0x400000;		// Tile layer 0
	CaveTileROM[1]	= Next; Next += 0x400000;		// Tile layer 1
	CaveTileROM[2]	= Next; Next += 0x200000;		// Tile layer 2
	YMZ280BROM		= Next; Next += 0x400000;
	RamStart		= Next;
	Ram01			= Next; Next += 0x010000;		// CPU #0 work RAM
	CaveTileRAM[0]	= Next; Next += 0x008000;
	CaveTileRAM[1]	= Next; Next += 0x008000;
	CaveTileRAM[2]	= Next; Next += 0x008000;
	CaveSpriteRAM	= Next; Next += 0x010000;
	CavePalSrc		= Next; Next += 0x010000;		// palette
	RamEnd			= Next;
	MemEnd			= Next;

	return 0;
}

static void NibbleSwap2(unsigned char* pData, int nLen)
{
	unsigned char* pOrg = pData + nLen - 1;
	unsigned char* pDest = pData + ((nLen - 1) << 1);

	for (int i = 0; i < nLen; i++, pOrg--, pDest -= 2) {
		pDest[1] = *pOrg & 15;
		pDest[0] = *pOrg >> 4;
	}

	return;
}

static int LoadRoms()
{
	// Load 68000 ROM
	BurnLoadRom(Rom01 + 0, 1, 2);
	BurnLoadRom(Rom01 + 1, 0, 2);

	BurnLoadRom(CaveSpriteROM + 0x000000, 2, 1);
	BurnLoadRom(CaveSpriteROM + 0x200000, 3, 1);
	BurnLoadRom(CaveSpriteROM + 0x400000, 4, 1);
	BurnLoadRom(CaveSpriteROM + 0x600000, 5, 1);
	BurnByteswap(CaveSpriteROM, 0x800000);
	NibbleSwap2(CaveSpriteROM, 0x800000);

	BurnLoadRom(CaveTileROM[0], 6, 1);
	NibbleSwap2(CaveTileROM[0], 0x200000);
	BurnLoadRom(CaveTileROM[1], 7, 1);
	NibbleSwap2(CaveTileROM[1], 0x200000);

	unsigned char* pTemp = (unsigned char*)malloc(0x200000);
	BurnLoadRom(pTemp, 8, 1);
	for (int i = 0; i < 0x0100000; i++) {
		CaveTileROM[2][(i << 1) + 1] = (pTemp[(i << 1) + 0] & 15) | ((pTemp[(i << 1) + 1] & 15) << 4);
		CaveTileROM[2][(i << 1) + 0] = (pTemp[(i << 1) + 0] >> 4) | (pTemp[(i << 1) + 1] & 240);
	}
	free(pTemp);

	// Load YMZ280B data
	BurnLoadRom(YMZ280BROM + 0x000000, 9, 1);
	BurnLoadRom(YMZ280BROM + 0x200000, 10, 1);

	return 0;
}

// Scan ram
static int DrvScan(int nAction, int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {						// Return minimum compatible version
		*pnMin = 0x020902;
	}

	EEPROMScan(nAction, pnMin);			// Scan EEPROM

	if (nAction & ACB_VOLATILE) {		// Scan volatile ram

		memset(&ba, 0, sizeof(ba));
    	ba.Data		= RamStart;
		ba.nLen		= RamEnd - RamStart;
		ba.szName	= "RAM";
		BurnAcb(&ba);

		SekScan(nAction);				// scan 68000 states

		YMZ280BScan();

		SCAN_VAR(nVideoIRQ);
		SCAN_VAR(nSoundIRQ);
		SCAN_VAR(nUnknownIRQ);
		SCAN_VAR(bVBlank);

		CaveScanGraphics();

		SCAN_VAR(DrvInput);
	}

	return 0;
}

static int DrvInit()
{
	int nLen;

	BurnSetRefreshRate(CAVE_REFRESHRATE);

	// Find out how much memory is needed
	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) {
		return 1;
	}
	memset(Mem, 0, nLen);										// blank all memory
	MemIndex();													// Index the allocated memory

	EEPROMInit(1024, 16);										// EEPROM has 1024 bits, uses 16-bit words

	// Load the roms into memory
	if (LoadRoms()) {
		return 1;
	}

	{
		SekInit(0, 0x68000);													// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom01,						0x000000, 0x0FFFFF, SM_ROM);	// CPU 0 ROM
		SekMapMemory(Ram01,						0x100000, 0x10FFFF, SM_RAM);
		SekMapMemory(CaveSpriteRAM,				0x400000, 0x40FFFF, SM_RAM);
		SekMapMemory(CaveTileRAM[0],			0x500000, 0x507FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[1],			0x600000, 0x607FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[2] + 0x4000,	0x700000, 0x703FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[2] + 0x4000,	0x704000, 0x707FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[2] + 0x4000,	0x708000, 0x70BFFF, SM_RAM);
		SekMapMemory(CaveTileRAM[2] + 0x4000,	0x70C000, 0x70FFFF, SM_RAM);

		SekMapMemory(CavePalSrc,				0xC00000, 0xC0FFFF, SM_ROM);	// Palette RAM (write goes through handler)
		SekMapHandler(1,						0xC00000, 0xC0FFFF, SM_WRITE);	//

		SekSetReadWordHandler(0, ddonpachReadWord);
		SekSetReadByteHandler(0, ddonpachReadByte);
		SekSetWriteWordHandler(0, ddonpachWriteWord);
		SekSetWriteByteHandler(0, ddonpachWriteByte);

		SekSetWriteWordHandler(1, ddonpachWriteWordPalette);
		SekSetWriteByteHandler(1, ddonpachWriteBytePalette);

		SekClose();
	}

	nCaveRowModeOffset = 1;

	CavePalInit(0x8000);
	CaveTileInit();
	CaveSpriteInit(0, 0x1000000);
	CaveTileInitLayer(0, 0x400000, 8, 0x4000);
	CaveTileInitLayer(1, 0x400000, 8, 0x4000);
	CaveTileInitLayer(2, 0x200000, 8, 0x4000);

	YMZ280BInit(16934400, &TriggerSoundIRQ, 3);

	bDrawScreen = true;

	// US version:    0x0571AC - 0x0571B8
	// Japan version: 0x056DF4 - 0x056E00

	nSpeedhack = (strcmp(BurnDrvGetTextA(DRV_NAME), "ddonpach") == 0) ? 0x0571AC : 0x056DF4;

#if defined FBA_DEBUG && defined USE_SPEEDHACKS
	bprintf(PRINT_IMPORTANT, _T("  * Using speed-hacks (detecting idle loops).\n"));
#endif

	DrvDoReset(); // Reset machine

	return 0;
}

// Rom information
static struct BurnRomInfo ddonpachRomDesc[] = {
	{ "b1.u27",       0x080000, 0xB5CDC8D3, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "b2.u26",       0x080000, 0x6BBB063A, BRF_ESS | BRF_PRG }, //  1

	{ "u50.bin",      0x200000, 0x14B260EC, BRF_GRA },			 //  2 Sprite data
	{ "u51.bin",      0x200000, 0xE7BA8CCE, BRF_GRA },			 //  3
	{ "u52.bin",      0x200000, 0x02492EE0, BRF_GRA },			 //  4
	{ "u53.bin",      0x200000, 0xCB4C10F0, BRF_GRA },			 //  5

	{ "u60.bin",      0x200000, 0x903096A7, BRF_GRA },			 //  6 Layer 0 Tile data
	{ "u61.bin",      0x200000, 0xD89B7631, BRF_GRA },			 //  7 Layer 1 Tile data
	{ "u62.bin",      0x200000, 0x292BFB6B, BRF_GRA },			 //  8 Layer 2 Tile data

	{ "u6.bin",       0x200000, 0x9DFDAFAF, BRF_SND },			 //  9 YMZ280B (AD)PCM data
	{ "u7.bin",       0x200000, 0x795B17D5, BRF_SND },			 // 10
};


STD_ROM_PICK(ddonpach);
STD_ROM_FN(ddonpach);


// Rom information
static struct BurnRomInfo ddonpchjRomDesc[] = {
	{ "u27.bin",      0x080000, 0x2432FF9B, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "u26.bin",      0x080000, 0x4F3A914A, BRF_ESS | BRF_PRG }, //  1

	{ "u50.bin",      0x200000, 0x14B260EC, BRF_GRA },			 //  2 Sprite data
	{ "u51.bin",      0x200000, 0xE7BA8CCE, BRF_GRA },			 //  3
	{ "u52.bin",      0x200000, 0x02492EE0, BRF_GRA },			 //  4
	{ "u53.bin",      0x200000, 0xCB4C10F0, BRF_GRA },			 //  5

	{ "u60.bin",      0x200000, 0x903096A7, BRF_GRA },			 //  6 Layer 0 Tile data
	{ "u61.bin",      0x200000, 0xD89B7631, BRF_GRA },			 //  7 Layer 1 Tile data
	{ "u62.bin",      0x200000, 0x292BFB6B, BRF_GRA },			 //  8 Layer 2 Tile data

	{ "u6.bin",       0x200000, 0x9DFDAFAF, BRF_SND },			 //  9 YMZ280B (AD)PCM data
	{ "u7.bin",       0x200000, 0x795B17D5, BRF_SND },			 // 10
};


STD_ROM_PICK(ddonpchj);
STD_ROM_FN(ddonpchj);


struct BurnDriver BurnDrvDoDonpachi = {
	"ddonpach", NULL, NULL, "1997",
	"DoDonPachi (1997 2/5 master ver, international)\0", NULL, "Atlus / Cave", "Cave",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_ONLY,
	NULL, ddonpachRomInfo, ddonpachRomName, ddonpachInputInfo, NULL,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 240, 320, 3, 4
};

struct BurnDriver BurnDrvDoDonpachiJ = {
	"ddonpchj", "ddonpach", NULL, "1997",
	"DoDonPachi (1997 2/5 master ver, Japan)\0", NULL, "Atlus / Cave", "Cave",
	L"DoDonPachi (1997 2/5 master ver, Japan)\0\u6012\u9996\u9818\u8702 (1997 2/5 master ver, Japan)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_ONLY,
	NULL, ddonpchjRomInfo, ddonpchjRomName, ddonpachInputInfo, NULL,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 240, 320, 3, 4
};


